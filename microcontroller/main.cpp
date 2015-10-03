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

using byte = unsigned char;

// List of commands, terminated by the "invalid" handler.
enum class Command: byte
{
    ledOn,
    ledOff,
    ledBlinkMs,
    _enumSize
};

// Declaration of commands.
using CommandFunc = void (*)();
static void _cmd_ledOn();
static void _cmd_ledOff();
static void _cmd_ledBlinkMs();

// Command names to be received, indexed by the command enum.
static const char _cmd1_string[] PROGMEM = "led on";
static const char _cmd2_string[] PROGMEM = "led off";
static const char _cmd3_string[] PROGMEM = "blink";
static const char* const commandStrings[] = {
    _cmd1_string,
    _cmd2_string,
    _cmd3_string,
};

#define CMDNUM (sizeof(commandStrings) / sizeof(char*))

// A list of commands, indexed by the command enum.
static const CommandFunc commands[] = {
    _cmd_ledOn,
    _cmd_ledOff,
    _cmd_ledBlinkMs,
};

// Sanity checks for command list consistency.
static_assert(sizeof(commands) / sizeof(CommandFunc) == CMDNUM,
	      "Sizes of command_strings and commands differ.");
static_assert(CMDNUM == (byte)Command::_enumSize,
	      "Sizes of Command and commandStrings differ.");

static const char newline[] PROGMEM = "\n\r";

FastPin<4> led;

int main()
{
    char tmp[10];
    RecvCmd<10, 3> cmdReceiver;

    softuart_init();
    sei();

    _delay_ms(2000);
    softuart_puts_P("Commands: ");
    sprintf(tmp, "%d", CMDNUM);
    softuart_puts(tmp);
    softuart_puts_p(newline);
    for (byte i = 0; i < CMDNUM; ++i) {
	cmdReceiver.addCommand_P(commandStrings[i]);
	softuart_puts_P("   ");
	softuart_puts_p(commandStrings[i]);
	softuart_puts_p(newline);
    }
    softuart_puts_P("You should enable local echo.\r\n");
    softuart_puts_p(newline);

    led.output();
    led.low();

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

void _cmd_ledOn()
{
    softuart_puts_P("Turning LED on.\n\r");
    led.high();
}

void _cmd_ledOff()
{
    softuart_puts_P("Turning LED off.\n\r");
    led.low();
}

void _cmd_ledBlinkMs()
{
    softuart_puts_P("Specify the number of ms or 'default': ");
    RecvCmd<10, 1> cmdReceiver;
    cmdReceiver.addCommand_P(PSTR("default"));
    char status;
    while (-1 == (status = cmdReceiver.addChar(softuart_getchar())));
    if (status == 0) {
	led.toggle();
	_delay_ms(1333);
	led.toggle();
    } else {
	int ms = strtol(cmdReceiver.buffer(), 0, 0);
	if (ms < 0)
	    return;
	led.toggle();
	for (int i = 0; i < ms; ++i)
	    _delay_ms(1);
	led.toggle();
    }
}
