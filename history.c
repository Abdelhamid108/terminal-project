/**
 * @file history.c
 * @brief Command history management module.
 *
 * This file handles the storage, retrieval, loading, and saving of command
 * history. It allows persistence of commands between shell sessions by
 * writing to a `.shell_history` file in the user's home directory.
 *
 * @author Abdelhamid
 * @date 2025-12-14
 */

#include "shell.h"

/** @brief Name of the history file. */
#define HISTORY_FILE ".shell_history"

/** @brief Maximum number of commands to store in history. */
#define HISTORY_SIZE 100

/** @brief Array to store history strings. */
char *history[HISTORY_SIZE];

/** @brief Current count of history items. */
int history_count = 0;

/**
 * @brief Adds a command line to the in-memory history list.
 *
 * Stores a copy of the command line in the history array. If the array is
 * full (HISTORY_SIZE), it shifts all elements down to remove the oldest
 * entry and make room for the new one at the end.
 *
 * @param line The command line string to add.
 */
void add_history(char *line) {
  if (history_count < HISTORY_SIZE) {
    history[history_count++] = strdup(line);
  } else {
    // Free the oldest history item
    free(history[0]);
    // Shift remaining items
    for (int i = 1; i < HISTORY_SIZE; i++) {
      history[i - 1] = history[i];
    }
    // Add new item at the end
    history[HISTORY_SIZE - 1] = strdup(line);
  }
}

/**
 * @brief Prints the current command history to stdout.
 *
 * Displays the list of stored commands, prefixed with their index number
 * (1-based).
 */
void print_history() {
  for (int i = 0; i < history_count; i++) {
    printf("%d %s\n", i + 1, history[i]);
  }
}

/**
 * @brief Loads command history from the persistence file.
 *
 * Constructs the path to `.shell_history` in the user's home directory
 * (supports both HOME and USERPROFILE environment variables). Reads the
 * file line by line and adds each valid command to the in-memory history.
 */
void load_history() {
  char path[1024];
  char *home = getenv("HOME");
  if (!home)
    home = getenv("USERPROFILE"); // Windows fallback

  if (!home)
    return; // Cannot determine home directory

  snprintf(path, sizeof(path), "%s/%s", home, HISTORY_FILE);

  FILE *fp = fopen(path, "r");
  if (fp) {
    char *line = NULL;
    size_t len = 0;
// Note: getline is a POSIX extension. Ensure compatibility or implementation
// exists.
#ifdef _WIN32
    // Simple fallback for Windows if getline is missing (or assume
    // MinGW/similar provides it) Ideally, use a custom read_line or fgets here
    // for portability. For now, assuming the build environment provides getline
    // or similar.
    char buf[1024];
    while (fgets(buf, sizeof(buf), fp)) {
      buf[strcspn(buf, "\n")] = 0;
      add_history(buf);
    }
#else
    while (getline(&line, &len, fp) != -1) {
      // Remove newline character
      line[strcspn(line, "\n")] = 0;
      add_history(line);
    }
    if (line)
      free(line);
#endif
    fclose(fp);
  }
}

/**
 * @brief Saves the current command history to the persistence file.
 *
 * Writes all commands currently in the in-memory history list to the
 * `.shell_history` file, overwriting its previous contents.
 */
void save_history() {
  char path[1024];
  char *home = getenv("HOME");
  if (!home)
    home = getenv("USERPROFILE"); // Windows fallback

  if (!home)
    return;

  snprintf(path, sizeof(path), "%s/%s", home, HISTORY_FILE);

  FILE *fp = fopen(path, "w");
  if (fp) {
    for (int i = 0; i < history_count; i++) {
      fprintf(fp, "%s\n", history[i]);
    }
    fclose(fp);
  }
}
