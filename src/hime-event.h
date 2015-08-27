#ifndef HIME_EVENT_H
#define HIME_EVENT_H

#include <string.h>
#include <stdio.h>
#include "hime.h"
#include "gtab.h"

typedef enum {
  HIME_EVENT_RETURN_NOT_PROCESSED=0,
  HIME_EVENT_RETURN_PROCESSED=1,
  HIME_EVENT_RETURN_STOP_PROCESSING=2
} HIME_EVENT_HANDLER_RETURN_TYPE;

typedef enum {
  HIME_INPUT_METHOD_ENGINE_EVENT_TYPE,
  HIME_INCREASE_USE_COUNT_EVENT_TYPE,
  HIME_INPUT_METHOD_EVENT_TYPE,
  HIME_KEY_EVENT_TYPE,
  HIME_PREEDIT_EVENT_TYPE,
  HIME_HALF_FULL_EVENT_TYPE,
  HIME_EVENT_N //HIME_EVENT_N should not be used as an event type, this is only used as a size indicator for HIME_EVENT_TYPE
} HIME_EVENT_TYPE;


typedef enum {
  HIME_SET_EN_CH,
  HIME_SET_PHO_MODE,
  HIME_CREATE_PHRASE_SAVE_MENU,
  HIME_DESTROY_PHRASE_SAVE_MENU
} HIME_INPUT_METHOD_ENGINE_EVENT_SUBTYPE;

typedef struct HIME_INPUT_METHOD_ENGINE_EVENT {
  HIME_INPUT_METHOD_ENGINE_EVENT_SUBTYPE type;
} HIME_INPUT_METHOD_ENGINE_EVENT;




typedef struct HIME_INCREASE_USE_COUNT_EVENT {
  INMD* source;
  void* pho;
  char* ch;
  int len;
} HIME_INCREASE_USE_COUNT_EVENT;


typedef enum {
  HIME_KEY_PRESS,
  HIME_KEY_RELEASE
} HIME_KEY_EVENT_SUBTYPE;

typedef struct HIME_KEY_EVENT {
  HIME_KEY_EVENT_SUBTYPE type;
  KeySym keysym;
  u_int kev_state;
} HIME_KEY_EVENT;


typedef struct HIME_PREEDIT_EVENT {
  int type;
} HIME_PREEDIT_EVENT;

typedef enum {
  HIME_TO_FULL,
  HIME_TO_HALF
} HIME_HALF_FULL_EVENT_SUBTYPE;

typedef struct HIME_HALF_FULL_EVENT {
  HIME_HALF_FULL_EVENT_SUBTYPE type;
} HIME_HALF_FULL_CHANGED_EVENT;

typedef struct HIME_CLIENT_STATE_EVENT {
  int type;
} HIME_CLIENT_STATE_EVENT;

typedef struct HIME_EVENT {
  HIME_EVENT_TYPE type;
  union {
    HIME_INPUT_METHOD_ENGINE_EVENT input_method_engine_event;
    HIME_INCREASE_USE_COUNT_EVENT increase_use_count_event;
    HIME_KEY_EVENT key_event;
    HIME_PREEDIT_EVENT preedit_event;
    HIME_HALF_FULL_CHANGED_EVENT half_full_event;
    HIME_CLIENT_STATE_EVENT client_state_event;
  };
} HIME_EVENT;


typedef struct event_list_item {
  struct event_list_item *prev;
  struct event_list_item *next;
  HIME_EVENT_HANDLER_RETURN_TYPE (*func_cb) (HIME_EVENT, void*);
  void* pointer;
} event_list_item;


typedef struct event_list {
  event_list_item *head;
  event_list_item *tail;
} event_list;


#ifdef HIME_EVENT_C
void event_list_append(event_list* list, HIME_EVENT_HANDLER_RETURN_TYPE (*func_cb) (HIME_EVENT, void*), void*);
event_list* event_list_new();
void event_list_free(event_list* list);
#endif //HIME_EVENT_C

void hime_event_connect(HIME_EVENT_TYPE event, HIME_EVENT_HANDLER_RETURN_TYPE (*func_cb) (HIME_EVENT, void*), void*);
int hime_event_dispatch(HIME_EVENT event);
int hime_event_module_dispatch(HIME_EVENT event, INMD* input_method, void (*default_handler)());
int hime_event_current_module_dispatch(HIME_EVENT event, void (*default_handler)());




#endif //HIME_EVENT_H