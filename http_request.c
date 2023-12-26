#include "http_request.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
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

char *getFile(char *file) {
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
  printf("Sending response\n");
  send(*socket, response, sizeof(response), 0);
}
