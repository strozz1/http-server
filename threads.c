#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_endian.h>
#include <sys/_pthread/_pthread_t.h>
#include <sys/_types/_in_addr_t.h>
#include <sys/_types/_size_t.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>
#include <unistd.h>

struct ClientThread {
  int thread;
  struct sockaddr_in address;
};
char *getFile(char file[]) {
  FILE *f = fopen(file, "r");
  if (f == NULL) {
    printf("File not found: %s\n", file);
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  printf("File size: %ld bytes.\n", size);
  fseek(f, 0, SEEK_SET);

  char *content = (char *)malloc(size);
  fread(content, size, 1, f);
  content[size] = '\0';

  fclose(f);
  return content;
}

void sendError(int *socket) {

  char *header = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n";
  int i = send(*socket, header, strlen(header), 0);
  printf("error: %s\n", header);
}

void sendFile(int *socket, char *buffer) {
  char header[100];
  snprintf(header, sizeof(header),
           "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: "
           "%zu\r\n\r\n",
           strlen(buffer));
  char response[(strlen(buffer) + strlen(header)) + 1];

  snprintf(response, sizeof(response), "%s%s", header, buffer);
  response[sizeof(response)] = '\0';
  printf("Sending respons");
  send(*socket, response, sizeof(response), 0);
}

char *getFileName(char *buffer) {
  char *ptr = strchr(buffer, '/');
  char *filename = (char *)malloc(100);
  ptr += 1;
  char *pos = strchr(ptr, ' ');
  size_t length = pos - ptr;
  strncpy(filename, ptr, length);

  filename[length] = '\0';
  return filename;
}

void *routine(void *args) {
  struct ClientThread *clientThread = (struct ClientThread *)args;

  int client = clientThread->thread;
  printf("Created thread for client connection: %d\n", client);

  int MAX_SIZE = 520;
  char *buffer = (char *)malloc(520);
  char *ptr = buffer;
  int res;

  res = recv(client, ptr, MAX_SIZE - 1, 0);

  if (res <= 0)
    return NULL;

  buffer[res] = '\0';
  char *filename = getFileName(buffer);
  printf("Filename: %s\n", filename);
  char *content = getFile(filename);
  if (content == NULL) {
    perror("Error looking for file");
    sendError(&client);
  } else {
    size_t length = sizeof(content);
    sendFile(&client, content);
  }

  printf("Closing client socket: %d.\n", client);
  free(content);
  close(client);
  return NULL;
}
int main(int argc, char *argv[]) {
  pthread_t thread1;

  int server_fd, client_sock;

  int running = 1;

  struct sockaddr_in address, client_addr;
  int address_len = sizeof(address);

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(2000);

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
    printf("Waiting for connections...\n");
    int client = accept(server_fd, (struct sockaddr *)&client_addr,
                        (socklen_t *)&client_addr);

    struct ClientThread clientThread;
    clientThread.thread = client;
    clientThread.address = client_addr;
    pthread_create(&thread1, NULL, &routine, (void *)&clientThread);
  }

  int i = pthread_join(thread1, NULL);
  printf("Result of thread: %d\n", i);
  close(server_fd);
  return EXIT_SUCCESS;
}
