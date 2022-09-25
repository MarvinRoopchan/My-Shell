#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "variables.h"
#include "io_helpers.h"


Node *create_node(char *key, char *value, Node *next) {

    Node *new_node = malloc(sizeof(Node));

    new_node->key[0] = '\0';
    new_node->value[0] = '\0';

    strncat(new_node->key, key, MAX_STR_LEN);
    new_node->key[strlen(key) + 1] = '\0';
    new_node->key[63] = '\0';
    strncat(new_node->value, value, MAX_STR_LEN);
    new_node->value[strlen(value) + 1] = '\0';
    new_node->value[63] = '\0';
    new_node->next = next;
    return new_node;
}

int find_node(Node *first, char *key) {

    Node *curr = first;

    while (curr != NULL) {

        if (strcmp(curr->key, key) == 0) {

            return 1;
        }

	else {

	     curr = curr->next;
	}
    }

    return -1;
}


char *node_key(Node *first, char *key) {

    Node *curr = first;

    while (curr != NULL) {

        if (strcmp(curr->key, key) == 0) {

            return curr->value;
        }

	else {

	    curr = curr->next;

	}
    }

    return NULL;

}


void free_memory(Node *front) {

        Node *curr = front;

        while (curr != NULL) {

                Node *previous = curr;

                curr = curr->next;

                free(previous);
        }

        return;
}

void display_all(Node *first) {

	Node *curr = first;

	while(curr != NULL) {

		if (curr->key != NULL && curr->value != NULL) {

			printf("%s ---> %s\n", curr->key, curr->value);

			curr = curr->next;

		}

	}

	return;

}
