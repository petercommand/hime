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
typedef void(*TEST_FUNC_CASES)(const char* name);
void test_case(const char*, TEST_FUNC, int case_no);


#endif //HIME_TEST_COMMON_H
