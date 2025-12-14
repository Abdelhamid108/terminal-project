# Custom Shell Project - Technical Manual

## Table of Contents
1.  [Overview](#overview)
2.  [Architecture](#architecture)
3.  [Detailed File Analysis](#detailed-file-analysis)
    *   [main.c](#mainc-entry-point)
    *   [shell.h](#shellh-header-definitions)
    *   [parser.c](#parserc-input-processing)
    *   [executor.c](#executorc-command-execution)
    *   [builtins.c](#builtinsc-internal-commands)
    *   [history.c](#historyc-session-memory)
    *   [utils.c](#utilsc-user-interface)
4.  [Core Technical Concepts](#core-technical-concepts)
5.  [Building and Running](#building-and-running)

---

## Overview

This project is a fully functional command-line interpreter (Shell) written in C. It mimics the behavior of industry-standard shells like Bash or Zsh. It is designed to be **Cross-Platform**, running seamlessly on both **Windows** and **Linux/Unix** systems.

**Key capabilities:**
*   **REPL Cycle**: Read-Eval-Print Loop architecture.
*   **Process Management**: Creates and manages child processes for executing programs.
*   **IO Redirection**: altering standard input/output streams (e.g., `ls > file.txt`).
*   **Piping**: Chaining commands together (e.g., `ls | grep .c`) (POSIX only).
*   **Persistence**: Saves command history to a file.

---

## Architecture

The shell operates in a continuous loop called the **REPL**:

1.  **READ**: The shell displays a prompt (e.g., `user@host$`) and waits for you to type a command.
2.  **PARSE**: The typed string (e.g., `ls -l`) is broken down into meaningful tokens (`ls`, `-l`).
3.  **EXECUTE**: The shell determines if the command is internal (Built-in) or external (System Program).
    *   *Built-in*: Executed directly by the shell process.
    *   *External*: The shell creates a copy of itself (Fork), and replaces the copy with the new program (Exec).
4.  **LOOP**: The process repeats until the user types `exit`.

---

## Detailed File Analysis

### `main.c`: Entry Point
**Purpose**: The brain of the operation. It initializes the shell and kicks off the main loop.

**Key Functions**:
*   `main()`:
    1.  Calls `load_history()` to read previous commands from the `.shell_history` file.
    2.  Calls `shell_loop()` to start the interactive session.
    3.  On exit, calls `save_history()` to ensure your session is remembered.
*   `shell_loop()`:
    *   Infinite loop: `do { ... } while (status);`
    *   Three major steps inside:
        1.  `read_line()`: Get input.
        2.  `parse_input()`: Break input into arguments.
        3.  `execute_command()`: Run the command.
    *   **Memory Management**: Crucially, it frees the memory for `line` and `args` at the end of every loop iteration to prevent memory leaks.

### `shell.h`: Header Definitions
**Purpose**: The central connector. It ensures all `.c` files can execute functions defined in other files.

**Key Components**:
*   **Includes**: Manages conditional includes for Windows (`<process.h>`, `<direct.h>`) vs Linux (`<unistd.h>`, `<sys/wait.h>`). This is the secret sauce for cross-platform compatibility.
*   **Macros**:
    *   `MAX_INPUT_SIZE`: Limits logical command length.
    *   `DELIMITERS`: Defines what separates words (spaces, tabs).
*   **Prototypes**: Declares "signatures" of functions like `shell_cd`, `parse_input`, so the compiler knows they exist before it sees the full code.

### `parser.c`: Input Processing
**Purpose**: Turning raw text into computer-readable lists.

**Key Function**: `parse_input(char *line)`
*   **Logic**:
    1.  It allocates an initial list size (`malloc`).
    2.  It uses `strtok()` (String Tokenizer) to split the string at every space or tab.
    3.  **Dynamic Resizing**: If you type a command with more arguments than the initial list can hold, it uses `realloc()` to grow the list size dynamically. This prevents the shell from crashing on long commands.
    4.  Returns a `char **` (a pointer to an array of string pointers), which determines the `argv` for the program.

### `executor.c`: Command Execution
**Purpose**: The muscle. It actually makes things happen.

**Key Concepts & Functions**:
*   `execute_command()`: The dispatcher.
    1.  **Pipes**: Checks for `|`. If found, it routes to the complex `execute_pipeline`.
    2.  **Built-ins**: Checks if the command is `cd`, `exit`, etc. If yes, it runs the C function directly.
    3.  **Redirection**: Scans for `>` or `<`.
    4.  **External**: If none of the above, it calls `launch_process`.

*   `launch_process()`: The OS Interface.
    *   **Linux**: Uses `fork()` to clone the current process. The child process then calls `execvp()`, which searches the system PATH for the command and executes it. The parent uses `waitpid()` to pause until the child is done.
    *   **Windows**: Uses `_spawnvp()`, which is a simplified Windows-specific way to spawning a new process synchronously.

*   `handle_redirection()`:
    *   Manipulates **File Descriptors**. Commands usually listen to `STDIN` (0) and write to `STDOUT` (1).
    *   This function uses `open()` to open a text file, and `dup2()` to essentially "rewire" the standard output to point to that file instead of the screen.

### `builtins.c`: Internal Commands
**Purpose**: Operations that *change the shell's state* must be built-in. An external program cannot change the working directory of the shell that launched it.

**Commands**:
*   `shell_cd`: Uses `chdir()` to change directory.
*   `shell_exit`: Returns `0`, which breaks the `shell_loop`.
*   `shell_help`, `shell_about`: Print info.
*   `shell_cp`, `shell_mv`, `shell_rm`: Simple implementations of file manipulation using standard C file I/O (`fopen`, `fread`, `fwrite`) and system calls (`rename`, `remove`).

### `history.c`: Session Memory
**Purpose**: User convenience. Remembers what you typed.

**Logic**:
*   Maintains a generic `history` array of strings.
*   **Circular Buffer**: If history exceeds `HISTORY_SIZE` (100), it deletes the oldest entry (index 0) and shifts all others down to make room for the new one.
*   **Persistence**:
    *   `load_history`: Reads `~/.shell_history` on startup.
    *   `save_history`: Writes the array back to the file on exit.

### `utils.c`: User Interface
**Purpose**: improving UX.

**Key Function**: `type_prompt()`
*   Retrieves environment variables (`USER`, `HOSTNAME`).
*   Gets current directory (`getcwd`).
*   **ANSI Colors**: Uses special character sequences (e.g., `\033[1;32m`) to print the prompt in Green and Blue, making it distinct from command output.

---

## Core Technical Concepts

### 1. Fork-Exec (Linux)
In Unix, you don't just "run" a program. You:
1.  **Fork**: Clone yourself. Now there are two identical processes (Parent and Child).
2.  **Exec**: The Child replaces its brain (memory) with a new program (e.g., `ls`).
3.  **Wait**: The Parent waits for the Child to finish.

### 2. File Descriptors (FD)
Every running process has access to:
*   `0`: Standard Input (Keyboard)
*   `1`: Standard Output (Screen)
*   `2`: Standard Error (Screen)
Redirection simply closes `1` and assigns the file ID of `output.txt` to slot `1`. The program doesn't know the difference; it just writes to `1`.

### 3. Piping (`|`)
Connects the **Output** of Command A to the **Input** of Command B.
*   The shell creates a "pipe" (a memory buffer).
*   Command A writes into the pipe.
*   Command B reads from the pipe.
*   This allows powerful chaining: `cat logs.txt | grep "Error" | count`

---

## Building and Running

### Prerequisites
*   A C Compiler (GCC for Linux/MinGW, or MSVC for Windows).
*   Make (optional).

### Steps
1.  **Compile**:
    Run `make` in the terminal.
    *   It compiles each `.c` file into a `.o` (object) file.
    *   It links all `.o` files into the final `myshell` executable.

2.  **Run**:
    ```bash
    ./myshell
    ```

3.  **Clean**:
    To remove compiled files: `make clean`

---

## Testing

The project includes several text files used for testing different features of the shell. You can run these by redirecting input: `./myshell < test_input.txt`

| File | Purpose |
|------|---------|
| `test_input.txt` | Tests basic commands like `ls`, `pwd`, `mkdir`, `cd`. |
| `test_phase2.txt` | Tests built-in commands like `help`, `history`, `count`. |
| `test_phase3.txt` | Tests I/O redirection (`>`, `<`) and piping. |
| `test_enhancements.txt` | Tests extra commands like `cp`, `mv`, `rm`. |
| `test_final.txt` | A comprehensive test of multiple features. |

*Note: Files like `hello.txt`, `ls_out.txt`, or `final_out.txt` are generated outputs from running these tests and can be safely deleted.*

