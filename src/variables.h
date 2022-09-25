#ifndef __VARIABLES_H__
#define __VARIABLES_H__


#include <string.h>
#include <stdlib.h>
#include "io_helpers.h"

typedef struct node {
    char key[MAX_STR_LEN];
    char value[MAX_STR_LEN];
    struct node *next; // pointer to next node struct
} Node;


Node *create_node(char *key, char *value, Node *next);

int find_node(Node *first, char *key);

char *node_key(Node *first, char *key);

void free_memory();

void display_all(Node *first);

void update(Node *first, char *key, char *value);

#endif
