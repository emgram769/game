#include "network_internals.h"
#include <stdio.h>

int main(void) {
  printf("sizeof packet_header_t %lu\n", sizeof(packet_header_t));
  connection_t *client = create_client("127.0.0.1");
  rrecv_loop(client);
}

