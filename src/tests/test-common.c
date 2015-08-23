#include <stdio.h>
#include <stdlib.h>
#include "test-common.h"


void test_case(const char* name, TEST_FUNC func, int case_no) {
  printf("Testing %s: test case #%d...", name, case_no);
  func();
  printf("Success\n");
}

void test_error(){
  printf("Failed!\n");
  exit(1);
}

void test_equal(int in){
  if(!in) {
    test_error();
  }
}