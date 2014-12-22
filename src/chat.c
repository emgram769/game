#include <ctype.h>

#include <display.h>

#include <chat.h>

static int chat_box_hidden;
static char chat_buffer[CHAT_BUFSIZE + 1];
static int chat_buffer_loc;

static display_t *chat_window;

void init_chat(void) {
  chat_window = create_window(CHAT_HEIGHT, CHAT_WIDTH, MAIN_WIDTH - CHAT_WIDTH, 0);
  chat_window->draw_char(chat_window, '>', 1, CHAT_HEIGHT - 2);
  hide_chat();
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
      chat_window->draw_char(chat_window, ' ', chat_window->cursor.x - 1, chat_window->cursor.y);
    }
  } else if (c == '\n' || c == '\r') {

  } else if (chat_buffer_loc < CHAT_BUFSIZE && isprint(c)) {
    chat_buffer[chat_buffer_loc++] = c;
  }

  int offset = chat_buffer_loc - (CHAT_WIDTH - 4);
  offset = offset > 0 ? offset : 0;
  chat_window->draw_strn(chat_window, chat_buffer + offset, CHAT_WIDTH - 4, 2, CHAT_HEIGHT - 2);

}

