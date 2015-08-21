#include "hime-signal.h"
#include "hime-client-state.h"


static notify_list* signal_notify_list[HIME_SIGNAL_N];

void notify_list_append(notify_list* list, void (*func_cb) (HIME_SIGNAL, void*), void* pointer){
  if(list->head == NULL){
    notify_list_item *item = tzmalloc(notify_list_item, 1);
    if(item == NULL){
      perror("failed to tzmalloc");
      return;
    }
    bzero(item, sizeof(notify_list_item));
    list->head = list->tail = item;
    item->func_cb = func_cb;
    item->pointer = pointer;
  }
  else{
    notify_list_item *prev = list->tail;
    notify_list_item *item = tzmalloc(notify_list_item, 1);
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

notify_list* notify_list_new(){
  notify_list* list = tzmalloc(notify_list, 1);
  if(list == NULL){
    perror("failed to tzmalloc");
    return NULL;
  }
  bzero(list, sizeof(notify_list));
  return list;
}

void notify_list_free(notify_list* list){
  if(list){
    notify_list_item* item = list->head;
    notify_list_item* next = NULL;
    while(item){
      next = item->next;
      free(item);
      item = next;
    }
  }
}


void hime_signal_connect(HIME_SIGNAL signal, void (*func_cb) (HIME_SIGNAL, void*), void* pointer){
  //This function connects func_cb and pointer with the given signal
  // func_cb will be call with signal and pointer when the signal is emitted
  if(!signal_notify_list[signal]) {
    signal_notify_list[signal] = notify_list_new();
  }
  notify_list_append(signal_notify_list[signal], func_cb, pointer);
}


void hime_signal_emit(HIME_SIGNAL signal){
  notify_list_item* item = signal_notify_list[signal]->head;
  while(item){
    item->func_cb(signal, item->pointer);
    item = item->next;
  }
}