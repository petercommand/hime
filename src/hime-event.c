#define HIME_EVENT_C

#include "hime-event.h"
#include "hime-module-cb.h"
#include "eve.h"

static event_list* event_notify_list[HIME_EVENT_N];

void event_list_append(event_list* list, HIME_EVENT_HANDLER_RETURN_TYPE (*func_cb) (HIME_EVENT, void*), void* pointer){
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


void hime_event_connect(HIME_EVENT_TYPE event, HIME_EVENT_HANDLER_RETURN_TYPE (*func_cb) (HIME_EVENT, void*), void* pointer){
  //This function connects func_cb and pointer with the given event
  // func_cb will be call with event and pointer when the event is emitted
  if(!event_notify_list[event]) {
    event_notify_list[event] = event_list_new();
  }
  event_list_append(event_notify_list[event], func_cb, pointer);
}



int hime_event_module_dispatch(HIME_EVENT event, void (*default_handler)()) {
  //This function uses module_cb() from eve.c, make sure that current_CS is valid, current input method is a module, and that the module has been initialized before using this function
  //Dispatches the optional event to currently loaded module, if target returns false, default_handler is called
  //default_handler can be null if there is no default action
  //return false if the module want the default_handler to be evaluated, or the module does not implement module_event_handler()
  //return true if the module return true from module_event_handler, which means that default_handler is not called
  if(module_cb()->module_event_handler) {
    if(!module_cb()->module_event_handler(event)) {//false
      if (default_handler != NULL) {
        default_handler();
      }
      return 0;
    }
    else {//true
      return 1;
    }
  }
  else {
    if(default_handler != NULL) {
      default_handler();
    }
    return 0;
  }
}

int hime_event_dispatch(HIME_EVENT event) {
  //Dispatch a HIME_EVENT, first, it triggers the first connected handler,
  //If the handler returns HIME_EVENT_RETURN_PROCESSED, no other handler is called
  //If the handler returns HIME_EVENT_RETURN_NOT_PROCESSED, the next handler will be evaluated, and so on, until one of the handler return true, or there is no more handlers in the list
  //If the handler returns HIME_EVENT_RETURN_STOP_PROCESSING, no other handler is called, return this value when the event is processed, but wants to let this function return false
  //return false if the last triggered handler returns HIME_EVENT_RETURN_NOT_PROCESSED or HIME_EVENT_RETURN_STOP_PROCESSING
  //return true if the last triggered handler return HIME_EVENT_RETURN_PROCESSED
  int status;
  if (event_notify_list[event.type]) {
    event_list_item *item = event_notify_list[event.type]->head;
    while (item) {
      status = item->func_cb(event, item->pointer);
      switch (status) {
        case HIME_EVENT_RETURN_NOT_PROCESSED:
          item = item->next;
          break;
        case HIME_EVENT_RETURN_PROCESSED:
          return 1;
        case HIME_EVENT_RETURN_STOP_PROCESSING:
          return 0;
        default:
          return 0;//other return value is invalid, treat it as not processed
      }
    }
  }
  return 0;
}