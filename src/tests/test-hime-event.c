#include "test-hime-event.h"

static int test_var = 5;

void func(HIME_EVENT eve, void* pointer);

TEST_FUNC list[] = {
    event_test_0,
    event_test_1
};


void event_test_0() {
  int num = 10;
  hime_event_connect(ENGINE_CHANGED_EVENT, func, GINT_TO_POINTER(num));
  HIME_EVENT eve;
  eve.type = ENGINE_CHANGED_EVENT;
  hime_event_dispatch(eve);
  if(test_var != 6) {
    test_error();
  }
}

void event_test_1() {

}

void func(HIME_EVENT eve, void* pointer) {
  test_equal(GPOINTER_TO_INT(pointer) == 10);
  test_var++;
}

void test_event(const char* name){
  int i;
  for(i = 0;i < sizeof(list)/sizeof(list[0]);i++) {
    test_case(name, list[i], i);
  }
}