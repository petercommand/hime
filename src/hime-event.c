#define HIME_EVENT_C

#include "hime-event.h"

static event_list* event_notify_list[HIME_EVENT_N];

void event_list_append(event_list* list, void (*func_cb) (HIME_EVENT, void*), void* pointer){
  //Add the callback function and the pointer to the event_list
  if(list->head == NULL){
    event_list_item *item = tzmalloc(event_list_item, 1);
    if(item == NULL){
      perror("failed to tzmalloc");
      return;
    }
    bzero(item, sizeof(event_list_item));
    list->head = list->tail = item;
    item->func_cb = func_cb;
    item->pointer = pointer;
  }
  else{
    event_list_item *prev = list->tail;
    event_list_item *item = tzmalloc(event_list_item, 1);
    if(item == NULL){
      perror("failed to tzmalloc");
      return;
    }
    prev->next = item;
    item->prev = prev;
    item->next = NULL;
    item->func_cb = func_cb;
    item->pointer = pointer;

    list->tail = item;
  }
}

event_list* event_list_new(){
  //Allocate heap for a new event_list
  event_list* list = tzmalloc(event_list, 1);
  if(list == NULL){
    perror("failed to tzmalloc");
    return NULL;
  }
  bzero(list, sizeof(event_list));
  return list;
}

void event_list_free(event_list* list){
  //Free the entire event_list
  if(list){
    event_list_item* item = list->head;
    event_list_item* next = NULL;
    while(item){
      next = item->next;
      free(item);
      item = next;
    }
  }
}


void hime_event_connect(HIME_EVENT_TYPE event, void (*func_cb) (HIME_EVENT, void*), void* pointer){
  //This function connects func_cb and pointer with the given event
  // func_cb will be call with event and pointer when the event is emitted
  if(!event_notify_list[event]) {
    event_notify_list[event] = event_list_new();
  }
  event_list_append(event_notify_list[event], func_cb, pointer);
}


void hime_event_dispatch(HIME_EVENT event) {
  //Dispatch a HIME_EVENT, triggers all corresponding registered callback function
  if (event_notify_list[event.type]) {
    event_list_item *item = event_notify_list[event.type]->head;
    while (item) {
      item->func_cb(event, item->pointer);
      item = item->next;
    }
  }
}