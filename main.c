#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "utils.h"
#include "code_write.h"

#define LINE_LENGTH 100


/* global variables */
char file_name[100];
int label_count;
char line[LINE_LENGTH];


int main(int argc, char *argv[]) {
    /* check the number of the argments */
    if (argc < 3) {
        fprintf(stderr, "the arguments are not enough\n");
        exit(1);
    }

    /* make the asm file and open it in write mode */
    FILE *asm_file = fopen(argv[1], "w");
    if (asm_file == NULL) {
        fprintf(stderr, "cannot make the asm file.\n");
        exit(1);
    }

    write_init(asm_file);

    for (int i=2; i<argc; i++) {
        /* check whether the passed file is vm file */
        if (!strstr(argv[i], ".vm")) {
            fprintf(stderr, "the passed file is not vm file.\n");
            exit(1);
        }

        /* open the vm file in read mode */
        FILE *vm_file = fopen(argv[i], "r");
        if (vm_file == NULL) {
            fprintf(stderr, "cannot open the vm file.\n");
            exit(1);
        }

        /* get the vm file name from path. */
        get_file_name(argv[i]);

        /* read one line, tlansalate it and write it into asm_file */
        while (fgets(line, sizeof(line), vm_file) != NULL) {
            vm_translator(asm_file, line);
        }

        fclose(vm_file);
    }
    
    fclose(asm_file);

    return 0;
}