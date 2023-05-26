#include "../inc/parser.h"
#include "../inc/usart.h"
#include <stdlib.h>

#define NUM_BUFFER_SIZE 6  // Enough to store a 16-bit number plus null terminator

typedef enum {
    STATE_WAITING_FOR_COMMAND,
    STATE_PARSING_NUMBER
} ParserState;

static ParserState state;
static char num_buffer[NUM_BUFFER_SIZE];
static int num_buffer_pos;
static parser_command_handler_t* command_handlers;
static uint8_t num_handlers;
static char lastCommand;

void parser_init(parser_command_handler_t* handlers, uint8_t handlers_count) {
    command_handlers = handlers;
    num_handlers = handlers_count;
    state = STATE_WAITING_FOR_COMMAND;
    num_buffer_pos = 0;
}

void parser_feed_char(char c) {
    switch (state) {
        case STATE_WAITING_FOR_COMMAND:
            lastCommand = '\0';
            for (uint8_t i = 0; i < num_handlers; i++) {
                if (command_handlers[i].command == c) {
                    state = STATE_PARSING_NUMBER;
                    num_buffer_pos = 0;
                    lastCommand = c;
                    break;
                }
            }
            break;
        case STATE_PARSING_NUMBER:
            if (c >= '0' && c <= '9') {
                // It's a digit. Add it to the buffer.
                if (num_buffer_pos < NUM_BUFFER_SIZE - 1) {
                    num_buffer[num_buffer_pos++] = c;
                }
            } else if (c == '\n' || c == '\r') {
                // End of the command. Convert the number and call the callback.
                num_buffer[num_buffer_pos] = '\0'; // Null-terminate the string
                uint16_t number = atoi(num_buffer);
                for (uint8_t i = 0; i < num_handlers; i++) {
                    if (command_handlers[i].command == lastCommand) {
                        command_handlers[i].callback(number);
                        break;
                    }
                }
                state = STATE_WAITING_FOR_COMMAND;
            }
            break;
    }
}