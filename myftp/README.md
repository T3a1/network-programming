# myftp

An FTP server in C that follows the commands and reply codes of
[RFC 959](https://www.rfc-editor.org/rfc/rfc959). It serves several clients at
the same time with a single `poll()` loop, without threads.

## Commands

| Group | Commands |
|-------|----------|
| Authentication | `USER`, `PASS` (user `Anonymous`, empty password) |
| Navigation | `PWD`, `CWD`, `CDUP` |
| Data connection | `PASV` (passive), `PORT` (active) |
| Files | `LIST`, `RETR`, `STOR`, `DELE` |
| Other | `HELP`, `NOOP`, `QUIT` |

Each command lives in its own file in `src/commands/` and is called by a
central dispatcher. File transfers (`RETR`, `STOR`) run in a child process,
so a large file does not block the other clients.

## Build and run

```sh
make
./myftp 2121 /path/to/root
```

Connect with any FTP client, for example `ftp localhost 2121` or `nc localhost 2121`.
