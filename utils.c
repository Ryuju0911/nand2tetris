#include <stdio.h>
#include <string.h>
#include "utils.h"


char file_name[100]; /* global variable */


/**
 * \brief Get the file name (yyy) from a vm file path (./../xxx/yyy.vm).
 * \param[in] vm_file_path: A vm file path (./../xxx/yyy.vm)
 */
void *get_file_name(char *vm_file_path) {
    char *pos = strrchr(vm_file_path, '/');
    if (pos != NULL) {
        *pos++;
        strcpy(file_name, pos);
    } else {
        strcpy(file_name, vm_file_path);
    }

    char *extension_pos = strstr(file_name, ".vm");
    *extension_pos = '\0';
}


/**
 * \brief Make the file name (xxx.asm) from a file name (xxx).
 * \param[in] vm_file_name: A file name (xxx)
 */
void *make_asm_file_name(char *asm_file_name, char *file_name) {
    strcat(asm_file_name, file_name);
    strcat(asm_file_name, ".asm");
}
