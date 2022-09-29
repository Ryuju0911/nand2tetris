#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code_write.h"
#include "utils.h"
#include "parser.h"


/* prototype declaration */
void write_push(FILE *fp, char *segment, int num);
void write_pop(FILE *fp, char *segment, int num);
void write_label(FILE *fp, char *label_name);
void write_goto(FILE *fp, char *label_name);
void write_if(FILE *fp, char *label_name);
void write_call(FILE *fp, char *function_name, int num_args);
void write_return(FILE *fp);
void write_function(FILE *fp, char *function_name, int num_locals);
void write_add(FILE *fp);
void write_sub(FILE *fp);
void write_neg(FILE *fp);
void write_eq(FILE *fp, int label_count);
void write_gt(FILE *fp, int label_count);
void write_lt(FILE *fp, int label_count);
void write_and(FILE *fp);
void write_or(FILE *fp);
void write_not(FILE *fp);
void save_state(FILE *fp, char *register_name);


/* global variables */
char file_name[100];
int label_count = 0;
int return_count = 0;


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
    } else if (cur_command_type == C_LABEL) {
        write_label(fp, arg1);
    } else if (cur_command_type == C_GOTO) {
        write_goto(fp, arg1);
    } else if (cur_command_type == C_IF) {
        write_if(fp, arg1);
    } else if (cur_command_type == C_FUNCTION) {
        write_function(fp, arg1, arg2);
    } else if (cur_command_type == C_RETURN) {
        write_return(fp);
    } else if (cur_command_type == C_CALL) {
        write_call(fp, arg1, arg2);
    }

    free(arg1);
}


/**
 * \brief Write assembly code corresponding to "if-goto" command in vm file.
 * \param[in] fp: A file to be written code 
 */
void write_init(FILE *fp) {
    fprintf(fp, "@256\n");
    fprintf(fp, "D=A\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=D\n");

    write_call(fp, "Sys.init", 0);
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
 * \brief Write assembly code corresponding to "label" command in vm file.
 * \param[in] fp: A file to be written code 
 * \param[in] label_name: The label name
 */
void write_label(FILE *fp, char *label_name) {
    fprintf(fp, "(%s)\n", label_name);
}


/**
 * \brief Write assembly code corresponding to "goto" command in vm file.
 * \param[in] fp: A file to be written code 
 * \param[in] label_name: The label name
 */
void write_goto(FILE *fp, char *label_name) {
    fprintf(fp, "// goto %s\n", label_name);
    fprintf(fp, "@%s\n", label_name);
    fprintf(fp, "0;JMP\n");
}


/**
 * \brief Write assembly code corresponding to "if-goto" command in vm file.
 * \param[in] fp: A file to be written code 
 * \param[in] label_name: The label name
 */
void write_if(FILE *fp, char *label_name) {
    fprintf(fp, "// if-goto %s\n", label_name);
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=M-1\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@%s\n", label_name);
    fprintf(fp, "D;JNE\n");
}


/**
 * \brief Write assembly code corresponding to "call" command in vm file.
 * \param[in] fp: A file to be written code 
 * \param[in] function_name: The function name to be called
 * \param[in] num_args: The number of argments passed to the function.
 */
void write_call(FILE *fp, char *function_name, int num_args) {
    fprintf(fp, "// call %s %d\n", function_name, num_args);

    /* push return address */
    fprintf(fp, "@return%d\n", return_count);
    fprintf(fp, "D=A\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "M=D\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=M+1\n");

    /* save the state of the calling function by pushing LCL, ARG, THIS and THAT */
    save_state(fp, "LCL");
    save_state(fp, "ARG");
    save_state(fp, "THIS");
    save_state(fp, "THAT");

    /* */
    fprintf(fp, "@SP\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@%d\n", num_args);
    fprintf(fp, "D=D-A\n");
    fprintf(fp, "@5\n");
    fprintf(fp, "D=D-A\n");
    fprintf(fp, "@ARG\n");
    fprintf(fp, "M=D\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@LCL\n");
    fprintf(fp, "M=D\n");
    fprintf(fp, "@%s\n", function_name);
    fprintf(fp, "0;JMP\n");
    fprintf(fp, "(return%d)\n", return_count);

    return_count++;
}


/**
 * \brief Write assembly code corresponding to "return" command in vm file.
 * \param[in] fp: A file to be written code 
 */
void write_return(FILE *fp) {
    fprintf(fp, "// return\n");

    /* memory[13] <- memory[LCL] */
    fprintf(fp, "@LCL\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@R13\n");
    fprintf(fp, "M=D\n");

    /* memory[14] <- memory[memory[R13]-5] */
    fprintf(fp, "@5\n");
    fprintf(fp, "D=A\n");
    fprintf(fp, "@R13\n");
    fprintf(fp, "A=M-D\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@R14\n");
    fprintf(fp, "M=D\n");

    /* memory[SP] -= 1 */
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=M-1\n");

    /* memory[memory[ARG]] <- memory[memory[SP]] */
    fprintf(fp, "@SP\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@ARG\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "M=D\n");

    /* memory[SP] <- memory[ARG] + 1 */
    fprintf(fp, "@ARG\n");
    fprintf(fp, "D=M+1\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=D\n");

    /* memory[THAT] <- memory[memory[R13]-1] */
    fprintf(fp, "@1\n");
    fprintf(fp, "D=A\n");
    fprintf(fp, "@R13\n");
    fprintf(fp, "A=M-D\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@THAT\n");
    fprintf(fp, "M=D\n");

    /* memory[THIS] <- memory[memory[R13]-2] */
    fprintf(fp, "@2\n");
    fprintf(fp, "D=A\n");
    fprintf(fp, "@R13\n");
    fprintf(fp, "A=M-D\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@THIS\n");
    fprintf(fp, "M=D\n");

    /* memory[ARG] <- memory[memory[R13]-3] */
    fprintf(fp, "@3\n");
    fprintf(fp, "D=A\n");
    fprintf(fp, "@R13\n");
    fprintf(fp, "A=M-D\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@ARG\n");
    fprintf(fp, "M=D\n");

    /* memory[LCL] <- memory[memory[R13]-4] */
    fprintf(fp, "@4\n");
    fprintf(fp, "D=A\n");
    fprintf(fp, "@R13\n");
    fprintf(fp, "A=M-D\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@LCL\n");
    fprintf(fp, "M=D\n");

    /* goto memory[R14] */
    fprintf(fp, "@R14\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "0;JMP\n");
}


/**
 * \brief Write assembly code corresponding to "function" command in vm file.
 * \param[in] fp: A file to be written code 
 * \param[in] function_name: The function name to be declared.
 * \param[in] num_locals: The number of local variables the declared function has.
 */
void write_function(FILE *fp, char *function_name, int num_locals) {
    fprintf(fp, "(%s)\n", function_name);

    for (int i=0; i<num_locals; i++) {
        write_push(fp, "constant", 0);
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


/**
 * \brief Write assembly code corresponding to "not".
 * \note This function should be called if the curretn command is C_ARITHMETIC.
 * \param[in] fp: A file to be written code
 */
void save_state(FILE *fp, char *register_name) {
    fprintf(fp, "@%s\n", register_name);
    fprintf(fp, "D=M\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "M=D\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=M+1\n");
}