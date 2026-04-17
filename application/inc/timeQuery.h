#ifndef SOFTWARE_MODEL_VISUALIZER_TIMEQUERY_H
#define SOFTWARE_MODEL_VISUALIZER_TIMEQUERY_H
#ifdef _WIN32
#include <profileapi.h>
#elif __linux__
#include <time.h>
#endif

struct TimeQuery {
#ifdef _WIN32
  LARGE_INTEGER Frequency;
  LARGE_INTEGER Start;
#elif __linux__
  struct timespec Start;
#endif
};

void initializeTimeQuery(struct TimeQuery* TimeQuery);
float getElapsedTime(const struct TimeQuery* TimeQuery);

#endif // SOFTWARE_MODEL_VISUALIZER_TIMEQUERY_H
