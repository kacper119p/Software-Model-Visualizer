#include "timeQuery.h"

void initializeTimeQuery(TimeQuery* TimeQuery) {
  clock_gettime(CLOCK_MONOTONIC, &TimeQuery->Start);
}

float getElapsedTime(const TimeQuery* const TimeQuery) {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);

  return (float)(now.tv_sec - TimeQuery->Start.tv_sec) +
         (float)(now.tv_nsec - TimeQuery->Start.tv_nsec) * 1e-9f;
}
