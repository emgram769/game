#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <signal.h>

#include <network.h>
#include <server.h>

/* Socket needs to be closed when we kill the program. */
int fd;
void kill_handler(int sig) {
  printf("\nCleaning up server...\n");
  close(fd);
  exit(0);
}

struct sockaddr_in client_addrs[MAX_CONNECTIONS];
int num_connections = 0;

int server(void) {
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  socklen_t addr_len = sizeof(client_addr);

  int recv_len;
  unsigned char buf[BUFSIZE];

  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("Failed to initialize the socket.\n");
    return 1;
  }

  signal(SIGINT, kill_handler);

  memset((char *)&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(PORT);

  if (bind(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    printf("Failed to bind the socket.\n");
    return 1;
  }

  while (1) {
    recv_len = recvfrom(fd, buf, BUFSIZE, 0, 
                       (struct sockaddr *)&client_addr, &addr_len);
    if (recv_len > 0) {
      buf[recv_len] = 0;
      printf("[%u] %s\n", client_addr.sin_addr.s_addr, buf);
      
      int i;
      int already_in = 0;
      for (i = 0; i < num_connections; i++) {
        if (client_addr.sin_addr.s_addr == client_addrs[i].sin_addr.s_addr) {
          already_in = 1;
          continue;
        }
        if (!sendto(fd, buf, recv_len, 0,
            (struct sockaddr *)&(client_addrs[i]),
            sizeof(client_addrs[i]))) {
          printf("Error broadcasting to %u.\n",
                 client_addrs[i].sin_addr.s_addr);
        };
      }

      if (!already_in) {
        client_addrs[num_connections++] = client_addr;
      }
    }
  }

  return 0;
}

