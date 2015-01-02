#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#include <sys/socket.h>

//#include <network.h>
#define PORT 55555

#include "network_internals.h"
#include "../util/inc/trace.h"

connection_t *create_client(char *server_name) {

  /* Get host name. */
  struct hostent *hp;
  hp = gethostbyname(server_name);
  if (!hp) {
    TRACE(NETWORK_TRACE, TRACE_PRINT("Unable to obtain host's address."));
    return NULL;
  }

  /* Create a connection object. */
  connection_t *con = calloc(1, sizeof(connection_t));

  /* Create a socket. */
  con->socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (con->socket < 0) {
    TRACE(NETWORK_TRACE, TRACE_PRINT("Unable to create socket."));
    goto cleanup;
  }

  /* Create a sockaddr. */
  con->addr.sin_family = AF_INET;
  con->addr.sin_port = htons(PORT);  
  memcpy((void *)&con->addr.sin_addr, hp->h_addr_list[0], hp->h_length);

  con->addr_len = sizeof(con->addr);
  //client_bind(con);

  return con;

cleanup:
  free(con);
  return NULL;
}

server_t *create_server(unsigned int max_connections) {

  /* Create a connection object. */
  server_t *srv = calloc(1, sizeof(server_t));
  
  /* Create a socket. */
  srv->socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (srv->socket < 0) {
    TRACE(NETWORK_TRACE, TRACE_PRINT("Unable to create socket."));
    goto cleanup;
  }

  /* Use a stack allocated sockaddr to bind the server. */
  struct sockaddr_in server_addr;
  memset((char *)&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(PORT);

  /* Bind the socket. */
  if (bind(srv->socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    TRACE(NETWORK_TRACE, TRACE_PRINT("Failed to bind the socket."));
    goto cleanup;
  }

  srv->clients = calloc(max_connections, sizeof(connection_t));
  //server_loop(srv);

  return srv;

cleanup:
  if (srv->clients) {
    free(srv->clients);
  }
  free(srv);

  return NULL;
}

int con_sendto(connection_t *con, char *buf, int len, int flags) {
  return sendto(con->socket, buf, len, flags, (struct sockaddr *)&con->addr, con->addr_len);
}
