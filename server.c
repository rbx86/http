#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>

#define BACKLOGS 10

int main() {

  struct addrinfo hints, *res;
  struct sockaddr_storage client_addr;
  int sockfd, client_fd;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP socket
  hints.ai_flags = AI_PASSIVE;

  int status =  getaddrinfo(NULL, "3490", &hints, &res);
  if (status != 0) {
    fprintf(stderr, "%s\n", gai_strerror(status));
    exit(1);
  }
  printf("[*] getaddrinfo() Successful\n");

  sockfd = socket(
    res->ai_family,
    res->ai_socktype,
    res->ai_protocol
  );
  if (sockfd == -1){
    perror("socket");
    exit(1);
  }

  if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("bind()");
    exit(1);
  }
  printf("[*] Bind Successful\n");
  
  freeaddrinfo(res);

  if (listen(sockfd, BACKLOGS) == -1) {
    perror("Listen failed");
    // close(sockfd);
    exit(1);
  }
  printf("[*] Listening on port 3490\n");

  socklen_t addr_len = sizeof(client_addr);
  client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);

  close(sockfd);
  return 0;
}
