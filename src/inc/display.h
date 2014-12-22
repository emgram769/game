#ifndef _DISPLAY_H 
#define _DISPLAY_H 

#define WIDTH   80
#define HEIGHT  25
#define INBOUND(x, y) ((x) >= 0 && (x) < WIDTH && (y) >= 0 && (y) < HEIGHT)

int init_display(void);
void draw_char(char c, int x, int y);
void draw_str(char *str, int x, int y);
int get_char(void);

#endif /* _DISPLAY_H */
