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
  hime_event_emit(eve);
  if(test_var != 6) {
    test_error();
  }
}

void event_test_1() {

}

void func() {
  test_var++;
}

void test_event(){
  int i;
  for(i = 0;i < sizeof(list);i++) {
    printf("Testing event: test case #%d...", i);
    list[i]();
    printf("Success\n");
  }
}