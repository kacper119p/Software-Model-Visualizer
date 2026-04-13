#ifndef SOFTWARE_MODEL_VISUALIZER_TIMEQUERY_H
#define SOFTWARE_MODEL_VISUALIZER_TIMEQUERY_H
#ifdef _WIN32
#include <profileapi.h>
#elif __linux__
#include <time.h>
#endif

#ifdef _WIN32
typedef struct TimeQuery {
  LARGE_INTEGER Frequency;
  LARGE_INTEGER Start;
} TimeQuery;
#elif __linux__
typedef struct TimeQuery {
  struct timespec Start;
} TimeQuery;
#endif

void initializeTimeQuery(TimeQuery* TimeQuery);
float getElapsedTime(const TimeQuery* TimeQuery);

#endif // SOFTWARE_MODEL_VISUALIZER_TIMEQUERY_H
