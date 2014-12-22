#include <curses.h>

#include <display.h>

static WINDOW *main_window;

int init_display(void) {
  if ((main_window = initscr()) == NULL) {
    printf("Error initialising ncurses.\n");
    return 1;
  }

  noecho();
  curs_set(0);

  return 0;
}

void draw_char(char c, int x, int y) {
  if (!INBOUND(x, y)) {
    return;
  }

  mvaddch(y, x, c);
  refresh();
}

void draw_str(char *str, int x, int y) {
  if (!INBOUND(x, y)) {
    return;
  }

  mvaddstr(y, x, str);
  refresh();
}

int get_char(void) {
  return getch();
}

