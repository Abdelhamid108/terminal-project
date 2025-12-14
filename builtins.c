/**
 * @file builtins.c
 * @brief Implementation of built-in shell commands.
 *
 * This file contains the function implementations for all the built-in
 * commands supported by the shell (e.g., cd, exit, help, history) and
 * the lookup tables (arrays) used to map command names to their
 * corresponding functions.
 *
 * @author Abdelhamid
 * @date 2025-12-14
 */

#include "shell.h"

// Forward declarations of built-in functions
int shell_cd(char **args);
int shell_exit(char **args);
int shell_help(char **args);
int shell_clear(char **args);
int shell_about(char **args);
int shell_history(char **args);
int shell_count(char **args);
int shell_cp(char **args);
int shell_mv(char **args);
int shell_rm(char **args);

/**
 * @brief Array of built-in command names.
 */
char *builtin_str[] = {"cd",      "exit",  "help", "clear", "about",
                       "history", "count", "cp",   "mv",    "rm"};

/**
 * @brief Array of function pointers corresponding to built-in commands.
 */
int (*builtin_func[])(char **) = {
    &shell_cd,      &shell_exit,  &shell_help, &shell_clear, &shell_about,
    &shell_history, &shell_count, &shell_cp,   &shell_mv,    &shell_rm};

/**
 * @brief Calculates the number of registered built-in commands.
 * @return int The count of built-in commands.
 */
int shell_num_builtins() { return sizeof(builtin_str) / sizeof(char *); }

/* =========================================================================
 *                          Built-in Command Implementations
 * ========================================================================= */

/**
 * @brief Changes the current working directory.
 *
 * Checks if the directory argument is provided and uses the `chdir()`
 * system call to change the directory. Prints an error if the directory
 * is invalid or accessed improperly.
 *
 * @param args Null-terminated array of arguments (args[1] is the path).
 * @return int Always returns 1 to continue execution.
 */
int shell_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "shell: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("shell");
    }
  }
  return 1;
}

/**
 * @brief Exits the shell program.
 *
 * @param args Null-terminated array of arguments (unused).
 * @return int Returns 0 to signal the main loop to terminate.
 */
int shell_exit(char **args) {
  (void)args; // unused
  return 0;
}

/**
 * @brief Displays help information and list of available commands.
 *
 * @param args Null-terminated array of arguments (unused).
 * @return int Always returns 1 to continue execution.
 */
int shell_help(char **args) {
  (void)args; // unused
  printf("Custom Shell Help\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (int i = 0; i < shell_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
 * @brief Clears the terminal screen.
 *
 * Uses `system("cls")` on Windows and ANSI escape codes on POSIX systems.
 *
 * @param args Null-terminated array of arguments (unused).
 * @return int Always returns 1 to continue execution.
 */
int shell_clear(char **args) {
  (void)args; // unused
#ifdef _WIN32
  system("cls");
#else
  printf("\033[H\033[J");
#endif
  return 1;
}

/**
 * @brief Displays version and author information for the shell.
 *
 * @param args Null-terminated array of arguments (unused).
 * @return int Always returns 1 to continue execution.
 */
int shell_about(char **args) {
  (void)args; // unused
  printf("Custom Shell v1.0\n");
  printf("Developed for the Terminal Project.\n");
  return 1;
}

/**
 * @brief Displays the session command history.
 *
 * Delegates to the `print_history()` function in history.c.
 *
 * @param args Null-terminated array of arguments (unused).
 * @return int Always returns 1 to continue execution.
 */
int shell_history(char **args) {
  (void)args; // unused
  print_history();
  return 1;
}

/**
 * @brief Detailed statistics of a file: lines, words, and characters.
 *
 * Opens the specified file and iterates through it character by character,
 * counting newlines, spaces (state machine for words), and total bytes.
 *
 * @param args Null-terminated array of arguments (args[1] is filename).
 * @return int 1 to continue execution.
 */
int shell_count(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "shell: expected argument to \"count\"\n");
    return 1;
  }

  FILE *fp = fopen(args[1], "r");
  if (fp == NULL) {
    perror("shell");
    return 1;
  }

  int lines = 0;
  int words = 0;
  int chars = 0;
  int state = 0; // 0: whitespace, 1: word
  char c;

  while ((c = fgetc(fp)) != EOF) {
    chars++;
    if (c == '\n') {
      lines++;
    }

    // Word counting logic
    if (c == ' ' || c == '\n' || c == '\t') {
      state = 0;
    } else if (state == 0) {
      state = 1;
      words++;
    }
  }

  printf("Lines: %d\n", lines);
  printf("Words: %d\n", words);
  printf("Chars: %d\n", chars);

  fclose(fp);
  return 1;
}

/**
 * @brief Copies a file from source to destination.
 *
 * Performs a binary copy using a buffer.
 *
 * @param args Null-terminated array of arguments (src, dst).
 * @return int 1 to continue execution.
 */
int shell_cp(char **args) {
  if (args[1] == NULL || args[2] == NULL) {
    fprintf(stderr, "shell: expected source and destination for \"cp\"\n");
    return 1;
  }

  FILE *src = fopen(args[1], "rb");
  if (src == NULL) {
    perror("shell");
    return 1;
  }

  FILE *dst = fopen(args[2], "wb");
  if (dst == NULL) {
    fclose(src);
    perror("shell");
    return 1;
  }

  char buffer[1024];
  size_t bytes;
  while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
    fwrite(buffer, 1, bytes, dst);
  }

  fclose(src);
  fclose(dst);
  return 1;
}

/**
 * @brief Moves or renames a file.
 *
 * Uses the `rename()` system call.
 *
 * @param args Null-terminated array of arguments (old name, new name).
 * @return int 1 to continue execution.
 */
int shell_mv(char **args) {
  if (args[1] == NULL || args[2] == NULL) {
    fprintf(stderr, "shell: expected source and destination for \"mv\"\n");
    return 1;
  }

  if (rename(args[1], args[2]) != 0) {
    perror("shell");
  }
  return 1;
}

/**
 * @brief Deletes a file.
 *
 * Uses the `remove()` system call.
 *
 * @param args Null-terminated array of arguments (filename).
 * @return int 1 to continue execution.
 */
int shell_rm(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "shell: expected argument to \"rm\"\n");
    return 1;
  }

  if (remove(args[1]) != 0) {
    perror("shell");
  }
  return 1;
}
