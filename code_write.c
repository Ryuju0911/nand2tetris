#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code_write.h"
#include "utils.h"
#include "parser.h"


// prototype declaration
void write_push(FILE *fp, char *segment, int num);
void write_pop(FILE *fp, char *segment, int num);
void write_add(FILE *fp);
void write_sub(FILE *fp);
void write_neg(FILE *fp);
void write_eq(FILE *fp, int label_count);
void write_gt(FILE *fp, int label_count);
void write_lt(FILE *fp, int label_count);
void write_and(FILE *fp);
void write_or(FILE *fp);
void write_not(FILE *fp);


/* global variables */
char file_name[100];
int label_count = 0;


/**
 * \brief translate vm code into asm code, and write them into asm file.
 *        This function does all the procedures for translataion.
 * \param[in] fp: A file to be written code
 * \param[in] line: A translated line
 */
void vm_translator(FILE *fp, char *line) {
    remove_comment(line);

    if (strlen(line) == 0) {
        return;
    }

    command_type cur_command_type = parse_command_type(line);
    char *arg1 = parse_arg1(line);
    int arg2 = parse_arg2(line);

    /* write code according to a current instruction */
    if (cur_command_type == C_PUSH) {
        write_push(fp, arg1, arg2);
    } else if (cur_command_type == C_POP) {
        write_pop(fp, arg1, arg2);
    } else if (cur_command_type == C_ARITHMETIC) {
        if (strncmp(arg1, "add", 3) == 0) {
            write_add(fp);
        } else if (strncmp(arg1, "sub", 3) == 0) {
            write_sub(fp);
        } else if (strncmp(arg1, "neg", 3) == 0) {
            write_neg(fp);
        } else if (strncmp(arg1, "eq", 2) == 0) {
            write_eq(fp, label_count);
            label_count++;
        } else if (strncmp(arg1, "gt", 2) == 0) {
            write_gt(fp, label_count);
            label_count++;
        } else if (strncmp(arg1, "lt", 2) == 0) {
            write_lt(fp, label_count);
            label_count++;
        } else if (strncmp(arg1, "and", 3) == 0) {
            write_and(fp);
        } else if (strncmp(arg1, "or", 2) == 0) {
            write_or(fp);
        } else if (strncmp(arg1, "not", 3) == 0) {
            write_not(fp);
        }
    }
    
    free(arg1);
}

/**
 * \brief Write assembly code corresponding to "push constant n".
 * \note This function should be called if the current command is C_PUSH.
 * \param[in] fp: A file to be written code
 * \param[in] segment: The segment where the num is pushed
 * \param[in] num: A pushed number
 */
void write_push(FILE *fp, char *segment, int index) {
    fprintf(fp, "// push %s %d\n", segment, index);

    if (strncmp(segment, "constant", 8) == 0) {
        fprintf(fp, "@%d\n", index);
        fprintf(fp, "D=A\n");
    } else if (strncmp(segment, "argument", 8) == 0) {
        fprintf(fp, "@%d\n", index);
        fprintf(fp, "D=A\n");
        fprintf(fp, "@ARG\n");
        fprintf(fp, "A=M+D\n");
        fprintf(fp, "D=M\n");
    } else if (strncmp(segment, "local", 5) == 0) {
        fprintf(fp, "@%d\n", index);
        fprintf(fp, "D=A\n");
        fprintf(fp, "@LCL\n");
        fprintf(fp, "A=M+D\n");
        fprintf(fp, "D=M\n");
    } else if (strncmp(segment, "this", 4) == 0) {
        fprintf(fp, "@%d\n", index);
        fprintf(fp, "D=A\n");
        fprintf(fp, "@THIS\n");
        fprintf(fp, "A=M+D\n");
        fprintf(fp, "D=M\n");
    } else if (strncmp(segment, "that", 4) == 0) {
        fprintf(fp, "@%d\n", index);
        fprintf(fp, "D=A\n");
        fprintf(fp, "@THAT\n");
        fprintf(fp, "A=M+D\n");
        fprintf(fp, "D=M\n");
    } else if (strncmp(segment, "pointer", 7) == 0) {
        if (index != 0 && index != 1) {
            fprintf(stderr, "invalid index.\n The pointer segment should indicate 0 or 1.\n");
            exit(1);
        }
        fprintf(fp, "@%d\n", index);
        fprintf(fp, "D=A\n");
        fprintf(fp, "@3\n");
        fprintf(fp, "A=A+D\n");
        fprintf(fp, "D=M\n");
    } else if (strncmp(segment, "static", 6) == 0) {
        fprintf(fp, "@%s.%d\n", file_name, index);
        fprintf(fp, "D=M\n");
    } else if (strncmp(segment, "temp", 4) == 0) {
        fprintf(fp, "@%d\n", index);
        fprintf(fp, "D=A\n");
        fprintf(fp, "@5\n");
        fprintf(fp, "A=A+D\n");
        fprintf(fp, "D=M\n");
    }

    fprintf(fp, "@SP\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "M=D\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=M+1\n");        
}


/**
 * \brief Write assembly code corresponding to "pop" command in vm file.
 * \param fp: A file to be written code 
 * \param[in] segment: The segment where popped number is stored 
 * \param[in] index: The index of the segment 
 */
void write_pop(FILE *fp, char *segment, int index) {
    fprintf(fp, "// pop %s %d\n", segment, index);

    if (strncmp(segment, "argument", 8) == 0) {
        fprintf(fp, "@%d\n", index);
        fprintf(fp, "D=A\n");
        fprintf(fp, "@ARG\n");
        fprintf(fp, "D=M+D\n");
        fprintf(fp, "@R13\n");
        fprintf(fp, "M=D\n");
        fprintf(fp, "@SP\n");
        fprintf(fp, "AM=M-1\n");
        fprintf(fp, "D=M\n");
        fprintf(fp, "@R13\n");
        fprintf(fp, "A=M\n");
        fprintf(fp, "M=D\n");
    } else if (strncmp(segment, "local", 5) == 0) {
        fprintf(fp, "@%d\n", index);
        fprintf(fp, "D=A\n");
        fprintf(fp, "@LCL\n");
        fprintf(fp, "D=M+D\n");
        fprintf(fp, "@R13\n");
        fprintf(fp, "M=D\n");
        fprintf(fp, "@SP\n");
        fprintf(fp, "AM=M-1\n");
        fprintf(fp, "D=M\n");
        fprintf(fp, "@R13\n");
        fprintf(fp, "A=M\n");
        fprintf(fp, "M=D\n");
    } else if (strncmp(segment, "this", 4) == 0) {
        fprintf(fp, "@%d\n", index);
        fprintf(fp, "D=A\n");
        fprintf(fp, "@THIS\n");
        fprintf(fp, "D=M+D\n");
        fprintf(fp, "@R13\n");
        fprintf(fp, "M=D\n");
        fprintf(fp, "@SP\n");
        fprintf(fp, "AM=M-1\n");
        fprintf(fp, "D=M\n");
        fprintf(fp, "@R13\n");
        fprintf(fp, "A=M\n");
        fprintf(fp, "M=D\n");
    } else if (strncmp(segment, "that", 4) == 0) {
        fprintf(fp, "@%d\n", index);
        fprintf(fp, "D=A\n");
        fprintf(fp, "@THAT\n");
        fprintf(fp, "D=M+D\n");
        fprintf(fp, "@R13\n");
        fprintf(fp, "M=D\n");
        fprintf(fp, "@SP\n");
        fprintf(fp, "AM=M-1\n");
        fprintf(fp, "D=M\n");
        fprintf(fp, "@R13\n");
        fprintf(fp, "A=M\n");
        fprintf(fp, "M=D\n");
    } else if (strncmp(segment, "pointer", 7) == 0) {
        if (index != 0 && index != 1) {
            fprintf(stderr, "invalid index.\n The pointer segment should indicate 0 or 1.\n");
            exit(1);
        }
        fprintf(fp, "@%d\n", index);
        fprintf(fp, "D=A\n");
        fprintf(fp, "@3\n");
        fprintf(fp, "D=A+D\n");
        fprintf(fp, "@R13\n");
        fprintf(fp, "M=D\n");
        fprintf(fp, "@SP\n");
        fprintf(fp, "AM=M-1\n");
        fprintf(fp, "D=M\n");
        fprintf(fp, "@R13\n");
        fprintf(fp, "A=M\n");
        fprintf(fp, "M=D\n");
    } else if (strncmp(segment, "static", 6) == 0) {
        fprintf(fp, "@SP\n");
        fprintf(fp, "AM=M-1\n");
        fprintf(fp, "D=M\n");
        fprintf(fp, "@%s.%d\n", file_name, index);
        fprintf(fp, "M=D\n");
    } else if (strncmp(segment, "temp", 4) == 0) {
        fprintf(fp, "@%d\n", index);
        fprintf(fp, "D=A\n");
        fprintf(fp, "@5\n");
        fprintf(fp, "D=A+D\n");
        fprintf(fp, "@R13\n");
        fprintf(fp, "M=D\n");
        fprintf(fp, "@SP\n");
        fprintf(fp, "AM=M-1\n");
        fprintf(fp, "D=M\n");
        fprintf(fp, "@R13\n");
        fprintf(fp, "A=M\n");
        fprintf(fp, "M=D\n");
    }
}


/**
 * \brief Write assembly code corresponding to "add".
 * \note This function should be called if the current command is C_ARITHMETIC.
 * \param[in] fp: A file to be written code
 */
void write_add(FILE *fp) {
    fprintf(fp, "// add\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "M=D+M\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=M+1\n");
}


/**
 * \brief Write assembly code corresponding to "sub".
 * \note This function should be called if the current command is C_ARITHMETIC.
 * \param[in] fp: A file to be written code
 */
void write_sub(FILE *fp) {
    fprintf(fp, "// sub\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "M=M-D\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=M+1\n");
}


/**
 * \brief Write assembly code corresponding to "neg".
 * \note This function should be called if the current command is C_ARITHMETIC.
 * \param[in] fp: A file to be written code
 */
void write_neg(FILE *fp) {
    fprintf(fp, "// neg\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "M=-M\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=M+1\n");
}


/**
 * \brief Write assembly code corresponding to "eq".
 * \note This function should be called if the curretn command is C_ARITHMETIC.
 * \param[in] fp: A file to be written code
 */
void write_eq(FILE *fp, int label_count) {
    fprintf(fp, "// eq\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "D=M-D\n");
    fprintf(fp, "@LABEL%d\n", label_count);
    fprintf(fp, "D;JEQ\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "M=0\n");
    fprintf(fp, "@ENDLABEL%d\n", label_count);
    fprintf(fp, "0;JMP\n");
    fprintf(fp, "(LABEL%d)\n", label_count);
    fprintf(fp, "@SP\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "M=-1\n");
    fprintf(fp, "(ENDLABEL%d)\n", label_count);
    fprintf(fp, "@SP\n"); 
    fprintf(fp, "M=M+1\n"); 
}


/**
 * \brief Write assembly code corresponding to "gt".
 * \note This function should be called if the curretn command is C_ARITHMETIC.
 * \param[in] fp: A file to be written code
 */
void write_gt(FILE *fp, int label_count) {
    fprintf(fp, "// gt\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "D=M-D\n");
    fprintf(fp, "@LABEL%d\n", label_count);
    fprintf(fp, "D;JGT\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "M=0\n");
    fprintf(fp, "@ENDLABEL%d\n", label_count);
    fprintf(fp, "0;JMP\n");
    fprintf(fp, "(LABEL%d)\n", label_count);
    fprintf(fp, "@SP\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "M=-1\n");
    fprintf(fp, "(ENDLABEL%d)\n", label_count);
    fprintf(fp, "@SP\n"); 
    fprintf(fp, "M=M+1\n"); 
}


/**
 * \brief Write assembly code corresponding to "lt".
 * \note This function should be called if the curretn command is C_ARITHMETIC.
 * \param[in] fp: A file to be written code
 */
void write_lt(FILE *fp, int label_count) {
    fprintf(fp, "// lt\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "D=M-D\n");
    fprintf(fp, "@LABEL%d\n", label_count);
    fprintf(fp, "D;JLT\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "M=0\n");
    fprintf(fp, "@ENDLABEL%d\n", label_count);
    fprintf(fp, "0;JMP\n");
    fprintf(fp, "(LABEL%d)\n", label_count);
    fprintf(fp, "@SP\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "M=-1\n");
    fprintf(fp, "(ENDLABEL%d)\n", label_count);
    fprintf(fp, "@SP\n"); 
    fprintf(fp, "M=M+1\n"); 
}


/**
 * \brief Write assembly code corresponding to "and".
 * \note This function should be called if the curretn command is C_ARITHMETIC.
 * \param[in] fp: A file to be written code
 */
void write_and(FILE *fp) {
    fprintf(fp, "// and\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "M=D&M\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=M+1\n");
}


/**
 * \brief Write assembly code corresponding to "or".
 * \note This function should be called if the curretn command is C_ARITHMETIC.
 * \param[in] fp: A file to be written code
 */
void write_or(FILE *fp) {
    fprintf(fp, "// or\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "M=D|M\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=M+1\n");
}


/**
 * \brief Write assembly code corresponding to "not".
 * \note This function should be called if the curretn command is C_ARITHMETIC.
 * \param[in] fp: A file to be written code
 */
void write_not(FILE *fp) {
    fprintf(fp, "// not\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "M=!M\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=M+1\n");
}