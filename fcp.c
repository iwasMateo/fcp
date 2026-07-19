#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
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

//Subtypes for RESPONSE
#define READY 2
#define BUSY 3
#define BLOCKED 4

// Protocols
#define CHAT 1
#define AUDIO 2
#define FILE 3

// Protocol versions (major and minor) are just any 8 bit number

struct request {
  uint8_t request_type;
  uint8_t subtype;
  uint8_t userlen;
  char* name;
  uint8_t protocol;
  uint8_t major;
  uint8_t minor;
  uint8_t len;
  char* payload;
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
    struct request current_req = {0};
    int request_size;
    while (running){
      if (client < 0) {
        perror("Client");
        close(server);
        close(client);
        return EXIT_FAILURE;
      }

      uint8_t buffer[1024];
      ssize_t n = recv(client, buffer, sizeof(buffer)-1, 0);

      if(n > 0) {
        buffer[n] = '\0';
        printf("%s\n", buffer);
      }

      int request_constructed = 0;
      
      if (!request_constructed) {
        current_req.request_type = buffer[0];
        current_req.subtype = buffer[1];
        current_req.userlen = buffer[2];
        char* name = malloc(current_req.userlen+1);
        memcpy(name, &buffer[3], current_req.userlen);
        name[current_req.userlen] = '\0';
        current_req.name = name;
        current_req.protocol = buffer[3 + current_req.userlen];
        current_req.major = buffer[3 + current_req.userlen + 1];
        current_req.minor = buffer[3 + current_req.userlen + 2];
        current_req.len = buffer[3 + current_req.userlen + 3];
        char* payload = malloc(current_req.len+1);
        if (payload == NULL) return 1;
        int payload_offset = 3 + current_req.userlen + 4;
        memcpy(payload, &buffer[payload_offset], current_req.len);
        payload[current_req.len] = '\0';
        current_req.payload = payload;
      }
      
      // Hier gucken wie viele Packets noch fehlen bevor man es druckt

      switch (current_req.request_type) {
        case EXIT:
          running = 0;
          break;

        case RESPONSE:
          if (current_req.subtype == READY) {
            char message[] = "Hello World";

            if (send(client, message, strlen(message), 0) == -1) {
              perror("send");
              close(server);
              close(client);
              return EXIT_FAILURE;
            }
          }
          break;

        case REQUEST:
          printf("%u %u %u %s %u %u %u %u %s", (unsigned) current_req.request_type, (unsigned) current_req.subtype, (unsigned) current_req.userlen, current_req.name, (unsigned) current_req.protocol, (unsigned) current_req.major, (unsigned) current_req.minor, (unsigned) current_req.len, current_req.payload);
          free(current_req.name);
          free(current_req.payload);
          break;
      }
    }
  }

  close(client);
  close(server);

  return 0;
}
