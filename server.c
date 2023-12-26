#include "worker.h"
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

int main(int argc, char *argv[]) {
  const int num_workers = 4;
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

  Worker_t *workers[num_workers];
  pthread_t threads[num_workers];
  printf("Preparing workers...\n");
  for (int i = 0; i < num_workers; i++) {
    Worker_t *worker = create_worker(i);
    workers[i] = worker;
    pthread_create(&threads[i], NULL, start_worker, (void *)worker);
  }

  sleep(2);
  printf("All workers ready.\n");
  while (running) {
    int client = accept(server_fd, (struct sockaddr *)&client_addr,
                        (socklen_t *)&client_addr);

    workers[0]->client = client;
    workers[0]->state = BUSY;
  }

  // TODO close threads
  close(server_fd);
  return EXIT_SUCCESS;
}
