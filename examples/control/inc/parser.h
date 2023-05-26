#ifndef _PARSER_H_
#define _PARSER_H_

#include <avr/io.h>

typedef void (*CommandCallback)(uint16_t parameter);

typedef struct {
    char command;
    CommandCallback callback;
} parser_command_handler_t;

void parser_init(parser_command_handler_t* handlers, uint8_t num_handlers);
void parser_feed_char(char c);

#endif /* _PARSER_H_ */