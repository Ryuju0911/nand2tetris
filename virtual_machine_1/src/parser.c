#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "parser.h"


/**
 * \brief Return the type of the current command. 
 * \note There are nine types of commands, and they are defined as command_type structure in parser.h.
 * \param[in] line: A line.
 * \param[out] cuurent_command: The type of the current command.
 */
int parse_command_type(char *line) {
    command_type cur_command_type;

    if (strlen(line) == 0) {
        return -1;
    }

	char *cur_command = (char*)malloc(50 * sizeof(char));
	int i = 0;
	while (*line != ' ') {
            cur_command[i] = *line;
            line++;
            i++;
        }
        cur_command[i] = '\0';

    if (strncmp(cur_command, "push", 4) == 0) {
        cur_command_type = C_PUSH;
    } else if (strncmp(cur_command, "pop", 3) == 0) {
        cur_command_type = C_POP;
    } else if (strncmp(cur_command, "label", 5) == 0) {
		cur_command_type = C_LABEL;
	} else if (strncmp(cur_command, "goto", 4) == 0) {
		cur_command_type = C_GOTO;
	} else if (strncmp(cur_command, "if-goto", 7) == 0) {
		cur_command_type = C_IF;
	} else if (strncmp(cur_command, "function", 8) == 0) {
		cur_command_type = C_FUNCTION;
	} else if (strncmp(cur_command, "call", 4) == 0) {
		cur_command_type = C_CALL;
	} else if (strncmp(cur_command, "return", 6) == 0) {
		cur_command_type = C_RETURN;
	} else {
		cur_command_type = C_ARITHMETIC;
	}

	free(cur_command);
    return cur_command_type;
}


/**
 * \brief Return the first argument of the current command.
 * \note This function should not be called if the current command is C_RETURN.
 *       In the case of C_ARITHEMETIC, the command itself (add, sub, etc.) is returned.
 * 		 Do not forget to free arg1.
 * \param[in] line: A line.
 * \param[out] arg1: The first argument of the current command.
 */
char *parse_arg1(char *line) {
    char *arg1 = (char*)malloc(50 * sizeof(char));
    int i = 0;

    if (parse_command_type(line) == C_ARITHMETIC) {
        while (*line != '\0') {
            arg1[i] = *line;
            line++;
            i++;
        }
        arg1[i] = '\0';
    } else {
        while (*line != ' ') {
            line++;
        }
        line++;
        while (*line != ' ') {
            arg1[i] = *line;
            line++;
            i++;
        }
        arg1[i] = '\0';
    }

    return arg1;
}


/**
 * \brief Return the second argument of the current command. The argument is converted to int from char.
 * \note This function should be called only if the current command is C_PUSH, C_POP, C_FUNCTION, or C_CALL.
 * \param[in] line: A line.
 * \param[out] arg2: The second argument of the current command.
 */
int parse_arg2(char *line) {
	command_type cur_command_type = parse_command_type(line);
	if (cur_command_type != C_PUSH && cur_command_type != C_POP && cur_command_type != C_FUNCTION && cur_command_type != C_CALL) {
		return -1;
	}

    int arg2;

    while (*line != ' ') {
            line++;
        }
    line++;

    while (*line != ' ') {
            line++;
        }
    line++;

	arg2 = atoi(line);

    return arg2;
}


/**
 * \brief Remove comment from a line.
 * \param[in] line: A line 
 */
void remove_comment(char *line) {
	char *comment = strstr(line, "//");
	if (comment != NULL) {
		*comment = '\0';
	}
}

