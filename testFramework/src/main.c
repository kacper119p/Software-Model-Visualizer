#include "testFramework.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#endif

#include "ansiColors.h"

extern TestEntry tests[];
extern const size_t testCount;

#if defined(_WIN32)
static inline void enableAnsi(void) {
  HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD mode;
  if (!GetConsoleMode(h, &mode))
    return;
  SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif

int main(void) {
#if defined(_WIN32)
  enableAnsi();
#endif

  size_t successCount = 0;
  size_t failedCount = 0;

  const char* currentFile = "\0";
  for (size_t i = 0; i < testCount; i++) {
    if (strcmp(currentFile, tests[i].FileName) != 0) {
      currentFile = tests[i].FileName;
      printf("[ " ANSI_CYAN_BOLD "RUNNING FILE" ANSI_RESET " ] %s\n",
             tests[i].FileName);
    }
    printf("[ " ANSI_CYAN "RUN" ANSI_RESET " ] %s\n", tests[i].Name);
    tests[i].Function(&tests[i]);
    if (tests[i].TestResult == TestResult_Failure) {
      printf("[ " ANSI_RED_INTENSE "FAILED" ANSI_RESET " ] %s\n",
             tests[i].Name);
      ++failedCount;
    } else {
      printf("[ " ANSI_GREEN_INTENSE "PASSED" ANSI_RESET " ] %s\n",
             tests[i].Name);
      ++successCount;
    }
  }

  printf("[ " ANSI_CYAN_BOLD "FINISHED" ANSI_RESET " ]\n");
  printf("Tests run: %zu\n", successCount + failedCount);
  if (failedCount == 0) {
    printf(ANSI_GREEN_INTENSE "All Passed" ANSI_RESET);
  } else {
    printf("Tests Passed : " ANSI_GREEN_INTENSE "%zu" ANSI_RESET "\n",
           successCount);
    printf("Tests Failed : " ANSI_RED "%zu" ANSI_RESET "\n", failedCount);
  }

  return failedCount > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
