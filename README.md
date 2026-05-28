# Dope HTTP Server (WIP)

## Motivation

What better way to learn a programming language than building a HTTP server :D. Too bad I LLM'd my way through my first 3 years of uni, so here's my attempt at actually coding (and learning how to code) without AI. Inspo from LowLevelTV on youtube  and this [reel](https://youtube.com/shorts/5JN7SZ6NETQ?si=BdNJZoFZ6dBhzrzb).

## Development notes

### Making a skeleton server

1. Setting up the `addrinfo` struct for hints

```c
struct addrinfo hints;
struct addrinfo *res;
memset(&hints, 0, sizeof(hints));
```

`hints` will be passed into `getaddrinfo()`. `getaddrinfo()` will return network address results based on preferences defined in `hints`. These results will be used to setup the socket.

```c
struct addrinfo {
    int ai_flags;
    int ai_family;              // AF_INET, AF_INET6, AF_UNSPEC
    int ai_socktype;            // SOCK_STREAM, SOCK_DGRAM
    int ai_protocol;            // 0 for "any", tcp, udp
    size_t ai_addrlen;          // size of ai_addr in bytes
    struct sockaddr *ai_addr;   // struct sockaddr_in or _in6
    char *ai_canonname;

    struct addrinfo *ai_next;   // linked list, next node
};
```
---

#### Note on `sockaddr`

`struct sockaddr` holds socket address information for many types of sockets.

```c
struct sockaddr {
    unsigned short sa_family;   // address family: AF_INET(IPv4), AF_INET6(IPv6)
    char sa_data[14];           // 14b protocol address (destination IP address + port number for socket)
};
```
However, no one actually uses `sockaddr`. It's more of a generic base type which `sockaddr_in` (IPv4) and `sockaddr_in6` (IPv6) builts upon. These have the fields such as `.sin_addr`/`.sin6_addr` and `.sin_port`/`.sin6_port` for the necessary address and port information. For example,

```c
struct sockaddr_in {
    short int sin_family;           // AF_INET (IPv4)
    unsigned short int sin_port;    // port number
    struct in_addr sin_addr;        // IPv4 address
    unsigned char sin_zero[8];      // just padding
};
```

While `connect()` expects `struct sockaddr *`, we can pass `struct sockaddr_in` or `sockaddr_in6` and typecast it to a pointer to `struct sock_addr`.

> **NOTE**: The last field in `sockaddr_in` is to pad the struct so it matches the length of `struct sockaddr`. `sin_port` should be set to Network Byte Order via `htons()`

```c
struct in_addr {
    uint32_t s_addr; // IP address (4 bytes->32 bits)
};
```
This is just the IP address. Lastly there's `sockaddr_storage`, the other sibling of `_in` and `_in6`. This is **just large enough** to hold both IPv4 and IPv6 structures. Sometimes when you make a call, you won't know in advance if it's an IPv4 or IPv6 address that's going to fill your `sock_addr` so you use `_storage` which is slightly larger than `struct sockaddr` and type case it to the type you need.

```c
struct sockaddr_storage {
    sa_family_t ss_family; // address family: AF_INET, AF_INET6

    // padding (INGORE)
    char __ss_pad1[_SS_PAD1SIZE];
    int64_t __ss_align;
    char __ss_pad2[_SS_PAD2SIZE];
};
```

> [!tip]
> To know what type to cast `struct sockaddr_storage` to, we can check the `ss_family` field; if it's `AF_INET` (IPv4) or `AF_INET6` (IPv6). For example;

```c
if (their_addr.ss_family == AF_INET) {
    struct sockaddr_in *s = (struct sockaddr_in *)&their_addr;
    // now use s->sin_addr, s->sin_port etc
} else {
    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&their_addr;
    // now use s->sin6_addr, s->sin6_port etc
}
```

---

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

## References

- Beej's Guide to Network Programming Using Internet Sockets ([dope ass book](https://www.beej.us/guide/bgnet/])).
- The internet
