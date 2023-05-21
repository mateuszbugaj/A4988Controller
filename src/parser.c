#include "../inc/parser.h"
#include <stdlib.h> // For atoi()

typedef enum {
    STATE_WAITING_FOR_COMMAND,
    STATE_PARSING_NUMBER
} ParserState;

static ParserState state;
static char number_buffer[6]; // Enough to store a 16-bit number plus null terminator
static int number_buffer_pos;
static CommandCallback speedCommandCallback;

void parser_init(CommandCallback _speedCommandCallback) {
    speedCommandCallback = _speedCommandCallback;
    state = STATE_WAITING_FOR_COMMAND;
    number_buffer_pos = 0;
}

void parser_feed_char(char c) {
    switch (state) {
        case STATE_WAITING_FOR_COMMAND:
            if (c == 'F') {
                // We're starting a speed command. Switch to parsing the number.
                state = STATE_PARSING_NUMBER;
                number_buffer_pos = 0;
            }
            break;

        case STATE_PARSING_NUMBER:
            if (c >= '0' && c <= '9') {
                // It's a digit. Add it to the buffer.
                if (number_buffer_pos < sizeof(number_buffer) - 1) {
                    number_buffer[number_buffer_pos++] = c;
                } else {
                    // The number is too long. Ignore this digit.
                }
            } else if (c == '\n' || c == '\r') {
                // End of the command. Convert the number and call the callback.
                number_buffer[number_buffer_pos] = '\0'; // Null-terminate the string
                uint16_t number = atoi(number_buffer);
                speedCommandCallback(number);
                state = STATE_WAITING_FOR_COMMAND;
            }
            break;
    }
}
