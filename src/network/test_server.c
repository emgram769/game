#include "network_internals.h"
#include <stdio.h>

int main(void) {
  server_t *srv = create_server(1);
  srv->clients[0].addr_len = sizeof(srv->clients[0].addr);
  char buf[512];
  int recv_len;
  unsigned long i;
  while (1) {
    recv_len = recvfrom(srv->socket, buf, 512, 0, 
                       (struct sockaddr *)&(srv->clients[0].addr), &(srv->clients[0].addr_len));
    printf("[%lu] %d\n", i++, recv_len);
    sendto(srv->socket, buf, recv_len, 0, (struct sockaddr *)&(srv->clients[0].addr), srv->clients[0].addr_len);
  }
}

