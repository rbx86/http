#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BACKLOGS 10

int main() {

  struct addrinfo hints, *res;
  struct sockaddr_storage client_addr;
  int sockfd, client_fd;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int status =  getaddrinfo(NULL, "3490", &hints, &res);
  if (status != 0) {
    fprintf(stderr, "%s\n", gai_strerror(status));
    return 1;
  }
  printf("[*] Get Address Info Successful\n");

  sockfd = socket(
    res->ai_family,
    res->ai_socktype,
    res->ai_protocol
  );
  if (sockfd == -1){
    perror("server: socket");
    freeaddrinfo(res);
    return 1;
  }
  printf("[*] Socket Created\n");

  if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("server: bind");
    freeaddrinfo(res);
    close(sockfd);
    return 1;
  }
  printf("[*] Bind Successful\n");
  
  freeaddrinfo(res);

  if (listen(sockfd, BACKLOGS) == -1) {
    perror("server: listen");
    close(sockfd);
    return 1;
  }
  printf("[*] Listening on port 3490\n");

  socklen_t addr_len = sizeof(client_addr);
  client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);

  // client info
  if (client_addr.ss_family == AF_INET) {
    char ip4[INET_ADDRSTRLEN];
    struct sockaddr_in *client_in = (struct sockaddr_in *)&client_addr;
    inet_ntop(AF_INET, &(client_in->sin_addr), ip4, INET_ADDRSTRLEN);
    printf("Client IPv4 Address: %s\n", ip4);
  } else {
    char ip6[INET6_ADDRSTRLEN];
    struct sockaddr_in6 *client_in6 = (struct sockaddr_in6 *)&client_addr;
    inet_ntop(AF_INET6, &(client_in6->sin6_addr), ip6, INET6_ADDRSTRLEN);
    printf("Client IPv6 Address: %s\n", ip6);
  }
  
  return 0;
}
