#include <stdio.h>
#include <stdlib.h>
#include "test-common.h"



void test_error(){
  printf("Failed!\n");
  exit(1);
}

void test_equal(int in){
  if(!in) {
    test_error();
  }
}