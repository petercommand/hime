#include "test-common.h"
#include "test-hime-event.h"



typedef struct TESTS {
  const char* name;
  TEST_FUNC_CASES func;
} TESTS;


static TESTS list[] = {
    {"hime-event", test_event}
};


int main() {
  int i;
  for(i = 0; i < sizeof(list)/sizeof(list[0]);i++) {
    printf(">>>> Testing %s <<<<\n", list[i].name);
    list[i].func(list[0].name);
  }
  return 0;
}