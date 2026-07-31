#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

// Main Request types
#define REQUEST 1
#define RESPONSE 2
#define UPGRADE 3
#define EXIT 4

// Subtypes for REQUEST
#define STATUS 1
#define DATA 2
#define AUTHENTICATE 3

//Subtypes for RESPONSE
#define READY 1
#define BUSY 2
#define BLOCKED 3
#define RESPDATA 4

// Protocols
#define CHAT 1
#define AUDIO 2
#define FILE 3

struct request_t {
    uint8_t base_header; // 1 Byte
    unsigned short int payload_size; // 2 Bytes
    char peer_id[8]; // 8 Bytes
    void* payload;
};



int main(void) {
  int server = socket(AF_INET6, SOCK_STREAM, 0);
  if (server < 0) {
    perror("Socket");
    close(server);
    return EXIT_FAILURE;
  }

  struct sockaddr_in6 address;
  memset(&address, 0, sizeof(address));
  address.sin6_family = AF_INET6;
  address.sin6_port = htons(6060);
  address.sin6_addr = in6addr_any;

  int yes = 1;

  setsockopt(server, IPPROTO_IPV6, IPV6_V6ONLY, &yes, sizeof(yes));

  if (bind(server, (struct sockaddr *) &address, sizeof(address)) < 0) {
    perror("bind");
    close(server);
    return EXIT_FAILURE;
  }

  if (listen(server, 2) < 0) {
    perror("listen");
    close(server);
    return EXIT_FAILURE;
  }
  int client = -1;
  while (1) {
    client = accept(server, NULL, NULL);
    int running = 1;
  }

  close(client);
  close(server);

  return 0;
}
