#ifndef PARSER_H
#define PARSER_H

typedef enum {
    A_COMMAND,
    C_COMMAND,
    L_COMMAND,
} command_type;

typedef struct {
    char *dest;
    char *comp;
    char *jump;
} c_mnemonic;

int parse_command_type(char *line);
char *parse_a_command(char *line);
char *parse_l_command(char *line);
c_mnemonic *parse_c_command(char *line);
void remove_whitespace(char *line);
void format_line(char *line);

#endif