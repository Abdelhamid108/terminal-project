/**
 * @file shell.h
 * @brief Header file for the custom shell implementation.
 *
 * This file contains the function declarations, macro definitions, and
 * required library includes for the shell. It checks for the operating
 * system (Windows or Linux/macOS) to include the appropriate system headers.
 *
 * @author Abdelhamid
 * @date 2025-12-14
 */

#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Cross-platform system headers */
#ifdef _WIN32
#include <direct.h>
#include <fcntl.h>
#include <io.h>
#include <process.h>
/* Map POSIX function names to Windows equivalents */
#define chdir _chdir
#define getcwd _getcwd
#define open _open
#define dup _dup
#define dup2 _dup2
#define close _close
#define O_WRONLY _O_WRONLY
#define O_CREAT _O_CREAT
#define O_TRUNC _O_TRUNC
#define O_RDONLY _O_RDONLY
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#else
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#endif

/**
 * @def MAX_INPUT_SIZE
 * @brief Maximum number of characters allowed in a single command line input.
 */
#define MAX_INPUT_SIZE 1024

/**
 * @def MAX_ARGS
 * @brief Maximum number of arguments allowed for a single command.
 */
#define MAX_ARGS 64

/**
 * @def DELIMITERS
 * @brief Characters used to split command line input into tokens.
 *
 * Includes space, tab, carriage return, newline, and alert code.
 */
#define DELIMITERS " \t\r\n\a"

/* =========================================================================
 *                               Function Declarations
 * ========================================================================= */

/**
 * @brief Main loop of the shell.
 *
 * continuously:
 * 1. Displays the prompt.
 * 2. Reads user input.
 * 3. Parses the input into arguments.
 * 4. Executes the command.
 *
 * The loop terminates when the user executes 'exit' or an end-of-file (EOF) is
 * encountered.
 */
void shell_loop();

/**
 * @brief Parses a line of input into an array of strings (tokens).
 *
 * @param line The input string to parse.
 * @return A null-terminated array of strings (char**), or NULL on failure.
 */
char **parse_input(char *line);

/**
 * @brief Decides whether to execute a built-in command or launch an external
 * process.
 *
 * @param args Null-terminated array of command arguments.
 * @return 1 to continue execution, 0 to terminate the shell.
 */
int execute_command(char **args);

/**
 * @brief Launches an external program using system calls.
 *
 * On Linux, it uses fork() + execvp().
 * On Windows, it uses _spawnvp().
 *
 * @param args Null-terminated array of command arguments.
 * @return Always returns 1 to continue execution.
 */
int launch_process(char **args);

/* -------------------------------------------------------------------------
 *                               Built-in Commands
 * ------------------------------------------------------------------------- */

/**
 * @brief Changes the current working directory.
 * @param args Command arguments (args[1] is the target directory).
 * @return 1 to continue execution.
 */
int shell_cd(char **args);

/**
 * @brief Exits the shell.
 * @param args Command arguments (unused).
 * @return 0 to terminate the shell.
 */
int shell_exit(char **args);

/**
 * @brief Displays help information including a list of built-in commands.
 * @param args Command arguments (unused).
 * @return 1 to continue execution.
 */
int shell_help(char **args);

/**
 * @brief Clears the terminal screen.
 * @param args Command arguments (unused).
 * @return 1 to continue execution.
 */
int shell_clear(char **args);

/**
 * @brief Displays information about the shell.
 * @param args Command arguments (unused).
 * @return 1 to continue execution.
 */
int shell_about(char **args);

/**
 * @brief Displays the command history.
 * @param args Command arguments (unused).
 * @return 1 to continue execution.
 */
int shell_history(char **args);

/**
 * @brief Counts lines, words, and characters in a file.
 * @param args Command arguments (args[1] is the filename).
 * @return 1 to continue execution.
 */
int shell_count(char **args);

/**
 * @brief Copies a file from source to destination.
 * @param args Command arguments (args[1] source, args[2] destination).
 * @return 1 to continue execution.
 */
int shell_cp(char **args);

/**
 * @brief Moves or renames a file.
 * @param args Command arguments (args[1] source, args[2] destination).
 * @return 1 to continue execution.
 */
int shell_mv(char **args);

/**
 * @brief Removes (deletes) a file.
 * @param args Command arguments (args[1] is the filename).
 * @return 1 to continue execution.
 */
int shell_rm(char **args);

/**
 * @brief Returns the number of built-in commands.
 * @return The count of built-in commands available.
 */
int shell_num_builtins();

/* -------------------------------------------------------------------------
 *                               History Management
 * ------------------------------------------------------------------------- */

/**
 * @brief Adds a command line to the history list.
 * @param line The command line string to add.
 */
void add_history(char *line);

/**
 * @brief Prints the stored history to the standard output.
 */
void print_history();

/**
 * @brief Loads history from a file (e.g., .shell_history) into memory.
 */
void load_history();

/**
 * @brief Saves the current history from memory to a file.
 */
void save_history();

/* -------------------------------------------------------------------------
 *                               Utilities
 * ------------------------------------------------------------------------- */

/**
 * @brief Displays the shell prompt.
 *
 * content commonly includes user@host:cwd$
 */
void type_prompt();

/**
 * @brief Reads a line of input from standard input.
 *
 * @param line Pointer to the buffer where the line will be stored.
 * @param len Pointer to the size of the buffer.
 */
void read_line(char **line, size_t *len);

#endif /* SHELL_H */
