#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <client.h>

static int fd;
static struct sockaddr_in server_addr;

int _send(net_data_t *net_data) {
  char *msg = marshal(net_data);
  int len = strnlen(msg, MAX_NET_DATA_LEN);
  int res = sendto(fd, msg, len, 0,
                   (struct sockaddr *)&server_addr,
                   sizeof(server_addr));
  free(msg);
  return res;
}

net_data_t *_recv(void) {
  char *buf = calloc(MAX_NET_DATA_LEN, sizeof(char));
  net_data_t *n = NULL;
  if(recvfrom(fd, buf, MAX_NET_DATA_LEN, 0, NULL, NULL)) {
     n = unmarshal(buf);
  }
  free(buf);
  return n;
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

  net_data_t n;
  n.type = JOIN;
  strncpy(n.nick, "nic\0", 4);
  strncpy(n.password, "vvv\0", 4);
  n.data.message = calloc(4, sizeof(char));
  strncpy(n.data.message, "woo\0", 4);
  t->send(&n);
  free(n.data.message);

  return t;
}

