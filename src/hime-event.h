#ifndef HIME_EVENT_H
#define HIME_EVENT_H

#include <string.h>
#include <stdio.h>
#include "hime.h"

typedef enum {
  CLIENT_STATE_CHANGED,
  KEY_PRESSED,
  KEY_RELEASED,
} HIME_EVENT_TYPE;

typedef struct HIME_EVENT {
  HIME_EVENT_TYPE type;
  union {
    HIME_ENGINE_CHANGED_EVENT engine_changed_event;
    HIME_KEY_PRESSED_EVENT key_pressed_event;
    HIME_KEY_RELEASED_EVENT key_released_event;
    HIME_HALF_FULL_CHANGED_EVENT half_full_changed_event;
    HIME_CLIENT_STATE_CHANGED_EVENT client_state_changed_event;
  };
} HIME_EVENT;

typedef struct event_list_item {
  struct event_list_item *prev;
  struct event_list_item *next;
  void (*func_cb) (HIME_EVENT event);
} event_list_item;


typedef struct event_list {
  event_list_item *head;
  event_list_item *tail;
} event_list;

void notify_list_append(event_list* list, void (*func_cb) (HIME_EVENT));
event_list* event_list_new();
void hime_event_connect(HIME_EVENT event, void (*func_cb) (HIME_EVENT));
void hime_event_emit(HIME_EVENT event);





#endif //HIME_EVENT_H