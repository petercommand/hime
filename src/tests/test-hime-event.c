#include "test-hime-event.h"


TEST_FUNC list[] = {
    event_test_0,
    event_test_1
};


void event_test_0() {

}

void event_test_1() {

}


void test_event(){
  int i;
  for(i = 0;i < sizeof(list);i++) {
    printf("Testing event: test case #%d...", i);
    list[i]();
    printf("Success\n");
  }
}