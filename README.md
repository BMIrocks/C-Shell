# C Shell

A small custom LINUX-like shell written in C, featuring:

- A **dynamic prompt** showing `username@hostname:cwd$`, with your launch directory as `~` and relative paths inside it.
- A built‑in `hop` command for changing directories.
- A `reveal` command for listing directory contents (like `ls`), with `-a` and `-l` flags and ANSI color coding.
- A persistent `log` command (like `history`) storing commands across sessions, with support for `!index` replay and `-d` to delete history.
- Support for chaining multiple commands on one line, separated by `;`.
- Modular code organization: separate `.c` and `.h` files.

---

## Building

```bash
gcc main.c globals.c prompt.c hop.c reveal.c log.c -o cshell
```

## Running

```bash
./cshell
```

## Commands

### hop — change directory

**Usage**  
```bash
hop [path]

# Go home
hop

# Go up one level
hop ..

# Return to last directory
hop -

# Into a subfolder of your home
hop ~/projects

# Into ../foo relative to current cwd
hop ../foo

# Absolute path
hop /usr/local/bin
```

### reveal — list directory contents

**Usage** 
``` bash
reveal [flags] [path]

# Default (non-hidden only)
reveal

# All entries including dotfiles
reveal -a

# Long format
reveal -l

# Combined flags
reveal -al ~/Documents

# List /etc in long format
reveal -l /etc

# Show hidden files in current dir
reveal -a .
```

### log — command history

**Usage** 
``` bash
log [flags]
!<index>

# Show last 15 commands
log

# Delete entire history
log -d

# Re-run command #7
!7

# Example: show history then re-run
log
#    5  hop ~/projects
#    6  reveal -l
#    7  ls -a
!6
# runs: reveal -l
