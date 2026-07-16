# Network

A collection of technical projects built as part of the Network Programming curriculum at Epitech. Each subfolder is a standalone project exploring different facets of network programming — from implementing a standard internet protocol from scratch to designing a custom client-server application protocol.

## Domain Overview

Network programming, as covered by these projects, focuses on building software that communicates reliably over TCP/IP: designing wire protocols, managing concurrent client connections, and structuring servers around non-blocking I/O. In practice here, that means:

- **Socket programming** — creating, binding, listening on, and managing TCP sockets directly via the BSD sockets API (`socket`, `bind`, `listen`, `accept`, `connect`).
- **Concurrent connection handling** — multiplexing many simultaneous client connections on a single thread using `poll()`, rather than spawning a thread or process per client.
- **Protocol design and parsing** — implementing a well-known standard protocol (FTP) to the letter of its RFC, as well as designing a custom application-layer protocol from scratch (commands, status codes, message framing).
- **Client/server architecture** — separating command parsing, execution, and response formatting into distinct layers so the protocol logic stays decoupled from the transport layer.
- **State and session management** — tracking authentication, working directories, transfer modes, or user/team/channel context across a persistent connection.

## Projects

### [`[2nd year] myFTP`](./%5B2nd%20year%5D%20myFTP/)

A File Transfer Protocol (FTP) server implemented in C, following the FTP RFC's command and reply-code conventions. Handles multiple simultaneous clients via `poll()`, and implements the core FTP command set — authentication (`USER`/`PASS`), navigation (`CWD`, `CDUP`, `PWD`), file operations (`RETR`, `STOR`, `DELE`, `LIST`), and both active and passive data connection modes (`PORT`/`PASV`). Each command is implemented as an isolated source file under `src/commands/`, dispatched through a central command executor.

### [`[2nd year] myTeams`](./%5B2nd%20year%5D%20myTeams/)

A team collaboration platform (in the spirit of Slack/Discord) built in C++, with a custom TCP client-server protocol documented in an accompanying RFC. Models a hierarchical structure of teams, channels, threads, and comments, plus private messaging between users, session/context management, and real-time notifications pushed to connected clients. Persistence is handled through line-delimited JSON-style files (`.AEL`), and the codebase is split cleanly into a `client/` (CLI, command objects, notifications) and a `server/` (connection handling, command engine, persistence).

## Common Threads

Both projects, despite implementing different protocols, share the same underlying networking approach:

- A `poll()`-driven event loop multiplexes all client connections without per-client threads or processes.
- Incoming data is parsed into discrete commands, each handled by its own dedicated function or class.
- Responses follow a structured format with explicit status/reply codes, mirroring real-world protocol conventions (FTP reply codes, custom MyTeams status codes).
- Server-side state (authentication, current directory/context, session data) is tracked per connection and persisted where needed.

## License

MIT License — see [`LICENSE`](./LICENSE). Copyright (c) 2026 Léandre Fouret.
