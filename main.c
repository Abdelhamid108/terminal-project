/**
 * @file main.c
 * @brief Entry point for the Custom Shell application.
 *
 * This file contains the main function which initializes the shell environment,
 * loads the command history, and enters the main execution loop. It serves as
 * the central controller for the shell's operation.
 *
 * @author Abdelhamid
 * @date 2025-12-14
 */

#include "shell.h"

/**
 * @brief Main entry point of the shell program.
 *
 * The main function performs the following steps:
 * 1. Initializes the shell (loading configuration/history).
 * 2. Enter the main shell loop (`shell_loop`).
 * 3. On exit, saves the history and performs necessary cleanup.
 *
 * @param argc Argument count (unused).
 * @param argv Argument vector (unused).
 * @return int Exit status (EXIT_SUCCESS).
 */
int main(int argc, char **argv) {
  // Suppress unused variable warnings
  (void)argc;
  (void)argv;

  // Load history from file
  load_history();

  // Start the main shell loop
  shell_loop();

  // Save history before exiting
  save_history();

  return EXIT_SUCCESS;
}

/**
 * @brief The main execution loop of the shell.
 *
 * This function handles the core REPL (Read-Eval-Print Loop) logic:
 * 1. **Read**: Display prompt and read a line of input.
 * 2. **Record**: Add the command line to history.
 * 3. **Parse**: Tokenize the input string into arguments.
 * 4. **Execute**: Run the parsed command (built-in or external).
 * 5. **Cleanup**: Free allocated memory for the line and arguments.
 *
 * The loop runs indefinitely until `execute_command` returns 0 (e.g., on
 * 'exit').
 */
void shell_loop() {
  char *line = NULL;
  size_t len = 0;
  char **args;
  int status = 1;

  do {
    type_prompt();
    read_line(&line, &len);

    // Add non-empty lines to history
    if (line && *line != '\0') {
      add_history(line);
    }

    args = parse_input(line);
    status = execute_command(args);

    // cleanup
    if (line) {
      free(line);
      line = NULL;
    }
    if (args) {
      free(args);
      args = NULL;
    }
  } while (status);
}
