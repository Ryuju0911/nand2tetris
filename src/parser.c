#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "parser.h"

int parse_command_type(char *line);
char *parse_a_command(char *line);
void remove_whitespace(char *line);
void format_line(char *line);

/**
 * \brief Return the type of the current command. 
 * \note There are three types of commands, which is A, C, and L.
 * \param[in] line: A line.
 * \param[out] cuurent_command: The type of the current command.
 */
int parse_command_type(char *line) {
    format_line(line);
    
    command_type cur_command_type;

    if (strlen(line) == 0) {
        return -1;
    }

    if (line[0] == '@') {
        cur_command_type = A_COMMAND;
    } else if (line[0] == '(') {
        cur_command_type = L_COMMAND;
    } else {
        cur_command_type = C_COMMAND;
    }

    return cur_command_type;
}


/**
 * \brief Return the symbol or decimal Xxx of the current command @Xxx.
 * \note This function should be called only when the current command is A-command.
 * 		 Do not forget to free symbol.
 * \param[in] line: A line.
 * \param[out] symbol: The symbol or decimal of the current command.
 */
char *parse_a_command(char *line) {
    char *symbol = (char*)malloc(strlen(line) * sizeof(char));
	line++; /* index 0 is "@", and it is unnecessary. */
	int i = 0;
	while (*line != '\0') {
		symbol[i] = *line;
		line++;
		i++;
	}
	symbol[i] = '\0';

    return symbol;
}


/**
 * \brief Return the symbol or decimal Xxx of the current command (Xxx).
 * \note This function should be called only when the current command is L-command.
 * 		 Do not forget to free symbol.
 * \param[in] line: A line.
 * \param[out] symbol: The symbol of the current command.
 */
char *parse_l_command(char *line) {
    char *symbol = (char*)malloc(strlen(line) * sizeof(char));
	line++; /* index 0 is "(", and it is unnecessary. */
	int i = 0;
	while (*line != '\0') {
		symbol[i] = *line;
		line++;
		i++;
	}
	symbol[i - 1] = '\0';

    return symbol;
}


/**
 * \brief Return the dest, comp, jump mnemonic in the current command.
 * \note This function should be called only when the current command is C-command.
 * 		 Do not forget to free a cur_command and its members.
 * \param[in] line: A line 
 * \param[out] cur_command: The structure which contains dest, comp, jump mnemonic
 */
c_mnemonic *parse_c_command(char *line) {
	c_mnemonic *cur_command = (c_mnemonic*)malloc(sizeof(c_mnemonic));

	cur_command->dest = (char*)malloc(4 * sizeof(char));
	cur_command->comp = (char*)malloc(4 * sizeof(char));
	cur_command->jump = (char*)malloc(4 * sizeof(char));

	if (strstr(line, "=")) {
		int i = 0;
		while (*line != '=') {
			cur_command->dest[i] = *line;
			line++;
			i++;
		}
		cur_command->dest[i] = '\0';
		line++;
	} else {
		cur_command->dest = "";
	}

	int j = 0;
	if (strstr(line, ";")) {
		while (*line != ';') {
			cur_command->comp[j] = *line;
			line++;
			j++;
		}
		line++;
	} else {
		while (*line != '\0') {
			cur_command->comp[j] = *line;
			line++;
			j++;
		}
	}
	cur_command->comp[j] = '\0';

	int k = 0;
	while (*line != '\0') {
		cur_command->jump[k] = *line;
		line++;
		k++;
	}
	cur_command->jump[k] = '\0';

	return cur_command;
}

/**
 * \brief Format a line by removing whitespace and comment.
 * \note This function enables line ready to be parsed.
 * \param[in] line: A line.
 * \param[out] line: Formatted line.
 */
void format_line(char *line) {
    remove_whitespace(line);
    char *comment = strstr(line, "//");
    if (comment != NULL) {
        *comment = '\0';
        remove_whitespace(line);
    }
}


/**
 * \brief Remove whitespace from a line.
 * \param[in] line: A line.
 */
void remove_whitespace(char *line) {
	char *p = line;
	
	while (*line != '\0') {
		if (!isspace(*line)) {
			*p = *line;
			p++;
		}
		line++;
	}
	*p = '\0';
}
