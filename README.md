# FrugalWatchdog

`FrugalWatchdog` is a simple and cheap DIY watchdog for your computer,
i.e. a small device that will automatically reset your computer if it
stops responding. It consists of a cheap USB to RS232 converter and an
ATtiny45 microcontroller. Because you can easily buy a converter that
does RS232 at TTL voltage levels, you don't even need a PCB. Check the
`img/` directory for an example build; the resistor and the LED are
entirely optional. The header that sticks out from the USB connector
is used to plug the watchdog directly to the motherboard. Total cost
is between 2€ and 5€.

This is not an original idea, but draws inspiration from other similar
projects. This one, however, has all the code available and is very
easy to extend if neccessary. It is also compatible with the
`watchdog` daemon for linux, working out of the box (the metaphorical
box, of course: you still have to do the soldering ;-) ).

## Building

The hardware build is intentionally underspecified because it is very
simple and because you should adapt it to your needs. The prototype
shown in the images is made to plug onto the motherboard of a small
home server, but you may want to use a cable and have the watchdog
plugged into an external USB port.

The pin numbers can be changed freely. The LED and computer reset pins
can be set in `main.cpp` whereas serial Rx and Tx pins can be chosen
in `softuart.h`. The ATtiny runs on its internal oscillator and it
seems that it runs best at 3.3V. At 5V, the timings for serial
communication are a bit off, so you may need to do some calibration.

To build the firmware, you need avr-gcc and avr-libc. Simply run
`make` in the `microcontroller/` directory. Upload the generated
`FrugalWatchdog.hex` file to the microcontroller however you want; the
`make upload` and `make fuses` commands use `avrdude` and ArduinoISP,
see the `Makefile`.

## Using manually

The watchdog is configured for serial communication at 2400 baud. It
recognizes the following commands:

  - `timeout`: sets the timeout in seconds. The default is 60 seconds
    to match the behaviour of the `watchdog` utility, see below.

  - `start`: starts the watchdog.

  - `stop`: stops the countdown.

  - `reset`: resets the countdown, i.e. it postpones the machine
     reset. Implies `start`. Sets the timeout string, see below.

  - `status`: prints the elapsed time since the last reset and prints
    the last timeout string.

The `timeout` and `reset` commands take an argument. It is not passed
on the same line, but on the next one. For example, a testing session
might look like this (input lines prefixed with `>`, printed lines
prefixed with `<`, comments begin with `#`):

    > status   # request the state
    < 0        # watchdog is not running, timer is at 0
    <          # we just flashed the firmware, no timeout string present
    > timeout  # set the timeout
    > 10       # provide the argument on a separate line
    # nothing is printed back unless we mistyped a command
    > start    # start the countdown, LED starts blinking
    # wait a second or two
    > status
    < 3        # we obviously waited three seconds
    <          # still no timeout string is set
    > reset    # reset the timer
    > TESTING  # arbitrary text at most 14 characters in length
    # wait a second
    > status
    < 1
    <          # still no timeout string
    # wait until the timeout expires; the LED remains on
    > status
    < 9        # the watchdog is stopped, counter is at timeout (rounded down)
    < TESTING  # the timeout string given at the last reset is printed back
    
As you can see, the `status` command prints the string that was given
at the last `reset` command before the timeout. This string is stored
in persistent memory and will not disappear if power is lost. This
way, you can store useful information (such as a timestamp) that will
tell you when the reset happened.

A script called `frugal_watchdog` is provided to make it easier to use
the watchdog. Run it with the `-h` argument to see how it is used. It
will take care of writing the timestamp and printing the date of the
last reset. Set the path to the watchdog serial device at the top of
the script.

## Using with the system daemon

There are two ways to use the watchdog. The simplest way is to set a
periodic job to run before the timeout. On unix-like systems, `cron`
will do this happily. It only provides one minute resolution though,
so you will want to increase the timeout to several minutes.

An even simpler, yet more comprehensive solution, is the `watchdog`
daemon for linux. It can monitor all sorts of things on the system to
determine its health. Refer to its documentation for more
information. For hardware watchdogs, it normally relies on the
`/dev/watchdog` interface provided by a linux driver. There is no such
driver for FrugalWatchdog as it would overcomplicate matters. Instead,
the `frugal_watchdog` script behaves like a test/repair script for the
`watchdog` daemon. Simply drop it in the `/etc/watchdog.d/` directory
and it will happily reset the watchdog hardware in every interval of
the `watchdog` daemon. If a timeout occurs, use `frugal_watchdog
status` to learn when it happened.

## Licence

Copyright 2015 Jure Varlec <jure@varlec.si>.

The package is available under the terms of GPL3. See the `COPYING`
file for more information.
