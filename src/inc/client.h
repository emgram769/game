#ifndef _CLIENT_H
#define _CLIENT_H

#include <network.h>

typedef struct {
  int (*send)(net_data_t *net_data);
  net_data_t *(*recv)(void);
} connection_t;

connection_t *client(char *server);
connection_t *con;

#endif /* _CLIENT_H */

