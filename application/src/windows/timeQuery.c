#ifdef _WIN32
#include "timeQuery.h"

#include <profileapi.h>

void initializeTimeQuery(TimeQuery* TimeQuery) {
  QueryPerformanceFrequency(&TimeQuery->Frequency);
  QueryPerformanceCounter(&TimeQuery->Start);
}

float getElapsedTime(const TimeQuery* const TimeQuery) {
  LARGE_INTEGER end;
  QueryPerformanceCounter(&end);

  return (float)(end.QuadPart - TimeQuery->Start.QuadPart) /
         (float)TimeQuery->Frequency.QuadPart;
}

#endif // _WIN32
