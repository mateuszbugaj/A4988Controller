#include "../inc/parser.h"
#include "../inc/usart.h"
#include <stdlib.h>

#define NUM_BUFFER_SIZE 6  // Enough to store a 16-bit number plus null terminator

typedef enum {
    STATE_WAITING_FOR_COMMAND,
    STATE_PARSING_NUMBER
} ParserState;

typedef struct {
    ParserState state;
    char num_buffer[NUM_BUFFER_SIZE];
    int num_buffer_pos;
    parser_command_handler_t* command_handlers;
    uint8_t num_handlers;
    char lastCommand;
    uint8_t isNegative;   // Added this flag
} ParserContext;

static ParserContext context;

void parser_init(parser_command_handler_t* handlers, uint8_t handlers_count) {
    context.command_handlers = handlers;
    context.num_handlers = handlers_count;
    context.state = STATE_WAITING_FOR_COMMAND;
    context.num_buffer_pos = 0;
    context.isNegative = 0;
}

void parser_feed_char(char c) {
    switch (context.state) {
        case STATE_WAITING_FOR_COMMAND:
            context.lastCommand = '\0';
            for (uint8_t i = 0; i < context.num_handlers; i++) {
                if (context.command_handlers[i].command == c) {
                    context.state = STATE_PARSING_NUMBER;
                    context.num_buffer_pos = 0;
                    context.isNegative = 0;  // Reset the flag when we start a new number
                    context.lastCommand = c;
                    break;
                }
            }
            break;
        case STATE_PARSING_NUMBER:
            if (c == '-') {
                // Handle the minus sign
                context.isNegative = 1;
            } else if (c >= '0' && c <= '9') {
                // It's a digit. Add it to the buffer.
                if (context.num_buffer_pos < NUM_BUFFER_SIZE - 1) {
                    context.num_buffer[context.num_buffer_pos++] = c;
                }
            } else if (c == '\n' || c == '\r') {
                // End of the command. Convert the number and call the callback.
                context.num_buffer[context.num_buffer_pos] = '\0'; // Null-terminate the string
                int16_t number = atoi(context.num_buffer); // Convert the string to an integer
                if (context.isNegative) {
                    // If the number was negative, negate it
                    number = -number;
                }
                for (uint8_t i = 0; i < context.num_handlers; i++) {
                    if (context.command_handlers[i].command == context.lastCommand) {
                        context.command_handlers[i].callback(number);
                        break;
                    }
                }
                context.state = STATE_WAITING_FOR_COMMAND;
            }
            break;
    }
}