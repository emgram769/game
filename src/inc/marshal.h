#ifndef _MARSHAL_H
#define _MARSHAL_H

#define MAX_NET_DATA_LEN  BUFSIZE
#define MAX_TYPE_LEN      30
#define MAX_NICK_LEN      30
#define MAX_PASSWORD_LEN  40
#define MAX_MESSAGE_LEN   (MAX_NET_DATA_LEN - \
  (MAX_TYPE_LEN + MAX_NICK_LEN + MAX_PASSWORD_LEN))

typedef enum {
  LOC,
  MSG,
  JOIN
} net_type;

typedef struct {
  
  net_type type;
  
  union {
    int position[2];
    char *message;
  } data;

  char nick[MAX_NICK_LEN];
  char password[MAX_PASSWORD_LEN];

} net_data_t;

net_data_t *unmarshal(char *buf);
char *marshal(net_data_t *net_data);

#endif /* _MARSHAL_H */
