#ifndef HASHMAP_H
#define HASHMAP_H

typedef struct pair {
    char key[100];
    char val[100];
    struct pair *next;
} Pair;

typedef struct hashmap {
    Pair **list;       /* the list of Pair* */
    unsigned int cap;  /* capacity, the length of list */
    unsigned int len;  /* length, the number of pairs */
} HashMap;

HashMap *newHashMap();
char *get(HashMap *map, char *key);
void set(HashMap *map, char *key, char *val);

#endif