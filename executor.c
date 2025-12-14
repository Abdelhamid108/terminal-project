/**
 * @file executor.c
 * @brief Command execution module.
 *
 * This file handles the actual execution of commands, including:
 * - Launching external processes (fork/exec on Linux, _spawnvp on Windows).
 * - Handling input/output redirection.
 * - Managing pipelines (pipes).
 * - Delegating to built-in commands.
 *
 * @author Abdelhamid
 * @date 2025-12-14
 */

#include "shell.h"

/* External references to built-in command tables defined in builtins.c */
extern char *builtin_str[];
extern int (*builtin_func[])(char **);

/**
 * @brief Launches an external process using system calls.
 *
 * On Linux/macOS:
 * Uses `fork()` to create a child process and `execvp()` to replace the
 * child's image with the new program. Parent waits for child to finish.
 *
 * On Windows:
 * Uses `_spawnvp()` to synchronously execute the new process.
 *
 * @param args Null-terminated array of arguments (args[0] is the command).
 * @return int Always returns 1 to indicate the shell should continue running.
 */
int launch_process(char **args) {
#ifdef _WIN32
  // Windows implementation using spawn
  int status = _spawnvp(_P_WAIT, args[0], (const char *const *)args);
  if (status == -1) {
    perror("shell");
  }
  return 1;
#else
  // POSIX implementation using fork/exec
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("shell");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("shell");
  } else {
    // Parent process waits for child
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
#endif
}

/**
 * @brief Handles input (`<`) and output (`>`) redirection tokens.
 *
 * Scans the argument list for redirection symbols. If found, opens the
 * specified files and updates the `in_fd` and `out_fd` parameters.
 * The redirection symbols and filenames are removed (set to NULL) in args
 * to prevent them from being passed to the command.
 *
 * @param args The null-terminated array of arguments.
 * @param in_fd Pointer on an integer to store the input file descriptor.
 * @param out_fd Pointer on an integer to store the output file descriptor.
 */
void handle_redirection(char **args, int *in_fd, int *out_fd) {
  for (int i = 0; args[i] != NULL; i++) {
    if (strcmp(args[i], ">") == 0) {
      args[i] = NULL; // Truncate args here
      // Open file for writing, create if not exists, truncate if exists
      *out_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (*out_fd < 0)
        perror("shell");
    } else if (strcmp(args[i], "<") == 0) {
      args[i] = NULL;
      // Open file for reading
      *in_fd = open(args[i + 1], O_RDONLY);
      if (*in_fd < 0)
        perror("shell");
    }
  }
}

// Pipeline execution logic (POSIX only)
#ifndef _WIN32
/**
 * @brief Executes a pipeline of commands connected by pipe `|`.
 *
 * This function creates necessary pipes and forks processes for each command
 * in the pipeline. It sets up `dup2` to redirect stdout of a command to stdin
 * of the next command.
 *
 * @param cmd_args Array of string arrays (command arguments).
 * @param num_cmds Total number of commands in the pipeline.
 * @return int Always returns 1.
 */
int execute_pipeline(char ***cmd_args, int num_cmds) {
  int i;
  int pipefd[2 * (num_cmds - 1)];
  int status;

  // Create all necessary pipes
  for (i = 0; i < num_cmds - 1; i++) {
    if (pipe(pipefd + i * 2) < 0) {
      perror("pipe");
      return 1;
    }
  }

  int pid;
  for (i = 0; i < num_cmds; i++) {
    pid = fork();
    if (pid == 0) {
      // Child Process Logic

      // Connect input from previous pipe (if not first command)
      if (i != 0) {
        if (dup2(pipefd[(i - 1) * 2], 0) < 0)
          perror("dup2");
      }
      // Connect output to next pipe (if not last command)
      if (i != num_cmds - 1) {
        if (dup2(pipefd[i * 2 + 1], 1) < 0)
          perror("dup2");
      }

      // Close all pipe file descriptors in child
      for (int j = 0; j < 2 * (num_cmds - 1); j++) {
        close(pipefd[j]);
      }

      if (execvp(cmd_args[i][0], cmd_args[i]) < 0) {
        perror("execvp");
        exit(EXIT_FAILURE);
      }
    } else if (pid < 0) {
      perror("fork");
      return 1;
    }
  }

  // Parent closes all pipe fds
  for (i = 0; i < 2 * (num_cmds - 1); i++) {
    close(pipefd[i]);
  }

  // Wait for all children to complete
  for (i = 0; i < num_cmds; i++) {
    wait(&status);
  }
  return 1;
}
#endif

/**
 * @brief Main execution dispatch logic.
 *
 * Determines if logic involves pipes, built-in commands, or simple external
 * execution. Manages file descriptor copying for redirection restoration.
 *
 * @param args Null-terminated array of arguments (tokens).
 * @return int 1 to continue execution, 0 to exit (if command is 'exit').
 */
int execute_command(char **args) {
  int i;
  int in_fd = -1, out_fd = -1;

  // Save original stdin/stdout to restore later
  int saved_stdin = dup(STDIN_FILENO);
  int saved_stdout = dup(STDOUT_FILENO);

  if (args[0] == NULL) {
    // Empty command
    return 1;
  }

  // 1. Check for Pipes ("|")
  int num_pipes = 0;
  for (i = 0; args[i] != NULL; i++) {
    if (strcmp(args[i], "|") == 0)
      num_pipes++;
  }

  if (num_pipes > 0) {
#ifdef _WIN32
    fprintf(stderr, "Piping not supported on Windows mode.\n");
    return 1;
#else
    // Split args into multiple commands for pipeline
    char ***cmd_args = malloc((num_pipes + 1) * sizeof(char **));
    int cmd_idx = 0;
    cmd_args[cmd_idx++] = args;
    for (i = 0; args[i] != NULL; i++) {
      if (strcmp(args[i], "|") == 0) {
        args[i] = NULL;                     // Terminate current command args
        cmd_args[cmd_idx++] = &args[i + 1]; // Start next command
      }
    }
    int status = execute_pipeline(cmd_args, num_pipes + 1);
    free(cmd_args);
    return status;
#endif
  }

  // 2. Check for Built-in Commands
  for (i = 0; i < shell_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  // 3. Handle Redirection (if any)
  handle_redirection(args, &in_fd, &out_fd);

  if (in_fd != -1) {
    dup2(in_fd, STDIN_FILENO);
    close(in_fd);
  }
  if (out_fd != -1) {
    dup2(out_fd, STDOUT_FILENO);
    close(out_fd);
  }

  // 4. Launch External Process
  int status = launch_process(args);

  // Restore original stdin/stdout
  dup2(saved_stdin, STDIN_FILENO);
  dup2(saved_stdout, STDOUT_FILENO);
  close(saved_stdin);
  close(saved_stdout);

  return status;
}
