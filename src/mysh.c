#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "io_helpers.h"
#include "builtins.h"
#include "variables.h"
#include <signal.h>

Node *front = NULL;

int get_key(char *strng) {

	int count = 0;
	int i;

	for (i = 0; strng[i] != '='; i++) {

		count ++;

	}

	return count;

}


void handler(int signal) {

	display_message("\n");

}



int execute(char **token_arr) {

	int result = fork();

	int status;

	if (result == 0) {
		execvp(token_arr[0], token_arr);
		display_error("ERROR: Unrecognized command: ", token_arr[0]);
		//display_message("mysh$ ");
		exit(-1);

	}

	wait(&status);

	while (! WIFEXITED(status)) {continue;}

	return(1);
}


void var_redef(char **token_arr) {


	for (int i = 0; token_arr[i] != NULL; i++) {


            if ((strchr(token_arr[i], '$') != NULL) && (strstr(token_arr[i], "=$") == NULL)) {

		char *pt = token_arr[i] + 1;

		if (find_node(front, pt) == 1) {

			token_arr[i] = node_key(front, strchr(token_arr[i], '$') + 1);

		}
            }
        }


	return;
}


void process_builtin(char *tokens_arr[]) {


	bn_ptr builtin_fn = check_builtin(tokens_arr[0]);

        if (builtin_fn != NULL) {
            ssize_t err = builtin_fn(tokens_arr);

            if (err == - 1) {
                display_error("ERROR: Builtin failed: ", tokens_arr[0]);
            }
        }

	else {

	    execute(tokens_arr);
            //display_error("ERROR: Unrecognized command: ", tokens_arr[0]);
        }

}


void process_pipes(size_t pipe_count, char *tokens[]) {

	int pipes[pipe_count - 1][2];

	int i;

	for (i = 0; i < pipe_count - 1; i++) {


		if (pipe(pipes[i]) == -1) {

			perror("pipe");
			exit(1);
		}

		pid_t result = fork();

		if (result < 0) {

			perror("fork");
			exit(1);
		}

		else if (result == 0 && i == 0) { // We are in the child

			char *token_arr_2[MAX_STR_LEN] = {NULL};

			tokenize_input(tokens[i], token_arr_2);
			var_redef(token_arr_2);
			dup2(pipes[i][1], STDOUT_FILENO);

			if (close(pipes[i][1]) == -1) {

				perror("close");
				exit(1);

			}

			if (close(pipes[i][0]) == -1) {

				perror("close");
				exit(1);
			}

			if (strchr(token_arr_2[0], '=') == NULL) {
				process_builtin(token_arr_2);
			}
			exit(0);

		}

		else if (result == 0 && i != 0) {

			char *token_arr_2[MAX_STR_LEN] = {NULL};

			tokenize_input(tokens[i], token_arr_2);

			var_redef(token_arr_2);

			if (dup2(pipes[i-1][0], STDIN_FILENO) == -1) {

				fprintf(stderr, "right here on the %d iteration\n", i);
				perror("dup2");
				exit(1);
			}

			for (int u = 0; u < i; u++) {

				if (close(pipes[u][0]) == -1) {

					perror("close");
					exit(1);
				}

				if (close(pipes[u][1]) == -1) {

					perror("close");
					exit(1);
				}


			}

			if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {

				perror("dup2");
				exit(1);
			}

			if (close(pipes[i][1]) == -1) {

				perror("close");
				exit(1);

			}

			if (close(pipes[i][0]) == -1) {

				perror("close");
				exit(1);
			}

			if (strchr(token_arr_2[0], '=') == NULL) {
				process_builtin(token_arr_2);
			}

			exit(0);

		}

		else { // Parent process gets here only


			if ((i-1) > 0) {

				if (close(pipes[i-1][0]) == -1) {

					perror("close");
					exit(1);
				}

				if (close(pipes[i-1][1]) == -1) {
					perror("exit");
					exit(1);
				}

			}

		}

	}

	int wstatus;
	if (wait(&wstatus) == -1) {

		perror("wait");
		exit(1);
	}

	while (! WIFEXITED(wstatus)) {

		continue;
	}

	char *token_arr_2[MAX_STR_LEN] = {NULL};

	tokenize_input(tokens[i], token_arr_2);

	var_redef(token_arr_2);

	int std_in = dup(0);
	if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {

		perror("dup2");
		exit(1);
	}

	if (close(pipes[i - 1][0]) == -1) {

		printf("parent i is %d\n", i);
		perror("close");
		exit(1);
	}

	if (close(pipes[i - 1][1]) == -1) {

		perror("close");
		exit(1);
	}

	if (strstr("cd", token_arr_2[0]) == NULL) {
		process_builtin(token_arr_2);
	}

	dup2(std_in, STDIN_FILENO);
	return;


}

void process_pipe(size_t pipe_count, char *tokens[]) {


	int pipes[pipe_count -1][2];

	for (int i = 0; i < pipe_count - 1; i++) {


		if (pipe(pipes[i]) == -1) {

			perror("pipe");
			exit(1);
		}

		pid_t result = fork();

		if (result < 0) {

			perror("fork");
			exit(1);
		}

		if (result == 0) { // We are in the child

			char *token_arr_2[MAX_STR_LEN] = {NULL};

			tokenize_input(tokens[i], token_arr_2);

			var_redef(token_arr_2);

			dup2(pipes[i][1], STDOUT_FILENO);
			if (close(pipes[i][1]) == -1) {

				perror("close");
				exit(1);

			}

			if (close(pipes[i][0]) == -1) {

				perror("close");
				exit(1);
			}

			if (strchr(token_arr_2[0], '=') == NULL) {
				process_builtin(token_arr_2);
			}
			exit(0);
		}

		else {

			if (close(pipes[i][1]) == -1) {

				perror("close");
				exit(1);
			}

		}

	}

	int wstatus;
	if (wait(&wstatus) == -1) {

		perror("wait");
		exit(1);
	}

	while (! WIFEXITED(wstatus)) {

		continue;
	}

	char *token_arr_2[MAX_STR_LEN] = {NULL};

	tokenize_input(tokens[pipe_count-1], token_arr_2);

	var_redef(token_arr_2);
	int std_in = dup(0);
	dup2(pipes[0][0], STDIN_FILENO);

	if (close(pipes[0][0]) == -1) {

		perror("close");
		exit(1);
	}

	if (strstr("cd", token_arr_2[0]) == NULL) {
		process_builtin(token_arr_2);
	}

	dup2(std_in, STDIN_FILENO);
	return;

}


int main(int argc, char* argv[]) {
    char *prompt = "mysh$ ";

    char input_buf[MAX_STR_LEN + 1];
    input_buf[MAX_STR_LEN] = '\0';
    char *token_arr[MAX_STR_LEN] = {NULL};

    struct sigaction newact;
    newact.sa_handler = handler;
    newact.sa_flags = 0;
    sigemptyset(&newact.sa_mask);
    sigaction(SIGINT, &newact, NULL);

    while (1) {

	display_message(prompt);
        int ret = get_input(input_buf);
        size_t token_count = 0;
	size_t pipe_count = 0;


	if (strstr(input_buf, "|") != NULL) {

		pipe_count = tokenize_pipe(input_buf, token_arr);
		//token_count = pipe_count;
	}

	else {

		token_count = tokenize_input(input_buf, token_arr);
	}
	for (int i = 0; input_buf[0] != '\0' && token_arr[0] != NULL && token_arr[i] != NULL; i++) {


            if ((strchr(token_arr[i], '$') != NULL) && (strstr(token_arr[i], "=$") == NULL)) {

		char *pt = token_arr[i] + 1;

		if (find_node(front, pt) == 1) {

			token_arr[i] = node_key(front, strchr(token_arr[i], '$') + 1);

		}
            }
        }


	if (token_count == 1 && (strchr(token_arr[0], '=') != NULL)) {

            char key[MAX_STR_LEN];
            char val[MAX_STR_LEN];
	    key[0] = '\0';
	    val[0] = '\0';

	    if (strcmp(token_arr[0], "=") == 0) {

		continue;
	    }

	    else {

		int count = get_key(token_arr[0]);
		strncat(key, token_arr[0], count);
		key[count + 1] = '\0';
		key[63] = '\0';
		char *temp_val;

		if ((strstr(token_arr[0], "=$")) != NULL) {

		    temp_val =  strchr(token_arr[0], '$') + 1;
		}

		else {

		    temp_val =  strchr(token_arr[0], '=') + 1;
		}

		strncat(val, temp_val, strlen(temp_val));

		val[strlen(temp_val) + 1] = '\0';
		val[63] = '\0';


		front = create_node(key, val, front);


	   }

        }


	else if (ret == -1 && input_buf[0] != '\0') {

		free_memory(front);
                break;

        }


        else if (ret != -1 && input_buf[0] != '\0' && token_count == 0 && pipe_count == 0) {

                continue;
        }


        else if (ret != -1 && ((token_count == 0 && pipe_count == 0)|| (strncmp("exit", token_arr[0], 4) == 0))) {// changed to &&

	     free_memory(front);
             break;
        }

	else if (pipe_count >= 1) {

		if (pipe_count == 2) {
			process_pipe(pipe_count, token_arr);
		}
		else {
			process_pipes(pipe_count, token_arr);
		}
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

		execute(token_arr);

            }
        }

    }

    return 0;
}

