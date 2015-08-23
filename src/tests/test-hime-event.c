#include "test-hime-event.h"

static int test_var = 5;

void func();

TEST_FUNC list[] = {
    event_test_0,
    event_test_1
};


void event_test_0() {
  hime_event_connect(ENGINE_CHANGED_EVENT, func, "test");
  HIME_EVENT eve;
  eve.type = ENGINE_CHANGED_EVENT;
  hime_event_dispatch(eve);
  if(test_var != 6) {
    test_error();
  }
}

void event_test_1() {

}

void func() {
  test_var++;
}

void test_event(const char* name){
  int i;
  for(i = 0;i < sizeof(list)/sizeof(list[0]);i++) {
    test_case(name, list[i], i);
  }
}