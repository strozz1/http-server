#ifndef WORKER_H_
#define WORKER_H_

typedef enum {
  BUSY = 0,
  READY = 1,
  DISABLE = 2,

} State;
/**
 * States:
 * 0: Working with a request.
 * 1: Worker free. Ready for requests.
 * 2: Worker disabled
 * */
typedef struct {
  int id;
  int state;
  int client;
  char *headerBuffer;
  int headerSize;
  char *requestBuffer;
  int requestSize;
} Worker_t;

Worker_t *create_worker(int id);
void *start_worker(void *args);
#endif // DEBUG
