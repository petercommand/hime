#ifndef HIME_EVENT_H
#define HIME_EVENT_H

#include <string.h>
#include <stdio.h>
#include "hime.h"

typedef enum {
  ENGINE_CHANGED_EVENT,
  INPUT_METHOD_CHANGED_EVENT,
  KEY_PRESSED_EVENT,
  KEY_RELEASED_EVENT,
  PREEDIT_CHANGED_EVENT,
  HALF_FULL_CHANGED_EVENT,
  HIME_EVENT_N //HIME_EVENT_N should not be used as an event type, this is only used as a size indicator for HIME_EVENT_TYPE
} HIME_EVENT_TYPE;

typedef struct HIME_ENGINE_CHANGED_EVENT {
  int type;
} HIME_ENGINE_CHANGED_EVENT;

typedef struct HIME_KEY_PRESSED_EVENT {
  int type;
} HIME_KEY_PRESSED_EVENT;

typedef struct HIME_KEY_RELEASED_EVENT {
  int type;
} HIME_KEY_RELEASED_EVENT;

typedef struct HIME_PREEDIT_CHANGED_EVENT {
  int type;
} HIME_PREEDIT_CHANGED_EVENT;

typedef struct HIME_HALF_FULL_CHANGED_EVENT {
  int type;
} HIME_HALF_FULL_CHANGED_EVENT;

typedef struct HIME_CLIENT_STATE_CHANGED_EVENT {
  int type;
} HIME_CLIENT_STATE_CHANGED_EVENT;

typedef struct HIME_EVENT {
  HIME_EVENT_TYPE type;
  union {
    HIME_ENGINE_CHANGED_EVENT engine_changed_event;
    HIME_KEY_PRESSED_EVENT key_pressed_event;
    HIME_KEY_RELEASED_EVENT key_released_event;
    HIME_PREEDIT_CHANGED_EVENT preedit_changed_event;
    HIME_HALF_FULL_CHANGED_EVENT half_full_changed_event;
    HIME_CLIENT_STATE_CHANGED_EVENT client_state_changed_event;
  };
} HIME_EVENT;


typedef struct event_list_item {
  struct event_list_item *prev;
  struct event_list_item *next;
  void (*func_cb) (HIME_EVENT, void*);
  void* pointer;
} event_list_item;


typedef struct event_list {
  event_list_item *head;
  event_list_item *tail;
} event_list;


#ifdef HIME_EVENT_C
void event_list_append(event_list* list, void (*func_cb) (HIME_EVENT, void*), void*);
event_list* event_list_new();
void event_list_free(event_list* list);
#endif //HIME_EVENT_C

void hime_event_connect(HIME_EVENT_TYPE event, void (*func_cb) (HIME_EVENT, void*), void*);
void hime_event_dispatch(HIME_EVENT event);





#endif //HIME_EVENT_H