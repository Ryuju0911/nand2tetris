#ifndef PARSER_H
#define PARSER_H

typedef enum {
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL,
} command_type;


int parse_command_type(char *line);
char *parse_arg1(char *line);
int parse_arg2(char *line);
void remove_comment(char *line);


#endif
