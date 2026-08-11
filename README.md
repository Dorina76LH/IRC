# ft_irc

An IRC server written in C++98, developed as part of the 42 school project `ft_irc`.

## Description

`ircserv` is an IRC server able to handle multiple clients simultaneously,
without forking, using non-blocking sockets and a single `poll()` (or
equivalent) call for all I/O operations (listening, reading, writing).

The server does not implement server-to-server communication and does not
provide an IRC client: you must connect to it using an existing reference
client.

## Technical choices

- **Input/Output multiplexing**: `poll()` 
- **Reference client**: Irrsi
- **Protocol reference**: RFC 1459
- **Standard**: strict C++98 (`-Wall -Wextra -Werror -std=c++98`)

## Build

```sh
make        # build ircserv
make clean  # remove object files
make fclean # remove object files and the executable
make re     # fclean + all
```

## Usage

```sh
./ircserv <port> <password>
```

- `port`: the port number on which the IRC server listens for incoming connections
- `password`: the connection password required by any client trying to connect

Example:
```sh
./ircserv 6667 mypassword
```

## Connecting with Irssi ? 

```

```

## Supported commands

### Mandatory (required by the subject)
- `PASS` — authenticate to the server
- `NICK` — set a nickname
- `USER` — set a username
- `JOIN` — join a channel
- `PRIVMSG` — send/receive private messages (to a user or a channel)
- `KICK` — eject a client from a channel *(channel operator)*
- `INVITE` — invite a client to a channel *(channel operator)*
- `TOPIC` — view or change a channel's topic *(operator-only if mode +t is set)*

- `MODE` — change a channel's mode *(channel operator)*
  - `i`: set/remove invite-only channel
  - `t`: set/remove the restriction of TOPIC to channel operators
  - `k`: set/remove the channel key (password)
  - `o`: give/take channel operator privilege
  - `l`: set/remove the channel user limit

### Additional (not required by the subject, added for robustness)
- `QUIT` — clean client disconnection
- `PART` — leave a channel

## Project structure

```
.
├── Makefile
├── includes/          # headers (.hpp)
│   ├── Client.hpp
│   ├── Channel.hpp
│   ├── Server.hpp
│   └── Commands.hpp
└── srcs/
    ├── main.cpp
    ├── Client.cpp
    ├── Channel.cpp
    ├── Server.cpp
    └── commands/       # one command per file
```

## Testing

```sh
nc -C 127.0.0.1 6667
```

Useful to check that partial/fragmented data is properly reassembled before
being processed (see the subject's example: sending `com`, `man`, `d\n`
separately with Ctrl+D).

## General rules (from the subject)

- The server must never crash, under any circumstances (including when it
  runs out of memory), and must never quit unexpectedly.
- Forking is prohibited; all I/O operations must be non-blocking.
- Only one `poll()` (or equivalent) is used to handle all operations
  (read, write, listen, etc.).
