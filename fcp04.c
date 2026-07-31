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
#define ERROR 5

// Protocols
#define CHAT 1
#define AUDIO 2
#define FILE 3

struct extended_payload_t {
char peer_id[32]; // 32 Bytes
uint8_t protocol; // 1 Byte
void* data;
};

struct request_t {
    uint8_t base_header; // 1 Byte
    uint32_t payload_size; // 4 Bytes
    void* payload;
};


char* good_input(const char* prompt) {
  char* input = NULL;
  size_t len = 0;
  printf("%s", prompt);
  ssize_t read = getline(&input, &len, stdin);
  if (read == -1) {
    perror("getline");
    free(input);
    return NULL;
  }
  // Remove newline character if present
  if (input[read - 1] == '\n') {
    input[read - 1] = '\0';
  }
  return input;
}


uint8_t make_header(int request_type, int subtype, bool extended_header, bool authorized){
  uint8_t new_header = 0;
  new_header |= request_type;
  new_header |= (subtype << 3);
  if (extended_header) new_header |= (0x01 << 6);
  if (authorized) new_header |= (0x01 << 7);
  return new_header;
}
int recv_all(int socket, void *buffer, size_t length) {
  size_t total = 0;

  while (total < length) {
    ssize_t received = recv(
      socket,
      (char *)buffer + total,
      length - total,
      0
    );

    if (received <= 0) {
      return -1;
    }

    total += received;
  }

  return 0;
}
int send_all(int socket, const void *buffer, size_t length) {
  size_t total = 0;

  while (total < length) {
    ssize_t sent = send(
      socket,
      (const char *)buffer + total,
      length - total,
      0
    );

    if (sent <= 0) {
      return -1;
    }
    total += sent;
  }
  return 0;
}

char* get_request_content(int client, int* running) {
  struct request_t request;
  if(recv_all(client, &request.base_header, sizeof(request.base_header)) < 0) {
    perror("recv");
    *running = 0;
    return NULL;
  }

  if(recv_all(client, &request.payload_size, sizeof(request.payload_size)) < 0) {
    perror("recv");
    *running = 0;
    return NULL;
  }
  request.payload = malloc(request.payload_size);
  if (!request.payload) {
    perror("malloc");
    *running = 0;
    return NULL;
  }

  if(recv_all(client, request.payload, request.payload_size) < 0) {
    perror("recv");
    free(request.payload);
    *running = 0;
    return NULL;
  }

  // Process the request here
  // For demonstration purposes, we just print the header and payload size
  printf("Received request: base_header=%u, payload_size=%u\n", request.base_header, request.payload_size);
  if(!(request.base_header & 0x02)) {
    printf("Message is: %s\n", (char*)request.payload);
  }
  free(request.payload);
  return NULL;
}

int send_message(int client, int type, int subtype, char* message) {
  if (message == NULL || message[0] == '\0') {
    return -1;
  }

  uint8_t header = make_header(type, subtype, false, false);
  uint32_t payload_size = strlen(message) + 1;

  if (send(client, &header, sizeof(header), 0) <= 0) {
    perror("send header");
    return -1;
  }

  if (send(client, &payload_size, sizeof(payload_size), 0) <= 0) {
    perror("send payload size");
    return -1;
  }

  if (send(client, message, payload_size, 0) <= 0) {
    perror("send payload");
    return -1;
  }

  return 0;
}


int main(void) {
  int server = socket(AF_INET, SOCK_STREAM, 0);
  if (server < 0) {
    perror("Socket");
    close(server);
    return EXIT_FAILURE;
  }

  struct sockaddr_in address;
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(6060);
  address.sin_addr.s_addr = INADDR_ANY;

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
    char* response_message = good_input("Enter a Request message: ");
    send_message(client, REQUEST, READY, response_message);
    free(response_message);
    while(running) {
      get_request_content(client, &running);
      response_message = good_input("Enter response message: ");
      send_message(client, RESPONSE, READY, response_message);
      free(response_message);
    }
  }

  close(client);
  close(server);

  return 0;
}
