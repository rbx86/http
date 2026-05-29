#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "3490"
#define BACKLOGS 10
#define BUFF_SIZE 1024

void handle_client(int client_fd, struct sockaddr_storage *client_addr) {

  char buf[BUFF_SIZE];

  if (client_addr->ss_family == AF_INET) {
    char ip4[INET_ADDRSTRLEN];
    struct sockaddr_in *client_in = (struct sockaddr_in *)client_addr;
    inet_ntop(AF_INET, &(client_in->sin_addr), ip4, INET_ADDRSTRLEN);
    printf("Client IPv4 Address: %s\n", ip4);
  } else {
    char ip6[INET6_ADDRSTRLEN];
    struct sockaddr_in6 *client_in6 = (struct sockaddr_in6 *)client_addr;
    inet_ntop(AF_INET6, &(client_in6->sin6_addr), ip6, INET6_ADDRSTRLEN);
    printf("Client IPv6 Address: %s\n", ip6);
  }

  ssize_t bytes_recvd = recv(client_fd, buf, BUFF_SIZE - 1, 0);
  if (bytes_recvd == -1) {
    perror("server: recv");
    goto end_conn;
  }
  buf[bytes_recvd] = '\0';
  printf("[/] Recieved Buffer:\n%s\n", buf);

  char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>hello, "
                "world</h1>\n";
  size_t msg_size = strlen(msg);

  ssize_t bytes_sent = send(client_fd, msg, msg_size, 0);
  if (bytes_sent == -1) {
    perror("server: send");
    goto end_conn;
  }
  
  printf("[/] Message Sent Successfully\n");
    
  end_conn:
    printf("[*] Terminating Connection...\n");
    close(client_fd);
}

int main() {

  struct addrinfo hints, *res;
  struct sockaddr_storage client_addr;
  int sockfd, client_fd;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int gai = getaddrinfo(NULL, PORT, &hints, &res);
  if (gai != 0) {
    fprintf(stderr, "%s\n", gai_strerror(gai));
    return 1;
  }
  printf("[*] Get Address Info Successful\n");

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sockfd == -1) {
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

  while (1) {
    socklen_t addr_len = sizeof(client_addr);
    client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);

    if (client_fd == -1) {
      perror("server: accept");
      continue;
    }

    handle_client(client_fd, &client_addr);
  }
  
  close(sockfd);
  return 0;
}
