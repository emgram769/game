#ifndef _LLIST_H
#define _LLIST_H

#include <pthread.h>

typedef struct {
  pthread_mutex_t lock;
  void *head;
  unsigned int count;
} llist_t;

int init_list(llist_t *list);
int insert_head(llist_t *list, void *new_data);
int insert_when(llist_t *list, void *new_data, int(*cmp)(void*, void*));
int insert_tail(llist_t *list, void *new_data);

void *remove_head(llist_t *list);
void *remove_tail(llist_t *list);

unsigned int count_list(llist_t *list);
void print_list(llist_t *list);

#endif /* _LLIST_H */
