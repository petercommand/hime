#ifndef HIME_SIGNAL_H
#define HIME_SIGNAL_H

#include <string.h>
#include <stdio.h>
#include "hime.h"

typedef enum {
  CLIENT_STATE_CHANGE,
  KEY_PRESSED,
  KEY_RELEASED,
  HIME_SIGNAL_N
} HIME_SIGNAL;



typedef struct notify_list_item {
  struct notify_list_item *prev;
  struct notify_list_item *next;
  void (*func_cb) (HIME_SIGNAL signal, void* pointer);
  void* pointer;
} notify_list_item;


typedef struct notify_list {
  notify_list_item *head;
  notify_list_item *tail;
} notify_list;

void notify_list_append(notify_list* list, void (*func_cb) (HIME_SIGNAL, void*), void* pointer);
notify_list*notify_list_new();
void hime_signal_connect(HIME_SIGNAL signal, void (*func_cb) (HIME_SIGNAL, void*), void* pointer);
void hime_signal_emit(HIME_SIGNAL signal);





#endif //HIME_SIGNAL_H
