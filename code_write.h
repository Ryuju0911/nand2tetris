#ifndef CODE_WRITE_H

#define CODE_WRITE_H

#include <stdio.h>

void vm_translator(FILE *fp, char *line);
void write_init(FILE *fp);

extern int label_count;

#endif