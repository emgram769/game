#ifndef _CHAT_H
#define _CHAT_H

#define CHAT_WIDTH 40
#define CHAT_HEIGHT MAIN_HEIGHT
#define CHAT_BUFSIZE 100

void init_chat(void);
int chat_open(void);
void chat_process_keyboard(int c);
void new_message(char *nick, char *message);

#endif /* _CHAT_H */
