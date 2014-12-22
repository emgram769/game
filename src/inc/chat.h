#ifndef _CHAT_H
#define _CHAT_H

#define CHAT_WIDTH 40
#define CHAT_HEIGHT MAIN_HEIGHT
#define CHAT_BUFSIZE 100

void init_chat(void);
int chat_open(void);
void chat_process_keyboard(int c);

#endif /* _CHAT_H */
