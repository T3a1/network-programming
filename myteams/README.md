# myteams

A team collaboration tool in C++ (server and command-line client), in the
spirit of Microsoft Teams. Users log in, create teams, channels inside teams,
threads inside channels and replies inside threads, and send private messages.
Connected users receive notifications in real time.

## Features

- TCP server with one `poll()` loop for every client
- Custom text protocol with status codes, described in [docs/protocol.md](docs/protocol.md)
- Every entity identified by a UUID
- Data saved to disk and reloaded at startup ([docs/persistence.md](docs/persistence.md))
- Logging through `libmyteams`, the library given with the subject (`libs/myteams/`)

## Client commands

`/help` `/login` `/logout` `/users` `/user` `/send` `/messages` `/subscribe`
`/subscribed` `/unsubscribe` `/use` `/create` `/list` `/info`

`/use` sets the context (team, channel, thread), and `/create`, `/list` and
`/info` act on that context.

## Build and run

```sh
make
export LD_LIBRARY_PATH=$PWD/libs/myteams
./myteams_server 4242
./myteams_cli 127.0.0.1 4242
```

## Layout

```
client/   command-line client: commands, notifications
server/   sockets, poll loop, command engine, data model (teams, channels, threads, comments)
libs/     logging library provided by Epitech
docs/     protocol and persistence format
```
