#include <ctype.h>
#include <string.h>

#include <display.h>
#include <client.h>

#include <chat.h>

static int chat_box_hidden;
static char chat_buffer[CHAT_BUFSIZE + 1];
static int chat_buffer_loc;

static char chat_window_buffer[CHAT_HEIGHT - 4][MAX_MESSAGE_LEN];
static int chat_window_loc;
char clear_buffer[CHAT_WIDTH - 4];

static display_t *chat_window;

void init_chat(void) {
  chat_window = create_window(CHAT_HEIGHT, CHAT_WIDTH, MAIN_WIDTH - CHAT_WIDTH, 0);
  chat_window->draw_char(chat_window, '>', 1, CHAT_HEIGHT - 2);
  memset(clear_buffer, ' ', CHAT_WIDTH - 4);
  hide_chat();
}

void update_chat(void) {
  chat_window->get_cursor(chat_window);
  int x = chat_window->cursor.x;
  int y = chat_window->cursor.y;
  if (!chat_box_hidden) {
    int i;
    for (i = 0; i < CHAT_HEIGHT - 2; i++) {
      chat_window->draw_strn(chat_window, clear_buffer,
                             CHAT_WIDTH - 4, 2, 1 + i);
      chat_window->draw_strn(chat_window, chat_window_buffer[i],
                             CHAT_WIDTH - 4, 2, 1 + i);
    }
  }
  chat_window->set_cursor(chat_window, x, y);
}

void toggle_chat(void) {

  if (chat_box_hidden) {
    show_chat();
  } else {
    hide_chat();
  }

}

void show_chat(void) {
  chat_window->show(chat_window);
  chat_box_hidden = 0;
  update_chat();
  cursor(1);
}

void hide_chat(void) {
  chat_window->hide(chat_window);
  chat_box_hidden = 1;
  cursor(0);
}

int chat_open(void) {
  return !chat_box_hidden;
}

void chat_process_keyboard(int c) {

  if (c == 27) {
    toggle_chat();
  } else if (c == 127 || c == 8) {
    if (chat_buffer_loc > 0) {
      chat_buffer[--chat_buffer_loc] = '\0';
      chat_window->get_cursor(chat_window);
      chat_window->draw_char(chat_window, ' ',
                             chat_window->cursor.x - 1,
                             chat_window->cursor.y);
    }
  } else if (c == '\n' || c == '\r') {
    net_data_t n;
    n.type = MSG;
    n.data.message = chat_buffer;
    strncpy(n.nick, "woo\0", 4);
    strncpy(n.password, "weo\0", 4);
    con->send(&n);
    memset(chat_buffer, ' ', chat_buffer_loc);
    chat_window->draw_strn(chat_window, chat_buffer,
                           CHAT_WIDTH - 4, 2, CHAT_HEIGHT - 2);
    memset(chat_buffer, 0, chat_buffer_loc);
    chat_buffer_loc = 0;
  } else if (chat_buffer_loc < CHAT_BUFSIZE && isprint(c)) {
    chat_buffer[chat_buffer_loc++] = c;
  }

  int offset = chat_buffer_loc - (CHAT_WIDTH - 4);
  offset = offset > 0 ? offset : 0;
  chat_window->draw_strn(chat_window, chat_buffer + offset,
                         CHAT_WIDTH - 4, 2, CHAT_HEIGHT - 2);

}

void new_message(char *nick, char *message) {
  if (chat_window_loc >= CHAT_HEIGHT - 5) {
    memmove(chat_window_buffer, &(chat_window_buffer[1]),
            (sizeof(chat_window_buffer) - MAX_MESSAGE_LEN));
    strncpy(chat_window_buffer[chat_window_loc], message, MAX_MESSAGE_LEN);
  } else {
    strncpy(chat_window_buffer[chat_window_loc++], message, MAX_MESSAGE_LEN);
  }
  update_chat();
}

