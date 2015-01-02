#include "network_internals.h"
#include <stdio.h>

int main(void) {
  connection_t *client = create_client("127.0.0.1");
  rrecv_loop(client);
}

