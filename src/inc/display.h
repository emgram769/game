#ifndef _DISPLAY_H 
#define _DISPLAY_H 

#define MAIN_WIDTH   80
#define MAIN_HEIGHT  25
#define INBOUND(x, y) ((x) >= 0 && (x) < MAIN_WIDTH && (y) >= 0 && (y) < MAIN_HEIGHT)

typedef struct _display_t {
  void *window;
  void *panel;
  void (*draw_char)(struct _display_t *self, char c, int x, int y);
  void (*draw_str)(struct _display_t *self, char *str, int x, int y);
  void (*draw_strn)(struct _display_t *self, char *str, int n, int x, int y);
  void (*hide)(struct _display_t *self);
  void (*show)(struct _display_t *self);
  void (*get_cursor)(struct _display_t *self);
  struct {
    int x;
    int y;
  } cursor;
} display_t;


//void draw_char(char c, int x, int y);
//void draw_str(char *str, int x, int y);

display_t *init_display(void);
display_t *create_window(int width, int height, int x, int y);

int get_char(void);
void cursor(int option);

void toggle_chat(void);
void show_chat(void);
void hide_chat(void);

#endif /* _DISPLAY_H */
