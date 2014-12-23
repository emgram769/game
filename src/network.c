#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <network.h>

net_data_t *unmarshal(char *buf) {
  net_data_t *output = malloc(sizeof(net_data_t));

  char *data = calloc(MAX_MESSAGE_LEN, sizeof(char));
  char type[MAX_TYPE_LEN];

  /* nick!pass:command!arg */
  int res = sscanf(buf, "%[A-Za-z0-9]!%[A-Za-z0-9]:%[A-Z]!%[^\t\n]",
                   output->nick, output->password, type, data);

  if (res < 4) {
    goto cleanup;
  }

  if (!strncmp(type, "MSG", 3)) {
    output->type = MSG;
    output->data.message = data;
    return output;
  } else if (!strncmp(type, "LOC", 3)) {
    output->type = LOC;
    res = sscanf(data, "(%d,%d)",
      &(output->data.position[0]), &(output->data.position[1]));
    
    free(data);

    if (res < 2) {
      free(output);
    }

    return output;
  } else {
    goto cleanup;
  }

cleanup:
  free(output);
  free(data);
  return NULL;

}

char *marshal(net_data_t *net_data) {
  char *buf = calloc(MAX_NET_DATA_LEN, sizeof(char));
  int len, pos = 0;

  /* Get the nick name. */
  len = strnlen(net_data->nick, MAX_NICK_LEN);
  strncpy(buf, net_data->nick, len);
  pos += len;

  buf[pos] = '!';
  pos++;

  /* Get the password. */
  len = strnlen(net_data->password, MAX_PASSWORD_LEN);
  strncpy(buf + pos, net_data->password, len);
  pos += len;

  buf[pos] = ':';
  pos++;

  /* Get the command. */
  switch(net_data->type) {
    case LOC:
      strncpy(buf + pos, "LOC", 3);
      break;
    case MSG:
      strncpy(buf + pos, "MSG", 3);
      break;
    case JOIN:
      strncpy(buf + pos, "JOIN", 4);
      break;
    default:
      goto cleanup;
      break;
  }
  pos += 3;

  buf[pos] = '!';
  pos++;

  /* Get args. */
  switch(net_data->type) {
    case LOC:
      sprintf(buf + pos, "(%d,%d)",
              net_data->data.position[0], net_data->data.position[1]);
      break;
    case MSG:
      len = strnlen(net_data->data.message, MAX_MESSAGE_LEN);
      strncpy(buf + pos, net_data->data.message, len);
      break;
    case JOIN:
      len = strnlen(net_data->data.message, MAX_MESSAGE_LEN);
      strncpy(buf + pos, net_data->data.message, len);
      break;
    default:
      goto cleanup;
      break;
  }

  return buf;

cleanup:
  free(buf);
  return NULL;
}

