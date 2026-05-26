#ifndef SERVER_H
#define SERVER_H

int  initServer(const char *port);
void runServer(int listenfd);

#endif