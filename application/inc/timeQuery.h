/* MIT License
 *
 * Copyright (c) 2026 Kacper Pastuszka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
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
