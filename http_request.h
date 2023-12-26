#ifndef HTTP_REQUEST_
#define HTTP_REQUEST_

char *getFile(char *file);

char *getFileName(char *buffer);

void sendError(int *client);
void sendFile(int *client, char *file);

#endif // !HTTP_REQUEST_
