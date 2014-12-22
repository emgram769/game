#include <panel.h>
#include <signal.h>
#include <ctype.h>

#include <display.h>

static WINDOW *main_window;
static WINDOW *chat_window;
static PANEL *chat_box;
static int chat_box_hidden;
static char chat_buffer[CHAT_BUFSIZE];
static int chat_buffer_loc;

void resize_handler(int sig) {
  endwin();
  refresh();
  move_panel(chat_box, 0, COLS - CHAT_WIDTH);

  /* Hack to refresh it. */
  toggle_chat();
  toggle_chat();
}

int init_display(void) {
  if ((main_window = initscr()) == NULL) {
    printf("Error initialising ncurses.\n");
    return 1;
  }

  noecho();
  curs_set(0);

  /* Set up chat box. */
  chat_window = newwin(CHAT_HEIGHT, CHAT_WIDTH, 0, COLS - CHAT_WIDTH);
  box(chat_window, 0, 0);
  mvwaddch(chat_window, CHAT_HEIGHT - 2, 1, '>');
  chat_box = new_panel(chat_window);
  hide_panel(chat_box);
  chat_box_hidden = 1;

  update_panels();

  /* Handle resizing. */
  signal(SIGWINCH, resize_handler);

  return 0;
}

void toggle_chat(void) {

  if (chat_box_hidden) {
    show_chat();
  } else {
    hide_chat();
  }

}

void show_chat(void) {

  show_panel(chat_box);
  chat_box_hidden = 0;
  curs_set(1);

  update_panels();
  doupdate();
}

void hide_chat(void) {

  hide_panel(chat_box);
  chat_box_hidden = 1;
  curs_set(0);

  update_panels();
  doupdate();
}

void draw_char(char c, int x, int y) {
  if (!INBOUND(x, y)) {
    return;
  }

  mvwaddch(main_window, y, x, c);
  refresh();
}

void draw_str(char *str, int x, int y) {
  if (!INBOUND(x, y)) {
    return;
  }

  mvwaddstr(main_window, y, x, str);
  refresh();
}

int get_char(void) {
  int c;

  while ((c = getch()) && !chat_box_hidden) {

    if (c == 27) {
      return 'c'; /* Escape? :) */
    } else if (c == 127 || c == 8) {
      if (chat_buffer_loc > 0) {
        chat_buffer[--chat_buffer_loc] = '\0';
        int x, y;
        getyx(chat_window, y, x);
        mvwaddch(chat_window, y, x - 1, ' ');
      }
    } else if (c == '\n' || c == '\r') {

    } else if (chat_buffer_loc < CHAT_BUFSIZE && isalpha(c)) {
      chat_buffer[chat_buffer_loc++] = c;
    }

    int offset = chat_buffer_loc - (CHAT_WIDTH - 4);
    offset = offset > 0 ? offset : 0;
    mvwaddnstr(chat_window, CHAT_HEIGHT - 2, 2, chat_buffer + offset, CHAT_WIDTH - 4);
    wrefresh(chat_window);

  }

  return c;
}

