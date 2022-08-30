#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create_dest_map(HashMap *map) {
    set(map, "", "000");
    set(map, "M", "001");
    set(map, "D", "010");
    set(map, "MD", "011");
    set(map, "A", "100");
    set(map, "AM", "101");
    set(map, "AD", "110");
    set(map, "AMD", "111");
}

void create_comp_map(HashMap *map) {
    set(map, "0", "0101010");
    set(map, "1", "0111111");
    set(map, "-1", "0111010");
    set(map, "D", "0001100");
    set(map, "A", "0110000");
    set(map, "!D", "0001101");
    set(map, "!A", "0110001");
    set(map, "-D", "0001111");
    set(map, "-A", "0110011");
    set(map, "D+1", "0011111");
    set(map, "A+1", "0110111");
    set(map, "D-1", "0001110");
    set(map, "A+1", "0110111");
    set(map, "D-1", "0001110");
    set(map, "A-1", "0110010");
    set(map, "D+A", "0000010");
    set(map, "D-A", "0010011");
    set(map, "A-D", "0000111");
    set(map, "D&A", "0000000");
    set(map, "D|A", "0010101");
    set(map, "M", "1110000");
    set(map, "!M", "1110001");
    set(map, "-M", "1110011");
    set(map, "M+1", "1110111");
    set(map, "M-1", "1110010");
    set(map, "D+M", "1000010");
    set(map, "D-M", "1010011");
    set(map, "M-D", "1000111");
    set(map, "D&M", "1000000");
    set(map, "D|M", "1010101");
}

void create_jump_map(HashMap *map) {
    set(map, "", "000");
    set(map, "JGT", "001");
    set(map, "JEQ", "010");
    set(map, "JGE", "011");
    set(map, "JLT", "100");
    set(map, "JNE", "101");
    set(map, "JLE", "110");
    set(map, "JMP", "111");
}

void create_symbol_map(HashMap *map) {
    set(map, "SP", "0");
    set(map, "LCL", "1");
    set(map, "ARG", "2");
    set(map, "THIS", "3");
    set(map, "THAT", "4");
    set(map, "R0", "0");
    set(map, "R1", "1");
    set(map, "R2", "2");
    set(map, "R3", "3");
    set(map, "R4", "4");
    set(map, "R5", "5");
    set(map, "R6", "6");
    set(map, "R7", "7");
    set(map, "R8", "8");
    set(map, "R9", "9");
    set(map, "R10", "10");
    set(map, "R11", "11");
    set(map, "R12", "12");
    set(map, "R13", "13");
    set(map, "R14", "14");
    set(map, "R15", "15");
    set(map, "SCREEN", "16384");
    set(map, "KBD", "24576");
}
