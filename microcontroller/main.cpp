#include "FastPin.h"
#include "RecvCmd.h"
extern "C" {
#include "softuart.h"
}
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

using byte = unsigned char;

// Declaration of commands.
using CommandFunc = void (*)();
static void _cmd_setTimeout();
static void _cmd_start();
static void _cmd_stop();
static void _cmd_reset();
static void _cmd_status();

// Command names to be received.
static const char _cmd1_string[] PROGMEM = "timeout";
static const char _cmd2_string[] PROGMEM = "start";
static const char _cmd3_string[] PROGMEM = "stop";
static const char _cmd4_string[] PROGMEM = "reset";
static const char _cmd5_string[] PROGMEM = "status";
static const char* const commandStrings[] = {
    _cmd1_string,
    _cmd2_string,
    _cmd3_string,
    _cmd4_string,
    _cmd5_string,
};

#define CMDNUM (sizeof(commandStrings) / sizeof(char*))

// The list of commands for easier calling.
static const CommandFunc commands[] = {
    _cmd_setTimeout,
    _cmd_start,
    _cmd_stop,
    _cmd_reset,
    _cmd_status,
};

// Sanity check for command list consistency.
static_assert(sizeof(commands) / sizeof(CommandFunc) == CMDNUM,
	      "Sizes of command_strings and commands differ.");


static void writeEEPROM(byte address, const char* source, byte length)
{
   EECR = 0;
   EEAR = address;
   for (byte i = 0; i < length; ++i) {
       EEDR = source[i];
       FAST_SET(EECR, EEMPE);
       FAST_SET(EECR, EEPE);
       while (FAST_GET(EECR, EEPE));
       EEAR = ++address;
   }
}

static void write1EEPROM(byte address, byte c)
{
    EECR = 0;
    EEAR = address;
    EEDR = c;
    FAST_SET(EECR, EEMPE);
    FAST_SET(EECR, EEPE);
    while (FAST_GET(EECR, EEPE));
}

static inline byte read1EEPROM(byte address)
{
    EEAR = address;
    FAST_SET(EECR, EERE);
    return EEDR;
}

/*
static void readEEPROM(byte address, char* dest, byte length)
{
    EEAR = address;
    for (byte i = 0; i < length; ++i) {
        FAST_SET(EECR, EERE);
        dest[i] = EEDR;
        ++EEAR;
    }
}
*/

static const unsigned long timerTick_us = 499712;

// Variables shared between subroutines.
static FastPin<4> ledPin;
static FastPin<3> resetPin;

// Set default timeout of one minute.
static unsigned long timeoutTicks = 60 * 1000000 / timerTick_us;;
static unsigned long ticks = 0;

// The timestamp is meant to be seconds from epoch in decimal, but can
// be anything really.
static char lastTimestamp[15];

int main()
{
    resetPin.low();
    resetPin.input();
    ledPin.low();
    ledPin.output();

    // Ensure that whatever is in the EEPROM is null-terminated. If it
    // wasn't yet, we have just been flashed so set the stored
    // timestamp to a null string.
    if (0 != read1EEPROM(sizeof(lastTimestamp))) {
	write1EEPROM(sizeof(lastTimestamp), 0);
	write1EEPROM(0, 0);
    }

    softuart_init();

    // Setup Timer1.
    TCCR1 = 15;  // Prescaler period 2^14.
    FAST_SET(TCCR1, CTC1);  // CTC mode with OCR1C register.
    // Set the counter interval to 244, which produces the period of
    // just under half a second (timerTick_us).
    OCR1C = 244;
    OCR1A = 244;
    // Timer1 interrupt will only be enabled once the timeout is known.

    sei();

    _delay_ms(2000);
    RecvCmd<16, CMDNUM> cmdReceiver;
    softuart_puts_P("\r\n"
		    "Frugal Watchdog started.\r\n"
		    "\r\n"
		    "Kuza pazi,\r\n"
		    "z repom miga,\r\n"
		    "vtane, leze, tacko da!\r\n"
		    "\r\n");
    for (byte i = 0; i < CMDNUM; ++i) {
	cmdReceiver.addCommand_P(commandStrings[i]);
    }

    softuart_turn_rx_on();
    for (;;) {
	auto status = cmdReceiver.addChar(softuart_getchar());
	if (status == -2) {
	    softuart_puts_P("Invalid command!\n\r");
	    cmdReceiver.reset();
	} else if (status >= 0) {
	    commands[(byte)status]();
	    cmdReceiver.reset();
	}
    }

    return 0;
}

ISR(TIM1_COMPA_vect, ISR_NOBLOCK)
{
    ledPin.toggle();
    if (++ticks > timeoutTicks) {
	// Timeout occured, record the timestamp and reset the machine.
	FAST_CLR(TIMSK, OCIE1A);
	ticks = timeoutTicks;
	byte n = strlen(lastTimestamp);
	writeEEPROM(0, lastTimestamp, n);
	write1EEPROM(n, 0);
	ledPin.high();
	resetPin.output();
	_delay_ms(1000);
	resetPin.input();
    }
}

static void _cmd_setTimeout()
{
    RecvCmd<10, 0> timeoutReceiver;
    while (-1 == timeoutReceiver.addChar(softuart_getchar()));
    unsigned int seconds = strtol(timeoutReceiver.buffer(), 0, 0);
    timeoutTicks = seconds * 1000000 / timerTick_us;
}

static void _cmd_start()
{
    if (!timeoutTicks)
    	return;
    TCNT1 = 0;
    FAST_SET(TIMSK, OCIE1A);  // Interrupt on match with OCR1A.
    ledPin.low();
}

static void _cmd_stop()
{
    FAST_CLR(TIMSK, OCIE1A);
    ledPin.low();
}

static void _cmd_reset()
{
    char c = 0;
    byte i = 0;
    while ('\r' != (c = softuart_getchar())) {
	if (c == '\n')
	    continue;
	lastTimestamp[i] = c;
	i = (i + 1) % sizeof(lastTimestamp);
    }
    lastTimestamp[i] = 0;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
	ticks = 0;
    }

    // Reset also starts the watchdog. This way, it will also function
    // with no configuration.
    _cmd_start();
}

static void _cmd_status()
{
    unsigned long elapsed;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
	elapsed = ticks;
    }
    elapsed = elapsed * timerTick_us / 1000000;
    char tmp[10];
    byte i;
    if (elapsed) {
	tmp[sizeof(tmp)-1] = 0;
	for (i = sizeof(tmp)-2; i > 0 && elapsed > 0; --i) {
	    tmp[i] = '0' + elapsed % 10;
	    elapsed /= 10;
	}
	softuart_puts(tmp + i + 1);
    } else {
	softuart_putchar('0');
    }
    softuart_puts_P("\r\n");

    // Print the last stored timestamp.
    byte c;
    i = 0;
    while ((c = read1EEPROM(i++)))
	softuart_putchar(c);
    softuart_puts_P("\r\n");
}
