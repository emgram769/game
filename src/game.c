#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>

#include <server.h>
#include <client.h>

#include <display.h>
#include <game.h>

static player_t player = {
  0,
  0
};

player_t players[256];

static connection_t *con;
static display_t *main_window;

void print_usage(void) {
  printf("Usage: ./game server\n");
}

void draw_player(void) {
  main_window->draw_char(main_window, 'X', player.x, player.y);
}

void clear_player(void) {
  main_window->draw_char(main_window, ' ', player.x, player.y);
}

void draw_remote(char c, int x, int y) {
  main_window->draw_char(main_window, c, x, y);
}

void add_player(char *nick, int x, int y) {
  int i;
  for (i = 0; i < MAX_PLAYERS; i++) {
    if (!players[i].active) {
      strncpy(players[i].nick, nick, MAX_NICK_LEN);
      players[i].x = x;
      players[i].y = y;
      players[i].active = 1;
      draw_remote('X', x, y);
    }
  }
}

void update_player_location(char *nick, int x, int y) {
  int i, found = 0;
  for (i = 0; i < MAX_PLAYERS; i++) {
    if (players[i].active && !strncmp(nick, players[i].nick, MAX_NICK_LEN)) {
      found = 1;
      draw_remote(' ', players[i].x, players[i].y);
      players[i].x = x;
      players[i].y = y;
      draw_remote('X', x, y);
    }
  }

  if (!found) {
    add_player(nick, x, y);
  }
}

void move_player(int x, int y) {
  clear_player();

  player.x += x;
  player.y += y;
  draw_player();

  net_data_t n;
  n.type = LOC;
  n.data.position[0] = player.x;
  n.data.position[1] = player.y;
  strncpy(n.nick, "woo\0", 4);
  strncpy(n.password, "wee\0", 4);

  con->send(&n);
}

void process_keyboard(void) {
  int c = get_char();
  if (chat_open()) {
    chat_process_keyboard(c);
    return;
  }
  if (c == '\033') {
    get_char(); /* Skip [ */
    c = get_char();
  } else {
    if (c == 'c') {
      toggle_chat();
    }
    return;
  }
  switch(c) {
    case 'A':
        move_player(0, -1);
        break;
    case 'B':
        move_player(0, 1);
        break;
    case 'C':
        move_player(1, 0);
        break;
    case 'D':
        move_player(-1, 0);
        break;
  }
}

void *process_connection(void *ptr) {
  net_data_t *n;
  while (1) {
    if (!(n = con->recv())) {
      continue;
    }
    update_player_location(n->nick, n->data.position[0], n->data.position[1]);
    free(n);
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    print_usage();
    exit(1);
  }
  
  if (strncmp(argv[1], "-s", 2) == 0) {
    return server();
  }

  if((main_window = init_display()) == NULL) {
    return 1;
  }

  con = client(argv[1]);

  pthread_t con_thread;

  if(pthread_create(&con_thread, NULL, process_connection, NULL)) {
    printf("Error creating connection thread.\n");
    return 1;
  }

  init_chat();
  draw_player();

  while (1) {
    process_keyboard();
  }

  return 0;
}

