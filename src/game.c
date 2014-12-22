#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>

#include <server.h>
#include <client.h>
#include <marshal.h>
#include <display.h>
#include <game.h>

static player_t player = {
  0,
  0
};
static connection_t *con;
//static pthread_mutex_t con_mutex = PTHREAD_MUTEX_INITIALIZER;

void print_usage(void) {
  printf("Usage: ./game server\n");
}

void draw_player(void) {
  draw_char('X', player.x, player.y);
}

void clear_player(void) {
  draw_char(' ', player.x, player.y);
}

void draw_remote(char c, int x, int y) {
  draw_char(c, x, y);
}

void move_player(int x, int y) {
  char buf[4];
  clear_player();
  sprintf(buf, "%c%c%c", ' ', (char)player.x, (char)player.y);
  con->send(buf, 4);

  player.x += x;
  player.y += y;
  draw_player();
  sprintf(buf, "%c%c%c", 'X', (char)player.x, (char)player.y);
  con->send(buf, 4);
}

void process_keyboard(void) {
  int c = get_char();
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
  char buf[3];
  while (1) {
    if (con->recv(buf, 3) < 3) {
      continue;
    }
    draw_remote(buf[0], buf[1], buf[2]);
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

  if(init_display()) {
    return 1;
  }

  con = client(argv[1]);

  pthread_t con_thread;

  if(pthread_create(&con_thread, NULL, process_connection, NULL)) {
    printf("Error creating connection thread.\n");
    return 1;
  }

  while (1) {
    process_keyboard();
  }

  return 0;
}

