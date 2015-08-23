#include "test-hime-event.h"

static int test_var = 5;

void event_test_0_func(HIME_EVENT eve, void* pointer);
void event_test_1_func(HIME_EVENT eve, void* pointer);
TEST_FUNC list[] = {
    event_test_0,
    event_test_1
};


void event_test_0() {
  int num = 10;
  hime_event_connect(ENGINE_CHANGED_EVENT, event_test_0_func, &num);
  HIME_EVENT eve;
  eve.type = ENGINE_CHANGED_EVENT;
  hime_event_dispatch(eve);
  if(test_var != 6) {
    test_error();
  }
}

void event_test_1() {
  int num = 11;
  hime_event_connect(PREEDIT_CHANGED_EVENT, event_test_1_func, &num);
}

void event_test_0_func(HIME_EVENT eve, void* pointer) {
  test_equal(*((int *)pointer) == 10);
  test_var++;
}

void event_test_1_func(HIME_EVENT eve, void* pointer) {
  test_equal(*((int *)pointer) == 11);
}

void test_event(const char* name){
  int i;
  for(i = 0;i < sizeof(list)/sizeof(list[0]);i++) {
    test_case(name, list[i], i);
  }
}