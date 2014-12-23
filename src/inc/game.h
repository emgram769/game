#ifndef _GAME_H
#define _GAME_H

#include <chat.h>

typedef struct {
  int x;
  int y;
  char nick[MAX_NICK_LEN];
  int active;
} player_t;

#define MAX_PLAYERS 256

#endif /* _GAME_H */

