
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_endian.h>
#include <sys/_types/_in_addr_t.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>

#define PORT 2001

int main(int argc, char *argv[]) {
  int server_fd, client_sock;

  int running = 1;

  struct sockaddr_in address;
  int address_len = sizeof(address);

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(PORT);

  int result;

  server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_fd == -1) {
    perror("Error creating the socket.\n");
    exit(EXIT_FAILURE);
  }
  printf("Socket created successfully\n");

  result = bind(server_fd, (struct sockaddr *)&address, address_len);
  if (result < 0) {
    perror("Error binding the socket.\n");
    exit(EXIT_FAILURE);
  }
  printf("Socket connection binded correctly.\n");

  result = listen(server_fd, 0);
  if (result < 0) {
    perror("Error putting socket on listen\n");
    exit(EXIT_FAILURE);
  }
  printf("Socket listening\n");

  while (running) {
    printf("Waiting for conenctions...\n");

    client_sock = accept(server_fd, (struct sockaddr *)&address,
                         (socklen_t *)&address_len);

    if (client_sock < 0) {
      perror("Error creating a client connection\n");
    }
    printf("CLient socket created. %d \n", client_sock);

    int MAX_SIZE = 520;
    int packet = 10;
    int current = 0;
    char buffer[MAX_SIZE];
    char *ptr = buffer;
    int result;

    while ((result = recv(client_sock, ptr, packet, 0)) > 0) {
      current += result;
      printf("Data received: %d, current used: %d\n", result, current);

      if (result < packet) { // if result is less than the packet max size
        buffer[current + 1] = '\0';
        printf("Data: %s\n", buffer);
      } else if (current >=
                 MAX_SIZE - 1) { // if current overflows buffer or equals
        buffer[MAX_SIZE - 1] = '\0';
        printf("Data: %s\n", buffer);
        printf("Buffer is full,%d reading again.\n", strlen(buffer));
        current = 0;
        memset(&buffer, 0, strlen(buffer));
        ptr = buffer;

        printf("buffer after clean: %s\n", buffer);
      }

      ptr += result;
    }
    printf("Data out: %s\n", buffer);

    //    printf("Echoing: %s\n", echoBuffer);

    //    char *buffer = "Esto es una prueba lololo.";
    //    int buffer_len = strlen(buffer);

    //    result = send(client_sock, echoBuffer, 100 - size, 0);

    //    if (result < 0) {
    //      perror("Error sending the data to the client.\n");
    //    }
  }

  return 0;
}
