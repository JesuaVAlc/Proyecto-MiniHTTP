/* passivesock.c - passivesock */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>

int passivesock(const char *service, const char *transport, int qlen) {
  struct addrinfo hints, *res, *p;
  int  sockfd, socktype;  /* socket descriptor and socket type  */

  memset(&hints, 0, sizeof(hints));

  /* Use protocol to choose a socket type */
  if (strcmp(transport, "udp") == 0)
    socktype = SOCK_DGRAM;
  else
    socktype = SOCK_STREAM;

  hints.ai_family = AF_UNSPEC;      // IPv4 o IPv6
  hints.ai_socktype = socktype;
  hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
  //hints.ai_protocol = 0;          /* Any protocol */
  //hints.ai_canonname = NULL;
  //hints.ai_addr = NULL;
  //hints.ai_next = NULL;

  int err = getaddrinfo(NULL, service, &hints, &res);
  if (err != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(err));
    return -1;
  }

  // Iterar sobre las direcciones posibles
  for (p = res; p != NULL; p = p->ai_next) {
    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd < 0)
        continue;
    printf ("%d, %d, %d, %d\n", p->ai_family, p->ai_socktype, p->ai_protocol, p->ai_addrlen); 
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == 0)
        break; // éxito

    close(sockfd);
  }
  freeaddrinfo(res);

  if (p == NULL) {
    fprintf(stderr, "No pudo bind\n");
    return -1;
  }
  
  if (socktype == SOCK_STREAM && listen(sockfd, qlen) < 0) {
    fprintf(stderr, "can't listen on %s port\n", service);
    return -1;
  }
  return sockfd;
}
