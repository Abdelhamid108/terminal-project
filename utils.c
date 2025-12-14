/**
 * @file utils.c
 * @brief Utility helper functions for the shell interface.
 *
 * This file contains functions that handle user interface elements,
 * specifically generating and displaying the command prompt and
 * reading user input from the standard input stream.
 *
 * @author Abdelhamid
 * @date 2025-12-14
 */

#include "shell.h"

/**
 * @brief Displays the shell prompt.
 *
 * The prompt format is: `user@host:cwd$ `
 * It attempts to retrieve the username, hostname, and current working
 * directory. If retrieval fails, it falls back to defaults ("user",
 * "localhost", "unknown").
 *
 * Colors used (ANSI escape codes):
 * - User@Host: Green (\033[1;32m)
 * - CWD: Blue (\033[1;34m)
 * - Reset: \033[0m
 */
void type_prompt() {
  char *user = getenv("USERNAME"); // Windows
  if (!user)
    user = getenv("USER"); // Linux
  if (!user)
    user = "user";

  char host[1024];
#ifdef _WIN32
  strcpy(host, "localhost");
#else
  if (gethostname(host, sizeof(host)) == -1) {
    strcpy(host, "localhost");
  }
#endif

  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    strcpy(cwd, "unknown");
  }

  // Print Prompt: user@host:cwd$
  printf("\033[1;32m%s@%s\033[0m:\033[1;34m%s\033[0m$ ", user, host, cwd);
}

/**
 * @brief Reads a line of input from standard input.
 *
 * This function handles reading a full line of text from the user.
 * It detects End-Of-File (EOF) to gracefully exit the shell.
 *
 * @note This implementation relies on `getline`, which is a POSIX standard.
 * On Windows systems without MinGW/Cygwin, a replacement or fallback
 * (like `fgets`) may be required.
 *
 * @param line Pointer to the buffer where the allocated string is stored.
 * @param len Pointer to the size variable for the buffer.
 */
void read_line(char **line, size_t *len) {
  // Check for getline availability or use a fallback if compiling on MSVC
  if (getline(line, len, stdin) == -1) {
    if (feof(stdin)) {
      // EOF (Ctrl+D / Ctrl+Z) received
      exit(EXIT_SUCCESS);
    } else {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }
}
