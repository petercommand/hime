#ifndef HIME_TEST_COMMON_H
#define HIME_TEST_COMMON_H
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

void test_error();
void test_equal(int);

typedef void(*TEST_FUNC)();


#endif //HIME_TEST_COMMON_H
