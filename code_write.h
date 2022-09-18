#ifndef CODE_WRITE_H

#define CODE_WRITE_H

#include <stdio.h>

void vm_translator(FILE *fp, char *line);

extern int label_count;

#endif