# C Shell

A custom shell written in C for Ubuntu/Linux with built-in commands for navigation, listing files, command history, process inspection, searching, and fetching manual pages.

## Ubuntu 24.04 Build And Run

Install the compiler toolchain:

```bash
sudo apt update
sudo apt install build-essential
```

Build the normal binary:

```bash
gcc -Wall -Wextra -Wpedantic main.c globals.c prompt.c hop.c reveal.c log.c proclore.c seek.c echo.c iman.c -o cshell
```

Build a debug binary with sanitizers:

```bash
gcc -Wall -Wextra -Wpedantic -fsanitize=address,undefined -g main.c globals.c prompt.c hop.c reveal.c log.c proclore.c seek.c echo.c iman.c -o cshell_debug
```

Run:

```bash
./cshell
```

Run the debug build:

```bash
ASAN_OPTIONS=detect_leaks=0 ./cshell_debug
```

`detect_leaks=0` is included because LeakSanitizer is often unreliable under traced/sandboxed environments.

## Behavior Notes

- The prompt shows `username@hostname:path$`.
- Paths under your real home directory are displayed with `~`.
- Command history is stored in `.cshell_log` in the directory where you launched `./cshell`.
- Commands can be chained with `;`.
- Single-quoted and double-quoted arguments are supported.
- `Ctrl+D` exits the shell cleanly.
- `iMan` needs network access.

## Commands

### `hop`

Change the current working directory.

```bash
hop
hop ~
hop -
hop ..
hop ~/Documents
hop /usr/bin
```

### `reveal`

List directory contents or inspect a single file.

```bash
reveal
reveal -a
reveal -l
reveal -la ~/Downloads
reveal main.c
reveal /etc
```

Flags:

- `-a` shows hidden entries
- `-l` prints long-format details

### `log`

Show history, clear history, or replay a numbered command.

```bash
log
log -d
!1
!7
```

### `proclore`

Show process details for the current shell or a specific PID.

```bash
proclore
proclore 1234
```

### `seek`

Search for files or directories whose names start with the given term.

```bash
seek main .
seek -f main .
seek -d src ~/projects
seek -e README .
```

Flags:

- `-f` searches only files
- `-d` searches only directories
- `-e` prints the file contents or changes into the directory when there is exactly one match

### `echo`

Print text.

```bash
echo hello
echo hello world
echo "hello world"
echo -n "no newline"
```

### `iMan`

Fetch a manual page from `man.he.net`.

```bash
iMan ls
iMan printf
iMan grep
```

## Examples

Quoted arguments:

```bash
echo "hello world"
echo 'hello "world"'
```

Command chaining:

```bash
echo "starting"; reveal -l; proclore
hop ..; hop -
```

Search and inspect:

```bash
seek -f main .
seek -e README .
reveal -l main.c
```

## Troubleshooting

If you see messages such as `prompt.c:21:49: command not found`, that text was pasted into the shell and Bash tried to run the compiler output as commands. Re-run the actual compile command instead:

```bash
gcc -Wall -Wextra -Wpedantic main.c globals.c prompt.c hop.c reveal.c log.c proclore.c seek.c echo.c iman.c -o cshell
```
