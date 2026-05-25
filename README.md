# C Shell

A small custom Unix-like shell written in C, featuring:

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
