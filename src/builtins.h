#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <poll.h>

/* Type for builtin handling functions
 * Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
typedef ssize_t (*bn_ptr)(char **);
ssize_t bn_echo(char **tokens);

ssize_t bn_cat(char **tokens);

ssize_t bn_wc(char **tokens);

ssize_t bn_cat(char **tokens);

ssize_t bn_cd(char **tokens);

ssize_t bn_ls(char **tokens);

ssize_t no_path_bn_ls(char *path, char *sub, int depth);

int check_path(char *path);

int recursive_traversal(char *path, int depth);

void strip_space(char *in_ptr);



/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd);


/* BUILTINS and BUILTINS_FN are parallel arrays of length BUILTINS_COUNT
 */
static const char * const BUILTINS[] = {"echo", "cat", "wc", "cd", "ls"};
static const bn_ptr BUILTINS_FN[] = {bn_echo, bn_cat, bn_wc, bn_cd, bn_ls, NULL};    // Extra null element for 'non-builtin'
static const size_t BUILTINS_COUNT = sizeof(BUILTINS) / sizeof(char *);

#endif
