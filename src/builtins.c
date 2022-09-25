#include "builtins.h"
#include "io_helpers.h"

// ====== Command execution =====

/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd) {
    ssize_t cmd_num = 0;
    while (cmd_num < BUILTINS_COUNT &&
           strncmp(BUILTINS[cmd_num], cmd, MAX_STR_LEN) != 0) {
        cmd_num += 1;
    }
    return BUILTINS_FN[cmd_num];
}


// ===== Builtins =====

/* Prereq: tokens is a NULL terminated sequence of strings.
 * Return 0 on success and -1 on error ... but there are no errors on echo. 
 */
ssize_t bn_echo(char **tokens) {

    ssize_t index = 1;

    if (tokens[index] == NULL) {

        display_message("\n");
        return 0;
    }

    while (tokens[index] != NULL) {

        if (tokens[index + 1] == NULL) {

            display_message(tokens[index]);
            index += 1;
        }

        else {

            display_message(tokens[index]);
            display_message(" ");
            index += 1;
        }
    }

    display_message("\n");
    return 0;
}


ssize_t bn_cat(char **tokens) {


	ssize_t index = 1;

	if (tokens[index] == NULL) {

		FILE *fptr = NULL;
		struct pollfd fds;
		fds.fd = 0;
		fds.events = POLLIN;
		int ret = poll(&fds, 1, 10); // in your project, set this to 10, not 3000.
		if (ret == 0) {
			write(STDERR_FILENO, "ERROR: No input source provided\n", strlen("ERROR: No input source provided\n"));
			return(-1);
		} else {

			fptr = stdin;
			char file_name[MAX_STR_LEN];

			fgets(file_name, MAX_STR_LEN, fptr);
			strip_space(file_name);
			char *token_arr[3] = {"cat", file_name, NULL};

			FILE *file = fopen(token_arr[index], "r");

    			if (file == NULL) {

				display_message(token_arr[index]);
				display_message("\n");
        			return(0);
    			}

			return bn_cat(token_arr);

		}

	}

	FILE *file = fopen(tokens[index], "r");

    	if (file == NULL) {

		write(STDERR_FILENO, "ERROR: Cannot open file\n", strlen("ERROR: Cannot open file\n"));
        	return(-1);
    	}

    	int c;
   	while ((c = fgetc(file)) != EOF) {

        	char string_c[2] = {(char)(c), '\0'};
        	display_message(string_c);

    	}

	return(0);

}


ssize_t bn_wc(char **tokens) {

	ssize_t index = 1;

	if (tokens[index] == NULL) {

		FILE *fptr = NULL;
		struct pollfd fds;
		fds.fd = 0;
		fds.events = POLLIN;
		int ret = poll(&fds, 1, 3000); // in your project, set this to 10, not 3000.
		if (ret == 0) {
			write(STDERR_FILENO,  "ERROR: No input source provided\n", strlen("ERROR: No input source provided\n"));
			return(-1);
		}
		else {
			fptr = stdin;

			int c;

			int word_count = 0;

			int new_line_count = 0;

			int char_count = 0;


			while ((c = fgetc(fptr)) != EOF) {

				char_count += 1;

				if (((char)(c)) == '\n') {

					new_line_count += 1;
				}

			// check if c is char then next char is white space then put next char back into reading stream
				else if (((char)(c)) != '\t' && ((char)(c)) != ' ') {

						int next = fgetc(fptr);

						if (((char)(next)) == ' ' || ((char)(next)) == '\n' || ((char)(next)) == '\t' || ((char)(next)) == EOF) { 

							word_count += 1;

						}

						ungetc(next, fptr);
				}


    			}

			char wc[20];
			char cc[20];
			char nlc[20];

			sprintf(wc, "%ld", (long)(word_count));
			sprintf(cc, "%ld", (long)(char_count));
			sprintf(nlc, "%ld", (long)(new_line_count));

			display_message("word count ");
			display_message(wc);
			display_message("\n");

			display_message("character count ");
			display_message(cc);
			display_message("\n");

			display_message("newline count ");
			display_message(nlc);
			display_message("\n");

			return(0);
		}
	}


	FILE *file = fopen(tokens[index], "r");

    	if (file == NULL) {

		write(STDERR_FILENO,  "ERROR: Cannot open file\n", strlen("ERROR: Cannot open file\n"));
               	return(-1);

    	}


	int c;

        int word_count = 0;

        int new_line_count = 0;

        int char_count = 0;


	while ((c = fgetc(file)) != EOF) {

        	char_count += 1;

        	if (((char)(c)) == '\n') {

            	new_line_count += 1;
               }

        // check if c is char then next char is white space then put next char back into reading stream
       	       else if (((char)(c)) != '\t' && ((char)(c)) != ' ') {

                    int next = fgetc(file);

                    if (((char)(next)) == ' ' || ((char)(next)) == '\n' || ((char)(next)) == '\t' || ((char)(next)) == EOF) { 

                          word_count += 1;

                    }

                    ungetc(next, file);
               }


        }

	fclose(file);

	char wc[20];
	char cc[20];
	char nlc[20];

	sprintf(wc, "%ld", (long)(word_count));
	sprintf(cc, "%ld", (long)(char_count));
	sprintf(nlc, "%ld", (long)(new_line_count));

	display_message("word count ");
	display_message(wc);
	display_message("\n");

	display_message("character count ");
	display_message(cc);
	display_message("\n");

	display_message("newline count ");
	display_message(nlc);
	display_message("\n");


   	return(0);

}


ssize_t bn_cd(char **tokens) {

	ssize_t index = 1;

	if (tokens[index] == NULL) {

		write(STDERR_FILENO, "ERROR: No input source provided\n", strlen("ERROR: No input source provided\n"));
		return(-1);
	}

	if (check_path(tokens[index]) && strlen(tokens[index]) >= 2) {


       		 int error = chdir("..");

       		 if (error != 0) {

            		write(STDERR_FILENO, "ERROR: Invalid path\n", strlen("ERROR: Invalid path\n"));
           		return(-1);

        	 }

        	char *new_path = tokens[index] + 2;

       		for (int i = 0; new_path[i] != '\0'; i++) {

            		int error2 = chdir("..");

           		if (error2 != 0) {

                		write(STDERR_FILENO, "ERROR: Invalid path\n", strlen("ERROR: Invalid path\n"));
               			return(-1);
           		}
       		}
   	}

	else {

        	int error3;

       		error3 = chdir(tokens[index]);

       		if (error3 != 0) {

			write(STDERR_FILENO, "ERROR: Invalid path\n", strlen("ERROR: Invalid path\n"));
            		return(-1);

        	}
   	}



	return(0);
}



int check_path(char *path) {

    for (int i = 0; path[i] != '\0'; i++) {

        if (path[i] != '.') {

            return 0;
        }
    }


    return(1);
}


ssize_t bn_ls(char **tokens) {

	ssize_t index = 1;

	if (tokens[index] == NULL) {

		return no_path_bn_ls(".", "", 0);

	}

	char *path = "";
	int depth = -1;
	char *substring = "";


	for (int i = 1; tokens[i] != NULL; i++) {


		if (strcmp(tokens[i], "--f") == 0) {

			substring = tokens[i+1];

			if (substring == NULL) {


				write(STDERR_FILENO, "ERROR: usage no substring\n", strlen("ERROR: usage no substring\n"));

				return(-1);

			}
		}
		else if (strcmp(tokens[i], "--rec") == 0) {


			path = tokens[i+1];

			if (path == NULL) {


				fprintf(stderr, "ERROR: Invalid path");
				return(-1);

			}

		}


		else if (strcmp(tokens[i], "--d") == 0) {


			if (tokens[i+1] == NULL) {


				write(STDERR_FILENO, "ERROR: usage no depth given\n", strlen("ERROR: usage no depth given\n"));


				return(-1);


			}

			else {


				depth = strtol(tokens[i+1], NULL, 10);

			}

		}

	}

	if (depth == -1 && strcmp(path, "") == 0 && strcmp(substring, "") == 0) {

		return no_path_bn_ls(tokens[index], "", 0);

	}

	else if ((depth == -1 && strcmp(path, "") != 0) || (strcmp(path, "") == 0 && depth != -1)) {

			write(STDERR_FILENO, "ERROR Usage --rec and --d must be provided together\n",
				 strlen("ERROR Usage --rec and --d must be provided together\n"));
			return(-1);
	}

	else if (strcmp(path, "") == 0 && depth == -1 && strcmp(tokens[index], "--f") == 0) {

		return no_path_bn_ls(".", substring, 0);

	}

	else if (strcmp(path, "") == 0 && depth == -1 && strcmp(tokens[index], "--f") != 0) {

		return no_path_bn_ls(tokens[index], substring, 0);

	}

	else if (strcmp(substring, "") == 0) {

		return no_path_bn_ls(path, "", depth);

	}

	else {

		return no_path_bn_ls(path, substring, depth);

	}

     return(0);

}


ssize_t no_path_bn_ls(char *path, char *sub, int depth) {


	DIR *directory = opendir(path);
	struct dirent *file;

	if (directory == NULL) {


		 write(STDERR_FILENO, "ERROR: Invalid path\n", strlen("ERROR: Invalid path\n"));
       		 return(-1);
    	}

	while ((file = readdir(directory)) != NULL) { // --f condition check for  substring

		if (strstr(file->d_name, sub) != NULL) {
       	 		display_message(file->d_name);
			display_message("\n");
		}

		if (file->d_type == DT_DIR && strcmp(file->d_name, ".")  != 0 && strcmp(file->d_name, "..") != 0 && depth > 1) {

            		int length = strlen(path) + strlen(file->d_name) + 2;

            		char new_path[length];
            		new_path[0] = '\0';

            		strncat(new_path, path, strlen(path));
            		strcat(new_path, "/");
            		strncat(new_path, file->d_name, strlen(file->d_name));
            		no_path_bn_ls(new_path, sub, depth - 1);
       		}
    	}


	closedir(directory);
	return(0);

}

void strip_space(char *in_ptr) {

        int null;

        for (int i = 0; in_ptr[i] != '\0'; i++) {

                null = i;
        }

        int i;
        for (i = null - 1; in_ptr[i] == ' '; i--) {

                continue;
        }

        in_ptr[i+1] = '\0';

        return;

}
