#ifndef _CLIENT_H
#define _CLIENT_H

typedef struct {
  int (*send)(char *msg, int len);
  int (*recv)(char *buf, int len);
} connection_t;

connection_t *client(char *server);

#endif /* _CLIENT_H */

