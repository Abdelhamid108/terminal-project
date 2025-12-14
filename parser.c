/**
 * @file parser.c
 * @brief Input parsing module for the shell.
 *
 * This file implements the functionality to tokenize user input strings
 * into executable commands and arguments. It handles memory allocation
 * and resizing for the argument list.
 *
 * @author Abdelhamid
 * @date 2025-12-14
 */

#include "shell.h"

/**
 * @brief Parses a raw input line into an array of tokens (arguments).
 *
 * This function uses `strtok` to split the input string based on standard
 * delimiters (space, tab, newline). It dynamically allocates memory for
 * the array of pointers and resizes it if the number of arguments exceeds
 * the initial buffer size.
 *
 * @param line The input string read from the user.
 * @return char** A null-terminated array of strings, where the first element
 *                is the command and subsequent elements are arguments.
 *                Returns NULL if allocation fails.
 */
char **parse_input(char *line) {
  int bufsize = MAX_ARGS;
  int position = 0;
  char **tokens = malloc(bufsize * sizeof(char *));
  char *token;

  if (!tokens) {
    fprintf(stderr, "shell: allocation error\n");
    exit(EXIT_FAILURE);
  }

  // Initialize strtok with the line and delimiters
  token = strtok(line, DELIMITERS);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    // Resize buffer if we exceeded the limit
    if (position >= bufsize) {
      bufsize += MAX_ARGS;
      tokens = realloc(tokens, bufsize * sizeof(char *));
      if (!tokens) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, DELIMITERS);
  }

  // Null-terminate the list of arguments
  tokens[position] = NULL;
  return tokens;
}
