#ifndef _PARSER_H_
#define _PARSER_H_

#include <avr/io.h>

typedef void (*CommandCallback)(uint16_t parameter);

void parser_init(CommandCallback speedCommandCallback);
void parser_feed_char(char c);

#endif /* _PARSER_H_ */