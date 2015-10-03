#ifndef RECV_CMD_H
#define RECV_CMD_H

#include <string.h>
#include <avr/pgmspace.h>

template<unsigned char recvBufferSize, unsigned char maxCommands = 10>
class RecvCmd
{
 public:
    using byte = unsigned char;

    RecvCmd() {
	reset();
    }

    // Add a command string to the list of recognized commands. The
    // string must be stored in PROGMEM.
    void addCommand_P(const char* cmdstring) {
	cmdStrings[numCmds++] = cmdstring;
    }

    // Resets the command buffer.
    void reset() {
	lastChar = 0;
	awaitNewline = false;
	memset(cmd, 0, recvBufferSize + 1);
    }

    /*
      Adds another character to command. If the command is not
      complete yet, it returns -1. If c is a newline, the command is
      complete. If the command is recognized, a non-negative number is
      returned specifying the command received, otherwise, -2 is
      returned.

      Note: newline is really CR, whereas LF characters are
      just ignored.

      Note: the buffer is never automatically reset, which allows you
      to access the invalid string. It can be echoed to the user, or
      it may contain a parseable parameter.
    */
    char addChar(char c);

    // Return the pointer to the command buffer. The string is always
    // null-terminated.
    const char* buffer() const {
	return cmd;
    }

    // Get the list of known commands.
    const char* commands() const {
	return cmdStrings;
    }

    // Get the number of known commands.
    byte numCommands() const {
	return numCmds;
    }

private:
    char cmd[recvBufferSize + 1];  // Leave space for null.
    byte lastChar;
    bool awaitNewline;
    const char* cmdStrings[maxCommands];
    byte numCmds = 0;
};


template<unsigned char recvBufferSize, unsigned char maxCommands>
char RecvCmd<recvBufferSize, maxCommands>::addChar(char c)
{
    if (c == '\n') {
	// Ignore LF.
    } else if (c != '\r') {
	// No newline, just add c to the buffer.
	cmd[lastChar] = c;
	lastChar = (lastChar + 1) % recvBufferSize;
    } else {
	// We have a newline, let's see which command we've got.
	for (byte i = 0; i < numCmds; ++i) {
	    if (0 == strcmp_P(cmd, cmdStrings[i])) {
		return i;
	    }
	}
	// None found, error out.
	return -2;
    }
    return -1;
}

#endif
