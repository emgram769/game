#include "inc/llist.h"
#include <stdio.h>

static llist_t list;

int greater_than(void *a, void *b) {
  return (unsigned long)a < (unsigned long)b ? -1 : 1;
}

void *adder(void *arg) {
  int i;
  for (i = 0; i < 1000; i++) {
    insert_when(&list, (void *)(long)i, &greater_than);
    //insert_head(&list, (void *)(long)i);
    printf("insert count: %d\n", count_list(&list));
    remove_head(&list);
    printf("remove count: %d\n", count_list(&list));
  }
  return NULL;
}

int main(void) {
  pthread_t t;
  init_list(&list);

  int i;
  for (i = 0; i < 10; i++) {
    (void) pthread_create(&t, NULL, adder, NULL);
  }

  (void) pthread_join(t, NULL);

  print_list(&list);
  return 0;
}

