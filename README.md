# C Shell

A comprehensive custom UNIX-like shell written in C, featuring dynamic prompts, built-in commands, process management, file operations, and network-based manual page fetching.

## Features

- **Dynamic prompt** showing `username@hostname:cwd$` with home directory displayed as `~`
- **Built-in commands** for navigation, file operations, process management, and system utilities
- **Command chaining** support with `;` separator
- **Persistent command history** across sessions
- **Network functionality** for fetching manual pages from the internet
- **Process information display** with detailed system metrics
- **File and directory search** with advanced filtering options
- **Modular code organization** with separate `.c` and `.h` files

---

## Building

```bash
gcc main.c globals.c prompt.c hop.c reveal.c log.c proclore.c seek.c echo.c iman.c -o cshell
```

## Running

```bash
./cshell
```

---

## Commands Reference

### hop — change directory

Navigate between directories with support for relative paths, home directory shortcuts, and previous directory recall.

**Usage**
```bash
hop [path]
```

**Examples**
```bash
# Go to home directory
hop
hop ~

# Go up one level
hop ..

# Return to last directory
hop -

# Navigate to subdirectory of home
hop ~/Documents
hop ~/projects/myapp

# Relative path navigation
hop ../folder
hop ../../parent

# Absolute path
hop /usr/local/bin
hop /etc/nginx

# Multiple level navigation
hop ../../../root/folder
```

---

### reveal — list directory contents

Display directory contents with various formatting options and filtering capabilities.

**Usage**
```bash
reveal [flags] [path]
```

**Flags**
- `-a` : Show all files including hidden files (starting with `.`)
- `-l` : Long format showing permissions, size, date, etc.

**Examples**
```bash
# Basic listing (current directory)
reveal

# List specific directory
reveal /home/user
reveal ~/Documents

# Show hidden files
reveal -a
reveal -a ~/

# Long format
reveal -l
reveal -l /etc

# Combined flags
reveal -al
reveal -la ~/projects
reveal -al /var/log

# Different directories with flags
reveal -a ../parent
reveal -l ~/Downloads
reveal -al /usr/bin
```

---

### log — command history management

Manage and replay command history with persistent storage across sessions.

**Usage**
```bash
log [flags]
!<index>
```

**Flags**
- `-d` : Delete entire command history

**Examples**
```bash
# Display last 15 commands
log

# Clear all history
log -d

# Re-execute command by index
!1
!7
!15

# Example workflow
log
# Output:
#    1  hop ~/projects
#    2  reveal -l
#    3  echo "Hello World"
#    4  seek -f main.c

# Re-run command #2
!2
# Executes: reveal -l
```

---

### proclore — process information display

Display detailed information about running processes including status, memory usage, and executable path.

**Usage**
```bash
proclore [pid]
```

**Examples**
```bash
# Show current shell process info
proclore

# Show specific process by PID
proclore 1234
proclore 5678

# Example output
proclore
# pid : 40152
# process Status : R+
# Process Group : 40152
# Virtual memory : 419928195072
# executable Path : /Users/user/cshell
```

**Process Status Codes**
- `R` / `R+` : Running (+ indicates foreground)
- `S` / `S+` : Sleeping in interruptible wait (+ indicates foreground)
- `T` : Stopped
- `Z` : Zombie

---

### seek — file and directory search

Search for files and directories within a target directory tree with advanced filtering options.

**Usage**
```bash
seek [flags] <search_term> [target_directory]
```

**Flags**
- `-f` : Search only for files (ignore directories)
- `-d` : Search only for directories (ignore files)
- `-e` : Execute action if single match found (change to directory)

**Examples**
```bash
# Basic search in current directory
seek main.c
seek config

# Search in specific directory
seek stdio.h /usr/include
seek README ~/projects

# Search only for files
seek -f main.c
seek -f "*.txt" ~/Documents

# Search only for directories
seek -d bin
seek -d src ~/projects

# Search with execute flag
seek -e config.txt
seek -e -f main.c
seek -e -d projects

# Combined flags
seek -ef main.c ~/src
seek -ed build ~/projects

# Search patterns
seek test
seek -f test.
seek -d test_dir
```

**Note**: `-d` and `-f` flags cannot be used together and will return "Invalid flags!" error.

---

### echo — display text

Print text to standard output with optional formatting control.

**Usage**
```bash
echo [flags] [text...]
```

**Flags**
- `-n` : Do not output trailing newline

**Examples**
```bash
# Basic text output
echo Hello World
echo "Hello, World!"

# Multiple arguments
echo one two three
echo "first argument" "second argument"

# Without trailing newline
echo -n "No newline"
echo -n Hello; echo World

# Special characters and quotes
echo "Hello 'World'"
echo 'Hello "World"'
echo Hello\tWorld

# Empty echo
echo
echo -n
```

---

### iMan — internet manual pages

Fetch and display manual pages from the internet using the man.he.net service.

**Usage**
```bash
iMan <command_name>
```

**Examples**
```bash
# Fetch manual for common commands
iMan ls
iMan grep
iMan find

# System calls and library functions
iMan printf
iMan malloc
iMan socket

# Network utilities
iMan curl
iMan wget
iMan ssh

# File operations
iMan chmod
iMan chown
iMan cp

# Process management
iMan ps
iMan kill
iMan top

# Text processing
iMan awk
iMan sed
iMan sort

# If command doesn't exist, shows error page
iMan nonexistentcommand
```

**Note**: If more than one argument is provided, iMan considers only the first argument and ignores the rest.

---

## Command Chaining

Execute multiple commands in sequence using the semicolon (`;`) separator.

**Examples**
```bash
# Basic chaining
hop ~/projects; reveal -l

# Multiple commands
echo "Starting..."; hop /tmp; reveal -a; hop -

# Mixed command types
log; proclore; echo "Done"

# Search and navigate
seek -ed src; reveal -l; hop -

# Complex workflows
hop ~/; reveal -al Documents; hop Documents; seek -f "*.txt"
```

---

## Error Handling

The shell provides comprehensive error handling for various scenarios:

- **Invalid commands**: `Unknown command: invalidcmd`
- **Permission errors**: `Missing permissions for task!`
- **File not found**: `No such file or directory`
- **Network errors**: Connection timeouts and DNS resolution failures
- **Process errors**: `No such process found`
- **Search errors**: `No match found!`
- **Flag conflicts**: `Invalid flags!` (for seek command with both -d and -f)

---

## Notes

- **Case sensitivity**: All commands and file operations are case-sensitive
- **Path resolution**: Supports `~` for home directory, `.` for current, `..` for parent
- **History persistence**: Command history is saved across shell sessions
- **Network dependency**: `iMan` requires internet connection
- **Platform compatibility**: Optimized for macOS with fallback support for Linux
- **Memory management**: Automatic cleanup of dynamically allocated resources
- **HTML filtering**: `iMan` automatically removes HTML tags and converts entities for clean output