#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "inc/llist.h"

typedef struct _lnode_t {
  pthread_mutex_t lock;
  struct _lnode_t *next;
  void *data;
} lnode_t;

int init_list(llist_t *list) {
  list->head = NULL;
  list->count = 0;
  return pthread_mutex_init(&list->lock, NULL);
}

int insert_head(llist_t *list, void *new_data) {
  assert(list);
  pthread_mutex_lock(&list->lock);
  list->count++;

  /* Create a new node. */
  lnode_t *new_node = malloc(sizeof(lnode_t));
  new_node->next = NULL;
  new_node->data = new_data;
  pthread_mutex_init(&new_node->lock, NULL);

  /* Get the head element or make it the head element. */
  if (!list->head) {
    list->head = new_node;
    pthread_mutex_unlock(&list->lock);
    return 0;
  }
  pthread_mutex_lock(&((lnode_t *)list->head)->lock);
  
  /* Replace the head element. */
  new_node->next = list->head;
  list->head = new_node;

  pthread_mutex_unlock(&((lnode_t *)list->head)->lock);
  pthread_mutex_unlock(&list->lock);

  return 0;
}

int insert_when(llist_t *list, void *new_data, int(*cmp)(void*, void*)) {
  assert(list);
  pthread_mutex_lock(&list->lock);
  list->count++;

  /* Create a new node. */
  lnode_t *new_node = malloc(sizeof(lnode_t));
  new_node->data = new_data;
  pthread_mutex_init(&new_node->lock, NULL);
  new_node->next = NULL;

  /* Get the head if there is one, otherwise make it the head. */
  if (!list->head) {
    list->head = new_node;
    pthread_mutex_unlock(&list->lock);
    return 0;
  }
  pthread_mutex_lock(&((lnode_t *)list->head)->lock);
  lnode_t *curr = list->head;

  /* Get the next element if there is one, otherwise insert it there. */
  if (!curr->next) {
    curr->next = new_node;
    pthread_mutex_unlock(&list->lock);
    pthread_mutex_unlock(&((lnode_t *)list->head)->lock);
    return 0;
  }
  pthread_mutex_lock(&curr->next->lock);
  lnode_t *next = curr->next;

  /* Now we can let go of the list's entry lock. */
  pthread_mutex_unlock(&list->lock);

  /* Once cmp returns a nonnegative number, we insert
     directly after the current element. */
  while (next && (cmp(curr->data, new_data) < 0)) {
    /* Hand over hand. */
    if (next->next) {
      pthread_mutex_lock(&next->next->lock);
      pthread_mutex_unlock(&curr->lock);
    } else {
      /* We are at the tail, we only need one lock. */
      pthread_mutex_unlock(&curr->lock);
    }

    curr = next;
    next = next->next;
  }

  /* Insert the node. */
  new_node->next = next;
  curr->next = new_node;

  pthread_mutex_unlock(&curr->lock);
  /* If we were not at the tail, we have two locks. */
  if (next) {
    pthread_mutex_unlock(&next->lock);
  }

  return 0;
}

int insert_tail(llist_t *list, void *new_data) {
  assert(list);
  return 0;
}

void *remove_head(llist_t *list) {
  assert(list);
  pthread_mutex_lock(&list->lock);

  /* Get the head element or make it the head element. */
  if (!list->head) {
    pthread_mutex_unlock(&list->lock);
    return NULL;
  }
  pthread_mutex_lock(&((lnode_t *)list->head)->lock);
  
  lnode_t *new_head = ((lnode_t *)list->head)->next;

  if (new_head) {
    pthread_mutex_lock(&((lnode_t *)new_head)->lock);
  }

  lnode_t *head = list->head;
  void *data = ((lnode_t *)list->head)->data;
  list->head = new_head;

  if (new_head) {
    pthread_mutex_unlock(&((lnode_t *)new_head)->lock);
  }

  pthread_mutex_unlock(&head->lock);
  free(head);

  list->count--;
  pthread_mutex_unlock(&list->lock);

  return data;
}

void *remove_when(llist_t *list, void *data, int(*cmp)(void*, void*)) {
  assert(list);
  pthread_mutex_lock(&list->lock);

  /* Get the head if there is one, otherwise return NULL. */
  if (!list->head) {
    pthread_mutex_unlock(&list->lock);
    return NULL;
  }
  pthread_mutex_lock(&((lnode_t *)list->head)->lock);
  lnode_t *curr = list->head;

  /* Get the next element if there is one, otherwise remove it here. */
  if (!curr->next) {
    lnode_t *head = list->head;
    list->head = NULL;
    pthread_mutex_unlock(&list->lock);
    pthread_mutex_unlock(&head->lock);
    void *data = head->data;
    free(head);
    return head->data;
  }
  pthread_mutex_lock(&curr->next->lock);
  lnode_t *next = curr->next;

  /* Now we can let go of the list's entry lock. */
  list->count--;
  pthread_mutex_unlock(&list->lock);

  /* Once cmp returns a nonnegative number, we remove
     directly after the current element. */
  while (next && (cmp(curr->data, data) < 0)) {
    /* Hand over hand. */
    if (next->next) {
      pthread_mutex_lock(&next->next->lock);
      pthread_mutex_unlock(&curr->lock);
    } else {
      /* We are at the tail, we only need one lock. */
      pthread_mutex_unlock(&curr->lock);
    }

    curr = next;
    next = next->next;
  }

  /* Remove the node if it exists. */
  if (next) {
    if (next->next) {
      pthread_mutex_lock(&next->next->lock);
    }
    curr->next = next->next;
    pthread_mutex_unlock(&curr->lock);
    pthread_mutex_unlock(&next->lock);
    if (next->next) {
      pthread_mutex_unlock(&next->next->lock);
    }

    void *data = next->data;
    free(next);
    return data;
  } else {
    pthread_mutex_unlock(&curr->lock);
    return NULL;
  }

  return NULL;

}

void *remove_tail(llist_t *list) {
  assert(list);
  pthread_mutex_lock(&list->lock);

  /* Get the head if there is one, otherwise return NULL. */
  if (!list->head) {
    pthread_mutex_unlock(&list->lock);
    printf("uh oh");
    return NULL;
  }
  pthread_mutex_lock(&((lnode_t *)list->head)->lock);
  lnode_t *curr = list->head;

  /* Get the next element if there is one, otherwise remove it here. */
  if (!curr->next) {
    lnode_t *head = list->head;
    list->head = NULL;
    pthread_mutex_unlock(&list->lock);
    pthread_mutex_unlock(&head->lock);
    void *data = head->data;
    free(head);
    return head->data;
  }
  pthread_mutex_lock(&curr->next->lock);
  lnode_t *next = curr->next;

  /* Now we can let go of the list's entry lock. */
  list->count--;
  pthread_mutex_unlock(&list->lock);

  /* Wait until we hit the tail. */
  while (next->next) {
    /* Hand over hand. */
    pthread_mutex_lock(&next->next->lock);
    pthread_mutex_unlock(&curr->lock);

    curr = next;
    next = next->next;
  }

  /* Remove the node. */
  void *data = next->data;
  curr->next = NULL;
  pthread_mutex_unlock(&next->lock);
  free(next);
  pthread_mutex_unlock(&curr->lock);
  return data;
}

void print_list(llist_t *list) {
  assert(list);
  pthread_mutex_lock(&list->lock);
  lnode_t *curr = list->head;

  int i = 0;
  while (curr) {
    printf("[%d] %p\n", i++, curr->data);
    curr = curr->next;
  }
  pthread_mutex_unlock(&list->lock);

}

unsigned int count_list(llist_t *list) {
  pthread_mutex_lock(&list->lock);

  unsigned int count = list->count;

  pthread_mutex_unlock(&list->lock);
  return count;
}

