#include "worker.h"
#include "http_request.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

const int HEADER_LEN = 1000;
const int REQUEST_LEN = 1000;

void handleRequest(Worker_t *worker) {
  int result_len =
      recv(worker->client, worker->requestBuffer, worker->requestSize - 1, 0);
  worker->requestBuffer[result_len] = '\0'; // set end of buffer.

  if (result_len < 0) {
  }
  printf("Worker:%d: Request:\n%s\n", worker->id, worker->requestBuffer);

  char *filename = getFileName(worker->requestBuffer);
  printf("Filename: %s\n", filename);
  char *content = getFile(filename);
  if (content == NULL) {
    perror("Error looking for file");
    sendError(&worker->client);
  } else {
    size_t length = sizeof(content);
    sendFile(&worker->client, content);
  }
  close(worker->client);
}

Worker_t *create_worker(int id) {

  Worker_t *worker = (Worker_t *)malloc(sizeof(Worker_t));
  if (worker == NULL) {
    return NULL;
  }
  worker->id = id;
  worker->state = READY;

  char *request = (char *)malloc(REQUEST_LEN);
  if (request == NULL) {
    return NULL;
  }
  worker->requestBuffer = request;
  worker->requestSize = REQUEST_LEN;

  char *header = (char *)malloc(HEADER_LEN);
  if (header == NULL) {
    return NULL;
  }
  worker->headerBuffer = header;
  worker->headerSize = HEADER_LEN;

  return worker;
}

/**
 * Starts the worker.
 **/
void *start_worker(void *args) {
  Worker_t *worker = (Worker_t *)args;

  // executes code
  while (1) {
    printf("Worker %d on wait...\n", worker->id);
    while (worker->state != 0) {
      // wait for a request
    }
    printf("New request for worker %d.\n", worker->id);
    handleRequest(worker);

    printf("Worker %d finished working, going back to idle.\n", worker->id);
    worker->state = 1; // set worker free.
  }
}

void destroyWorker(Worker_t worker) {
  // TODO destroy.

  free(worker.requestBuffer);
  free(worker.headerBuffer);
}
