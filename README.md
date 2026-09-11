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
- **Reference client**: Irssi
- **Protocol reference**: RFC 1459
- **Standard**: strict C++98 (`-Wall -Wextra -Werror -std=c++98`)

## Instructions

### Build

```sh
make        # build ircserv
make clean  # remove object files
make fclean # remove object files and the executable
make re     # fclean + all
```

### Usage

```sh
./ircserv <port> <password>
```

- `port`: the port number on which the IRC server listens for incoming connections
- `password`: the connection password required by any client trying to connect

Example:
```sh
./ircserv 6667 mypassword
```

### Connecting with Irssi

```sh
irssi -c 127.0.0.1 -p 6667 -w mypassword
```

Or from an already-running Irssi session:

```
/connect 127.0.0.1 6667 mypassword
```

Then, once connected:

```
/nick <nickname>
/join #channel
```

### Testing

```sh
nc -C 127.0.0.1 6667
```

Useful to check that partial/fragmented data is properly reassembled before
being processed (see the subject's example: sending `com`, `man`, `d\n`
separately with Ctrl+D).

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
- `PING` — keepalive / connection check (replies with `PONG`)
- `HELP` — lists available commands, or describes one command, via a small built-in "bot" (see Bonus)

## Bonus

- **Bot**: a `HelpBot` that answers the `HELP` command in-channel/in-PM with usage information for each supported command (see `Bot.hpp`/`Bot.cpp`).

## Project structure

```
.
├── Makefile
├── includes/              # headers (.hpp)
│   ├── Client.hpp
│   ├── Channel.hpp
│   ├── Server.hpp
│   ├── Parser.hpp
│   ├── Bot.hpp
│   └── Commands.hpp
├── srcs/
│   ├── main.cpp
│   ├── Client.cpp
│   ├── Channel.cpp
│   ├── Server.cpp
│   ├── Parser.cpp        # aggregates/splits raw socket data into IRC commands
│   ├── Bot.cpp            # HelpBot logic used by the HELP command
│   └── commands/          # one command per file
│       ├── Pass.cpp
│       ├── Nick.cpp
│       ├── User.cpp
│       ├── Join.cpp
│       ├── Part.cpp
│       ├── PrivMsg.cpp
│       ├── Kick.cpp
│       ├── Invite.cpp
│       ├── Topic.cpp
│       ├── Mode.cpp
│       ├── Ping.cpp
│       ├── Quit.cpp
│       └── Help.cpp
└── tests/                 # standalone test programs (not built by the main Makefile)
```

## Resources

- [RFC 1459](https://www.rfc-editor.org/rfc/rfc1459) — Internet Relay Chat Protocol (main reference used for command syntax and numeric replies)
- [Modern IRC Client Protocol (ircdocs.horse)](https://modern.ircdocs.horse/) — up-to-date, community-maintained rewrite of the IRC spec, easier to read than the RFCs
- [IRC numerics list (alien.net.au)](https://www.alien.net.au/irc/irc2numerics.html) — reference table for all numeric reply codes (ERR_*/RPL_*)
- [Irssi documentation](https://irssi.org/documentation/) — reference client used for manual testing

## General rules (from the subject)

- The server must never crash, under any circumstances (including when it
  runs out of memory), and must never quit unexpectedly.
- Forking is prohibited; all I/O operations must be non-blocking.
- Only one `poll()` (or equivalent) is used to handle all operations
  (read, write, listen, etc.).
