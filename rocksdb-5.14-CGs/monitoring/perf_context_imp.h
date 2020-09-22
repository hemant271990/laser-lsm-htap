//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
#pragma once
#include "monitoring/perf_step_timer.h"
#include "rocksdb/perf_context.h"
#include "util/stop_watch.h"

namespace rocksdb {
#if defined(NPERF_CONTEXT) || !defined(ROCKSDB_SUPPORT_THREAD_LOCAL)
extern PerfContext perf_context;
#else
#if defined(OS_SOLARIS)
extern __thread PerfContext perf_context_;
#define perf_context (*get_perf_context())
#else
extern __thread PerfContext perf_context;
#endif
#endif

#if defined(NPERF_CONTEXT)

#define PERF_TIMER_GUARD(metric)
#define PERF_CONDITIONAL_TIMER_FOR_MUTEX_GUARD(metric, condition)
#define PERF_TIMER_MEASURE(metric)
#define PERF_TIMER_STOP(metric)
#define PERF_TIMER_START(metric)
#define PERF_COUNTER_ADD(metric, value)

#else

// Stop the timer and update the metric
#define PERF_TIMER_STOP(metric) perf_step_timer_##metric.Stop();

#define PERF_TIMER_START(metric) perf_step_timer_##metric.Start();

// Declare and set start time of the timer
#define PERF_TIMER_GUARD(metric)                                  \
  PerfStepTimer perf_step_timer_##metric(&(perf_context.metric)); \
  perf_step_timer_##metric.Start();

#define PERF_CONDITIONAL_TIMER_FOR_MUTEX_GUARD(metric, condition, stats,      \
                                               ticker_type)                   \
  PerfStepTimer perf_step_timer_##metric(&(perf_context.metric), true, stats, \
                                         ticker_type);                        \
  if (condition) {                                                            \
    perf_step_timer_##metric.Start();                                         \
  }

// Update metric with time elapsed since last START. start time is reset
// to current timestamp.
#define PERF_TIMER_MEASURE(metric) perf_step_timer_##metric.Measure();

// Increase metric value
#define PERF_COUNTER_ADD(metric, value)        \
  if (perf_level >= PerfLevel::kEnableCount) { \
    perf_context.metric += value;              \
  }

// Increase metric value
#define PERF_COUNTER_GET_ADD( level, value)        \
  if (perf_level >= PerfLevel::kEnableCount) {  \
    if(level == 0) \
      perf_context.h2saxena_get_at_level_0 += value;              \
    if(level == 1) \
      perf_context.h2saxena_get_at_level_1 += value;              \
    if(level == 2) \
      perf_context.h2saxena_get_at_level_2 += value;              \
    if(level == 3) \
      perf_context.h2saxena_get_at_level_3 += value;              \
    if(level == 4) \
      perf_context.h2saxena_get_at_level_4 += value;              \
    if(level == 5) \
      perf_context.h2saxena_get_at_level_5 += value;              \
    if(level == 6) \
      perf_context.h2saxena_get_at_level_6 += value;              \
    if(level == 7) \
      perf_context.h2saxena_get_at_level_7 += value;              \
  }
#endif

}
