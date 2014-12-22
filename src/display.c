#include <stdlib.h>

#include <panel.h>
#include <signal.h>
#include <ctype.h>

#include <display.h>

static WINDOW *main_window;

void resize_handler(int sig) {
  endwin();
  refresh();
  //move_panel(chat_box, 0, COLS - CHAT_WIDTH);

  /* Hack to refresh it. */
  toggle_chat();
  toggle_chat();
}

void draw_char(display_t *self, char c, int x, int y) {
  mvwaddch(self->window, y, x, c);
  wrefresh(self->window);
}

void draw_str(display_t *self, char *str, int x, int y) {
  mvwaddstr(self->window, y, x, str);
  wrefresh(self->window);
}

void draw_strn(display_t *self, char *str, int n, int x, int y) {
  mvwaddnstr(self->window, y, x, str, n);
  wrefresh(self->window);
}

void hide(display_t *self) {
  hide_panel(self->panel);
  update_panels();
  doupdate();
}

void show(display_t *self) {
  show_panel(self->panel);
  update_panels();
  doupdate();
}

void get_cursor(display_t *self) {
  getyx(self->window, self->cursor.y, self->cursor.x);
}

display_t *init_display(void) {
  if ((main_window = initscr()) == NULL) {
    printf("Error initialising ncurses.\n");
    return NULL;
  }

  noecho();
  curs_set(0);

  /* Set up chat box. */
  /*chat_window = newwin(CHAT_HEIGHT, CHAT_WIDTH, 0, COLS - CHAT_WIDTH);
  box(chat_window, 0, 0);
  mvwaddch(chat_window, CHAT_HEIGHT - 2, 1, '>');
  chat_box = new_panel(chat_window);
  hide_panel(chat_box);
  chat_box_hidden = 1;*/

  /* Handle resizing. */
  signal(SIGWINCH, resize_handler);

  display_t *out = malloc(sizeof(display_t));
  out->window = main_window;
  out->draw_char = &draw_char;
  out->draw_str = &draw_str;

  refresh();

  return out;
}

display_t *create_window(int width, int height, int x, int y) {
  WINDOW *window = newwin(width, height, y, x);
  box(window, 0, 0);
  PANEL *panel = new_panel(window);
  update_panels();
  doupdate();

  display_t *out = malloc(sizeof(display_t));
  out->window = window;
  out->panel = panel;
  out->draw_char = &draw_char;
  out->draw_str = &draw_str;
  out->draw_strn = &draw_strn;
  out->hide = &hide;
  out->show = &show;
  out->get_cursor = &get_cursor;

  return out;
}

int get_char(void) {
  int c = getch();

 /* while ((c = getch()) && !chat_box_hidden) {
  
    if (c == 27) {
      return 'c';
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

  }*/

  return c;
}

void cursor(int option) {
  curs_set(option);
}

