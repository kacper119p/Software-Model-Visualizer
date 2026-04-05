#pragma once
#include <stdio.h>

#include "ansiColors.h"

typedef struct TestEntry TestEntry;

typedef void (*TestFunction)(TestEntry*);

typedef enum TestResult { TestResult_Success, TestResult_Failure } TestResult;

struct TestEntry {
  const char* Name;
  const char* FileName;
  TestFunction Function;
  TestResult TestResult;
};

#define ASSERT(cond)                                                           \
  do {                                                                         \
    if (!(cond)) {                                                             \
      _Test_Data_->TestResult = TestResult_Failure;                            \
      printf(ANSI_RED                                                          \
             "ASSERTION FAILED" ANSI_RESET                                     \
             ": %s:%d:\n Expected condition: (%s) to evaluate to " ANSI_GREEN  \
             "true" ANSI_RESET ", but was " ANSI_RED "false" ANSI_RESET ".\n", \
             __FILE__, __LINE__, #cond);                                       \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_FLOAT_EQ(a, b, eps)                                             \
  ASSERT(((a) - (b)) < (eps) && ((b) - (a)) < (eps))

#define TEST_PREFIX_(a, b) a##__##b
#define TEST_PREFIX(a, b) TEST_PREFIX_(a, b)
#define TEST(testName)                                                         \
  void TEST_PREFIX(TEST_FILE_PREFIX, testName)(TestEntry * _Test_Data_)
