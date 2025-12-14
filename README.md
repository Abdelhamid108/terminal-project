# Custom Shell Project

## Overview
This project is a custom command-line shell implementation in C. It serves as a study in systems programming, process management, and OS interaction. The shell mimics standard Linux bash behavior while offering cross-platform support for both Windows and Linux environments.

## Features

### Core Capabilities
- **Command Execution**: execute standard external programs (e.g., `ls`, `grep`, `python`).
- **Cross-Platform**: Runs on Linux (fork/exec) and Windows (_spawnvp).
- **History Management**: Persistent command history saved to `.shell_history`.
- **Input Parsing**: Tokenizes user input effectively.

### Built-in Commands
The shell includes a suite of built-in commands:
- `cd [dir]`: Change directory.
- `exit`: Terminate the shell.
- `help`: Show available commands.
- `clear`: Clear the screen.
- `about`: Project information.
- `history`: View command history.
- `count [file]`: Count lines/words/chars in a text file.
- `cp [src] [dst]`: Copy a file.
- `mv [src] [dst]`: Move/Rename a file.
- `rm [file]`: Delete a file.

### Advanced Features
- **Redirection**:
    - `>`: Output redirection (write to file).
    - `<`: Input redirection (read from file).
- **Piping (`|`)**: Chain commands (Linux/POSIX only).

## Project Structure

| File | Description |
|------|-------------|
| `main.c` | Entry point. Initializes shell and runs the main loop. |
| `shell.h` | Header file with function prototypes and global constants. |
| `parser.c` | Tokenizes input strings into arguments. |
| `executor.c` | Handles execution logic, including pipes and redirection. |
| `builtins.c` | Implementation of internal commands (cd, help, etc.). |
| `history.c` | History management (load, save, add). |
| `utils.c` | Utility functions (prompt display, input reading). |
| `Makefile` | Build configuration script. |

## Build and Run

### Prerequisites
- GCC Compiler (or compatible C compiler).
- Make (optional, for using Makefile).

### Compilation
Run the following command in the project directory:
```bash
make
```
This produces an executable named `myshell` (or `myshell.exe` on Windows).

### Usage
Start the shell:
```bash
./myshell
```

Sample session:
```bash
user@host:~$ ls -l
user@host:~$ echo "Hello" > test.txt
user@host:~$ count test.txt
Lines: 1
Words: 1
Chars: 6
user@host:~$ history
1 ls -l
2 echo "Hello" > test.txt
3 count test.txt
user@host:~$ exit
```

## Author
Abdelhamid
