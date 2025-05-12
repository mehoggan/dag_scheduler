////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include "dag_scheduler/LoggedClass.hpp"
#include "dag_scheduler/StopWatch.h"

namespace com::dag_scheduler {
class TestStopWatch : public ::testing::Test,
                      public LoggedClass<TestStopWatch> {
public:
    TestStopWatch() : LoggedClass<TestStopWatch>(*this) {}

protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(TestStopWatch, outputValidate) {
    StopWatch stop_watch(LogTag(__FUNCTION__), "no-op-0");
    stop_watch.mark();
    stop_watch = StopWatch(LogTag(__FUNCTION__), "no-op-1", false);
    stop_watch.mark();
    // Expect no output because we are using the copy ctor which is a reset.
    stop_watch = StopWatch(LogTag(__FUNCTION__), "no-op-2");
    stop_watch.mark();
    stop_watch.start();
    stop_watch.mark();
    stop_watch.mark();
    stop_watch.mark();
    stop_watch.mark();
    stop_watch.mark();
    stop_watch.mark();
    stop_watch.stop();
}
}  // namespace com::dag_scheduler
