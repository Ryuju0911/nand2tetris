#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

unsigned int hashcode(HashMap *map, char *key);

/**
 * \brief Make a new hashmap instance, and return the pointer to it.
 * \note Do not forget to free a map.
 * \param[out] HashMap*: The pointer to a new hashmap
 */
HashMap *newHashMap(){
    HashMap *map = malloc(sizeof(map));
    map->cap = 20;
    map->len = 0;
    map->list = calloc((map->cap), sizeof(Pair*));
    return map;
}

/**
 * \brief Calculate hashcode of key, and return it. 
 * \param[in] map: Hashmap
 * \param[in] key: Key to be hashed
 * \return Hashcode of key 
 */
unsigned int hashcode(HashMap *map, char *key) {
    unsigned int code;
    for (code = 0; *key != '\0'; key++) {
        code = *key + 31 * code;
    }
    return code % (map->cap);
}


/**
 * \brief Get the value corresponding to a key from hashmap.
 * \param[in] map: Hashmap 
 * \param[in] key: Target key
 * \return Value if the key is in hashmap, NULL otherwise. 
 */
char *get(HashMap *map, char *key) {
    Pair *current;
    for (current = map->list[hashcode(map, key)]; current; current = current->next) {
        if (!strcmp(current->key, key)) {
            return current->val;
        }
    }
    return NULL;
}

/**
 * \brief Set {key: value} pair in hashmap.
 * \note value should be string.
 */
void set(HashMap *map, char *key, char *val) {
    unsigned int index = hashcode(map, key);
    Pair *current;
    for (current = map->list[index]; current; current = current->next) {
        /* key has been already in hashmap */
        if (!strcmp(current->key, key)) {
            strcpy(current->val, val);
            return;
        }
    }
    /* key is not in the hashmap */
    Pair *p = malloc(sizeof(*p));
    strcpy(p->key, key);
    strcpy(p->val, val);
    p->next = map->list[index];
    map->list[index] = p;
    map->len++;
}
