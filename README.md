# Dope HTTP Server (WIP)

## Motivation

## Development notes

### Making a skeleton server

1. Setting up the `addrinfo` struct for hints

```c
struct addrinfo hints;
struct addrinfo *res;
memset(&hints, 0, sizeof(hints));
```

`hints` will be passed into `getaddrinfo()`. `getaddrinfo()` will return network address results based on preferences defined in `hints`. These results will be used to setup the socket.

2. `getaddrinfo()` for network results

This will return a pointer to a linked list of `addrinfo` structures with all the data you'd need to set up the socket. Linked list because one hostname might resolve to more than one address (eg. IPv4 and IPv6 addresses).

```c
#define PORT 3490

hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flag = AI_PASSIVE;

getaddrinfo(NULL, PORT, &hints, &res);
```

NOTE: `hints` only stores network preferences, the results are stored in `res`.

3. Create the socket with `socket()`

```c
int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
```

`socket()` will return a socket file descriptor (UID for the created socket) which you'll use to perform actions on the socket.

> Just like you use `read()` and `write()` to read/write into a file using its file descriptor. You'll use `send()` and `recv()` on a socket. The OS abstracts a network connection with sockets. 

4. Bind the socket with `bind()`
