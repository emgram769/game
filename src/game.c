#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>

#include <server.h>
#include <client.h>

#include <display.h>
#include <chat.h>
#include <game.h>

static player_t player = {
  0,
  0
};
static connection_t *con;
static display_t *main_window;
//static pthread_mutex_t con_mutex = PTHREAD_MUTEX_INITIALIZER;

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

void move_player(int x, int y) {
  clear_player();
  //sprintf(buf, "%c%c%c", ' ', (char)player.x, (char)player.y);

  player.x += x;
  player.y += y;
  draw_player();
  //sprintf(buf, "%c%c%c", 'X', (char)player.x, (char)player.y);
  //con->send(buf, 4);
  net_data_t n;
  n.type = LOC;
  n.data.position[0] = player.x;
  n.data.position[1] = player.y;
  strncpy(n.nick, "woo", 3);
  strncpy(n.password, "wee", 3);
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
    draw_remote('X', n->data.position[0], n->data.position[1]);
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

