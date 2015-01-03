#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#include <sys/socket.h>

//#include <network.h>
#define PORT 55555

#include "network_internals.h"
#include "../util/inc/trace.h"

connection_t *create_client(char *server_name, char *server_port) {

  /* Get host name. */

	struct addrinfo protocol_spec;
	struct addrinfo *possible_addrs, *curr_addr;
	memset(&protocol_spec, 0, sizeof(struct addrinfo));
 	protocol_spec.ai_family = AF_INET;    	/* Allow IPv4. TODO IPv6 */
  protocol_spec.ai_socktype = SOCK_DGRAM; /* Datagram socket */
  protocol_spec.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
  protocol_spec.ai_protocol = 0;          /* Any protocol */
  protocol_spec.ai_canonname = NULL;
  protocol_spec.ai_addr = NULL;
  protocol_spec.ai_next = NULL;

	int err;
	if ((err = getaddrinfo(server_name, server_port, &protocol_spec, &possible_addrs))) {
    TRACE(NETWORK_TRACE, TRACE_PRINT("Unable to obtain host's address.  Error: %s", gai_strerror(err)));
    return NULL;
	}

  /* Create a connection object. */
  connection_t *con = calloc(1, sizeof(connection_t));

  /* Create a socket by iterating through returned addresses. */
	for (curr_addr = possible_addrs; curr_addr != NULL; curr_addr = curr_addr->ai_next) {
		con->socket = socket(curr_addr->ai_family, curr_addr->ai_socktype, curr_addr->ai_protocol);
		if (con->socket < 0) {
			continue;
		}

		/* TODO: ping server for connectivity (possibly using bind) */
  	//client_bind(con);
		break;
	}

	/* We were unable to find an address given the host name. */
  if (curr_addr == NULL) {
    TRACE(NETWORK_TRACE, TRACE_PRINT("Unable to create socket."));
    goto cleanup;
  }

  /* Create a sockaddr. */
  con->addr.sin_family = curr_addr->ai_family;
  con->addr.sin_port = htons(PORT);
	switch (curr_addr->ai_family) {
		case AF_INET:
  		memcpy((void *)&con->addr.sin_addr, &((struct sockaddr_in *)curr_addr->ai_addr)->sin_addr, curr_addr->ai_addrlen);
			break;
		default:
			TRACE(NETWORK_TRACE, TRACE_PRINT("Unable to use ai_family returned."));
			goto cleanup;
	}

	freeaddrinfo(possible_addrs);

  con->addr_len = sizeof(con->addr);

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

