#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "hashmap.h"
#include "code.h"

#define LINE_LENGTH 100

char *decimal_to_binary(int num, char *binary_code);

int main(int argc, char *argv[]) {
    /* check the number of the argments */
    if (argc < 2) {
        printf("the arguments are not enough\n");
        return -1;
    }

    /* check whether the passed file is asm file */
    if (!strstr(argv[1], ".asm")) {
        printf("the passed file is not asm file\n");
        return -1;
    }

    /* get the asm file name and define hack fine name */
    char hack_file_name[strlen(argv[1]) + 2];
    strcpy(hack_file_name, argv[1]);
    char *extension_pos = strstr(hack_file_name, "asm");
    *extension_pos = '\0';
    strcat(hack_file_name, "hack");

    /* open the asm file in read mode */
    FILE *asm_file = fopen(argv[1], "r");
    if (asm_file == NULL) {
        printf("cannot open the asm file.\n");
        return -1;
    }

    /* make the hack file and open it in write mode */
    FILE *hack_file = fopen(hack_file_name, "w");
    if (hack_file == NULL) {
        printf("cannot make the hack file.\n");
        return -1;
    }

    /* consturuct the necessary hashmaps */
    HashMap *dest_map = newHashMap();
    HashMap *comp_map = newHashMap();
    HashMap *jump_map = newHashMap();
    HashMap *symbol_map = newHashMap();
    create_dest_map(dest_map);
    create_comp_map(comp_map);
    create_jump_map(jump_map);
    create_symbol_map(symbol_map);

    char line[LINE_LENGTH]; /* character array to store each line in the asm file */

    /*
     * first path
     * Go through the entire asm file, line by line, and
     * build the symbol hashmap without generating any code.
     */
    int line_count = 0;
    while (fgets(line, sizeof(line), asm_file) != NULL) {
        format_line(line);
        if (*line == '\0') {
            continue;
        }
        if (parse_command_type(line) == L_COMMAND) {
            char line_address[17] = "";
            sprintf_s(line_address, sizeof(line_address), "%d", line_count);
            char *target = parse_l_command(line);
            set(symbol_map, target, line_address);
            free(target);
        } else {
            line_count++;
        }
    }

    /* set the file pointer at the beginning of the file */
    fseek(asm_file, 0, SEEK_SET);

    /*
     * second path 
     * Go again thorugh the entire program, and parse each line.
     */
    int var_count = 0;
    while (fgets(line, sizeof(line), asm_file) != NULL) {
        if (parse_command_type(line) == C_COMMAND) {
            /* the current command type is C */
            c_mnemonic *cur_command = parse_c_command(line);
            char byte_code[17] = "";

            strcat(byte_code, "111");
            strcat(byte_code, get(comp_map, cur_command->comp));
            strcat(byte_code, get(dest_map, cur_command->dest));
            strcat(byte_code, get(jump_map, cur_command->jump));

            fprintf(hack_file, byte_code);
            fprintf(hack_file, "\n");

            free(cur_command);
        } else if (parse_command_type(line) == A_COMMAND) {
            /* the current command type is A */
            char *symbol = parse_a_command(line);
            char byte_code[17] = "";

            if ((symbol[0] >= 'a' && symbol[0] <= 'z') || (symbol[0] >= 'A' && symbol[0] <= 'Z')) {
                /* symbol is an alphabet */
                if (get(symbol_map, symbol)) {
                    /* symbol has already been set in the hashmap */
                    char *address = get(symbol_map, symbol);
                    fprintf(hack_file, decimal_to_binary(atoi(address), byte_code));
                    fprintf(hack_file, "\n");
                } else {
                    /* symbol has not been set in the table yet */
                    char var_address[17] = "";
                    sprintf_s(var_address, sizeof(var_address), "%d", var_count + 16);
                    set(symbol_map, symbol, var_address);
                    fprintf(hack_file, decimal_to_binary(atoi(var_address), byte_code));
                    fprintf(hack_file, "\n");
                    var_count++;
                }
            } else {
                /* symbol is a decimal */
                fprintf(hack_file, decimal_to_binary(atoi(symbol), byte_code));
                fprintf(hack_file, "\n");
            }
            free(symbol);
        }
    }

    /* free the hashmaps */
    free(dest_map->list);
    free(dest_map);
    free(comp_map->list);
    free(comp_map);
    free(jump_map->list);
    free(jump_map);
    free(symbol_map->list);
    free(symbol_map);

    /* close the files */
    fclose(asm_file);
    fclose(hack_file);

    return 0;
}

/**
 * \brief Convert decimal number to 16 dgits binary code.
 * \param[in] num: Decimal number 
 * \param[out] binary_code: Character array to store binary code.
 * \return binary_code: 16 digits binary code.
 */
char *decimal_to_binary(int num, char *binary_code) {
    for (int i = 15; i >= 0; i--) {        
        /* calculate bitmask to check whether i-th bit of num is set or not */
        int mask = (1 << i);
        
        if (num & mask) {
            /* i-th bit of num is set */
            binary_code[15 - i] = '1';
        } else {
            /* i-th bit of num is not set */
            binary_code[15 - i] = '0';
        }
    }
    binary_code[16] = '\0';

    return binary_code;
}
