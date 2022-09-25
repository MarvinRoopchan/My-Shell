#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "io_helpers.h"
#include "builtins.h"
#include "variables.h"


void free_memory(Node *front) {

	Node *curr = front;

	while (curr != NULL) {

		Node *previous = curr;

		curr = curr->next;

		free(previous);
	}

	return;

}

int total_length(char **arr, int count) {

    count = 0;

    for (int i = 0; i < count; i++) {

        count += strlen(arr[i]);
    }

    return count;
}

void convert_key_value(char *strng, char **arr, int count) {

    display_message(front->value);
    display_message("YUP");

    if (strng[0] == '$') {

        for (int i = 0; i < count; i ++) {

            char value[MAX_STR_LEN];
	    strncpy(value, node_key(front, arr[i]), strlen(arr[i]));
	    value[63] = '\0';

	    //display_message(front->value);

            if (value == NULL) {

                arr[i] = "";
            }

            else {

                arr[i] = value;
            }
        }
    }

    else {

        for (int i = 1; i < count; i ++) {

	    char value[MAX_STR_LEN];

	    strncpy(value, node_key(front, arr[i]), strlen(arr[i]));
	    value[63] = '\0';

            if (value == NULL) {

                arr[i] = "";
            }

            else {

                arr[i] = value;
            }
        }
    }

    return;
}

int count_token(char *strng) {

  int count = 0;

  for (int i = 0; strng[i] != '\0'; i++) {

    if (strng[i] == '$') {

      count += 1;
    }
  }

  if (strng[0] != '$') {

    count ++;
  }

  return count;
}

int main(int argc, char* argv[]) {
    char *prompt = "mysh$ ";

    char *temp_str = malloc(sizeof(char) * MAX_STR_LEN);

    char input_buf[MAX_STR_LEN + 1];
    input_buf[MAX_STR_LEN] = '\0';
    char *token_arr[MAX_STR_LEN] = {NULL};

    while (1) {

	display_message(prompt);

        int ret = get_input(input_buf);
        size_t token_count = tokenize_input(input_buf, token_arr);

	for (int i = 0; input_buf[0] != '\0' && token_arr[0] != NULL && token_arr[i] != NULL; i++) {

            if ((strchr(token_arr[i], '$')) != NULL) {

		//display_message(front->value);
                int count = count_token(token_arr[i]);

                char *tokenize[count];

                int u = 0;

		strncpy(temp_str, token_arr[i], MAX_STR_LEN);
		temp_str[strlen(token_arr[i]) + 1] = '\0';
		temp_str[63] = '\0';

                char *trck = strtok(temp_str, "$"); //Modifying global copy stromg to temp tru placing node in header
               while (trck != NULL && u < count) {

                    tokenize[u] = trck;
                    trck = strtok(NULL, "$");
                    u += 1;
                }

		//free(temp_str);

		display_message("after tok ");

                convert_key_value(token_arr[i], tokenize, count);
                char *complete_string = malloc(sizeof(char) * MAX_STR_LEN);

                complete_string[0] = '\0';

               for (int v = 0; v < count; v++) {

                strncat(complete_string, tokenize[v], strlen(tokenize[v]));
               }

		char temp[strlen(complete_string) + 1];
		strncpy(temp, complete_string, strlen(complete_string) + 1);
		free(complete_string);

		display_message(temp);
                token_arr[i] = temp;
            }
        }


	if (token_count == 1 && (strchr(token_arr[0], '=') != NULL)) {

            char *key;
            char *val;

            key = strtok(token_arr[0], "=");
	    val = strtok(NULL, "=");

		/**
             if (strcmp(strchr(token_arr[0],'='),"=") == 0) {

                val = " ";
            }

            else {

                val = strtok(NULL, "=");
            }
		**/

            front = create_node(key, val, front);
	  display_message(node_key(front, key));

        }


	else if (ret == -1 && input_buf[0] != '\0') {

		free_memory(front);
		free(temp_str);
                break;

        }


        else if (ret != -1 && input_buf[0] != '\0' && token_count == 0) {

                continue;
        }

        else if (ret != -1 && (token_count == 0 || (strncmp("exit", token_arr[0], 4) == 0))) {

	     free_memory(front);
	     free(temp_str);
             break;
        }

        // Command execution
        else if (token_count >= 1) {
            bn_ptr builtin_fn = check_builtin(token_arr[0]);
            if (builtin_fn != NULL) {
                ssize_t err = builtin_fn(token_arr);
                if (err == - 1) {
                    display_error("ERROR: Builtin failed: ", token_arr[0]);
                }
            } else {
                display_error("ERROR: Unrecognized command: ", token_arr[0]);
            }
        }

    }

    return 0;
}
