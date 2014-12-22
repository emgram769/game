#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <network.h>
#include <client.h>

static int fd;
static struct sockaddr_in server_addr;

int _send(char *msg, int len) {
  return sendto(fd, msg, len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
}

int _recv(char *buf, int len) {
  return recvfrom(fd, buf, len, 0, NULL, NULL);
}

connection_t *client(char *server) {
  struct hostent *hp;

  hp = gethostbyname(server);
  if (!hp) {
    printf("Unable to obtain host's address.\n");
    return NULL;
  }
  
  fd = socket(AF_INET, SOCK_DGRAM, 0);

  memset((char*)&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);  
  memcpy((void *)&server_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

  connection_t *t = malloc(sizeof(connection_t));

  t->send = _send;
  t->recv = _recv;

  return t;
}

