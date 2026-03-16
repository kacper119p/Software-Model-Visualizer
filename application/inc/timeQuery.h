#ifndef SOFTWARE_MODEL_VISUALIZER_TIMEQUERY_H
#define SOFTWARE_MODEL_VISUALIZER_TIMEQUERY_H

#include <profileapi.h>

typedef struct {
  LARGE_INTEGER Frequency;
  LARGE_INTEGER Start;
} TimeQuery;

void initializeTimeQuery(TimeQuery* TimeQuery);
float getElapsedTime(const TimeQuery* TimeQuery);

#endif // SOFTWARE_MODEL_VISUALIZER_TIMEQUERY_H
