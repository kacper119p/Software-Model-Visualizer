#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "ansiColors.h"

typedef struct TestEntry TestEntry;

typedef void (*TestFunction)(TestEntry*);

typedef enum TestResult { TestResult_Success, TestResult_Failure } TestResult;

struct TestEntry {
  const char* const Name;
  const char* const FileName;
  TestFunction Function;
  TestResult TestResult;
};

#define ASSERT_TRUE(cond)                                                      \
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

#define _ASSERT_EQUAL_INT_(Actual, Expected, Type, Format)                     \
  do {                                                                         \
    const Type _actual_ = Actual;                                              \
    const Type _expected_ = Expected;                                          \
    if (_actual_ != _expected_) {                                              \
      _Test_Data_->TestResult = TestResult_Failure;                            \
      printf(ANSI_RED "ASSERTION FAILED" ANSI_RESET ": %s:%d: "                \
                      "Expected " Format ", but was " Format ".\n",            \
             __FILE__, __LINE__, _expected_, _actual_);                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_EQUAL_INT8(Actual, Expected)                                    \
  _ASSERT_EQUAL_INT_(Actual, Expected, int8_t, "%" PRId8)
#define ASSERT_EQUAL_INT16(Actual, Expected)                                   \
  _ASSERT_EQUAL_INT_(Actual, Expected, int16_t, "%" PRId16)
#define ASSERT_EQUAL_INT32(Actual, Expected)                                   \
  _ASSERT_EQUAL_INT_(Actual, Expected, int32_t, "%" PRId32)
#define ASSERT_EQUAL_INT64(Actual, Expected)                                   \
  _ASSERT_EQUAL_INT_(Actual, Expected, int64_t, "%" PRId64)
#define ASSERT_EQUAL_UINT8(Actual, Expected)                                   \
  _ASSERT_EQUAL_INT_(Actual, Expected, uint8_t, "%" PRIu8)
#define ASSERT_EQUAL_UINT16(Actual, Expected)                                  \
  _ASSERT_EQUAL_INT_(Actual, Expected, uint16_t, "%" PRIu16)
#define ASSERT_EQUAL_UINT32(Actual, Expected)                                  \
  _ASSERT_EQUAL_INT_(Actual, Expected, uint32_t, "%" PRIu32)
#define ASSERT_EQUAL_UINT64(Actual, Expected)                                  \
  _ASSERT_EQUAL_INT_(Actual, Expected, uint64_t, "%" PRIu64)

#define _ASSERT_CLOSE_FLOAT_(Actual, Expected, Tolerance, Type, Format)        \
  do {                                                                         \
    const Type _actual_ = Actual;                                              \
    const Type _expected_ = Expected;                                          \
    const Type _tolerance_ = Tolerance;                                        \
    if (fabs(_actual_ - _expected_) > _tolerance_) {                           \
      printf(ANSI_RED "ASSERTION FAILED" ANSI_RESET ": %s:%d: "                \
                      "Expected " Format ", but was " Format                   \
                      ", tolerance: " Format ".\n",                            \
             __FILE__, __LINE__, _actual_, _expected_, _tolerance_);           \
      _Test_Data_->TestResult = TestResult_Failure;                            \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_CLOSE_FLOAT(Actual, Expected, Tolerance)                        \
  _ASSERT_CLOSE_FLOAT_(Actual, Expected, Tolerance, float, "%f")
#define ASSERT_CLOSE_DOUBLE(Actual, Expected, Tolerance)                       \
  _ASSERT_CLOSE_FLOAT_(Actual, Expected, Tolerance, double, "%lf")

#define _ASSERT_COMPARE_(Actual, Expected, Type, Format, Comparator,           \
                         ComparisonMessage)                                    \
  do {                                                                         \
    const Type _actual_ = Actual;                                              \
    const Type _expected_ = Expected;                                          \
    if (!(_actual_ Comparator _expected_)) {                                   \
      _Test_Data_->TestResult = TestResult_Failure;                            \
      printf(ANSI_RED "ASSERTION FAILED" ANSI_RESET ": %s:%d: "                \
                      "Expected " Format " to be " ComparisonMessage Format    \
                      ".\n",                                                   \
             __FILE__, __LINE__, _expected_, _actual_);                        \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define _ASSERT_LESS_(Actual, Expected, Type, Format)                          \
  _ASSERT_COMPARE_(Actual, Expected, Type, Format, <, "less than ")

#define ASSERT_LESS_INT8(Actual, Expected)                                     \
  _ASSERT_LESS_(Actual, Expected, int8_t, "%" PRId8)
#define ASSERT_LESS_INT16(Actual, Expected)                                    \
  _ASSERT_LESS_(Actual, Expected, int16_t, "%" PRId16)
#define ASSERT_LESS_INT32(Actual, Expected)                                    \
  _ASSERT_LESS_(Actual, Expected, int32_t, "%" PRId32)
#define ASSERT_LESS_INT64(Actual, Expected)                                    \
  _ASSERT_LESS_(Actual, Expected, int64_t, "%" PRId64)
#define ASSERT_LESS_UINT8(Actual, Expected)                                    \
  _ASSERT_LESS_(Actual, Expected, uint8_t, "%" PRIu8)
#define ASSERT_LESS_UINT16(Actual, Expected)                                   \
  _ASSERT_LESS_(Actual, Expected, uint16_t, "%" PRIu16)
#define ASSERT_LESS_UINT32(Actual, Expected)                                   \
  _ASSERT_LESS_(Actual, Expected, uint32_t, "%" PRIu32)
#define ASSERT_LESS_UINT64(Actual, Expected)                                   \
  _ASSERT_LESS_(Actual, Expected, uint64_t, "%" PRIu64)
#define ASSERT_LESS_FLOAT(Actual, Expected)                                    \
  _ASSERT_LESS_(Actual, Expected, float, "%f")
#define ASSERT_LESS_DOUBLE(Actual, Expected)                                   \
  _ASSERT_LESS_(Actual, Expected, double, "%lf")

#define _ASSERT_GREATER_(Actual, Expected, Type, Format)                       \
  _ASSERT_COMPARE_(Actual, Expected, Type, Format, >, "greater than ")

#define ASSERT_GREATER_INT8(Actual, Expected)                                  \
  _ASSERT_GREATER_(Actual, Expected, int8_t, "%" PRId8)
#define ASSERT_GREATER_INT16(Actual, Expected)                                 \
  _ASSERT_GREATER_(Actual, Expected, int16_t, "%" PRId16)
#define ASSERT_GREATER_INT32(Actual, Expected)                                 \
  _ASSERT_GREATER_(Actual, Expected, int32_t, "%" PRId32)
#define ASSERT_GREATER_INT64(Actual, Expected)                                 \
  _ASSERT_GREATER_(Actual, Expected, int64_t, "%" PRId64)
#define ASSERT_GREATER_UINT8(Actual, Expected)                                 \
  _ASSERT_GREATER_(Actual, Expected, uint8_t, "%" PRIu8)
#define ASSERT_GREATER_UINT16(Actual, Expected)                                \
  _ASSERT_GREATER_(Actual, Expected, uint16_t, "%" PRIu16)
#define ASSERT_GREATER_UINT32(Actual, Expected)                                \
  _ASSERT_GREATER_(Actual, Expected, uint32_t, "%" PRIu32)
#define ASSERT_GREATER_UINT64(Actual, Expected)                                \
  _ASSERT_GREATER_(Actual, Expected, uint64_t, "%" PRIu64)
#define ASSERT_GREATER_FLOAT(Actual, Expected)                                 \
  _ASSERT_GREATER_(Actual, Expected, float, "%f")
#define ASSERT_GREATER_DOUBLE(Actual, Expected)                                \
  _ASSERT_GREATER_(Actual, Expected, double, "%lf")

#define TEST_PREFIX_(a, b) a##__##b
#define TEST_PREFIX(a, b) TEST_PREFIX_(a, b)
#define TEST(testName)                                                         \
  void TEST_PREFIX(TEST_FILE_PREFIX, testName)(TestEntry * _Test_Data_)

#endif // TEST_FRAMEWORK_H
