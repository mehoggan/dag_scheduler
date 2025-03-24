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

#include <condition_variable>

#include "dag_scheduler/InterruptibleTaskThread.h"
#include "dag_scheduler/LoggedClass.hpp"
#include "dag_scheduler/Logging.h"
#include "utils/TestTask.h"

namespace com::dag_scheduler {
class TestInterruptibleTaskThread
        : public ::testing::Test,
          public LoggedClass<TestInterruptibleTaskThread> {
public:
    TestInterruptibleTaskThread()
            : LoggedClass<TestInterruptibleTaskThread>(*this), ts_(LOG_TAG) {}

protected:
    InterruptibleTaskThread ts_;

protected:
    virtual void SetUp() {}

    virtual void TearDown() {}
};

TEST_F(TestInterruptibleTaskThread, ctor) {
    EXPECT_FALSE(ts_.is_running());
    EXPECT_FALSE(ts_.was_interrupted());
    EXPECT_FALSE(ts_.has_task());
}

TEST_F(TestInterruptibleTaskThread, move_ctor_not_running) {
    InterruptibleTaskThread ts(std::move(ts_));
    EXPECT_FALSE(ts.is_running());
    EXPECT_FALSE(ts.was_interrupted());
    EXPECT_FALSE(ts.has_task());
    EXPECT_FALSE(ts_.is_running());
    EXPECT_FALSE(ts_.was_interrupted());
    EXPECT_FALSE(ts_.has_task());
}

TEST_F(TestInterruptibleTaskThread, move_assignement_not_running) {
    InterruptibleTaskThread ts;
    ts = std::move(ts_);
    EXPECT_FALSE(ts.is_running());
    EXPECT_FALSE(ts.was_interrupted());
    EXPECT_FALSE(ts.has_task());
    EXPECT_FALSE(ts_.is_running());
    EXPECT_FALSE(ts_.was_interrupted());
    EXPECT_FALSE(ts_.has_task());
}

TEST_F(TestInterruptibleTaskThread, set_task_and_run_succesfull) {
    std::condition_variable exit_cond;
    auto complete_callback = [&](bool status) {
        EXPECT_TRUE(status);
        EXPECT_FALSE(ts_.is_running());
        EXPECT_FALSE(ts_.was_interrupted());
        EXPECT_FALSE(ts_.has_task());
        exit_cond.notify_one();
    };
    std::mutex exit_mutex;
    std::unique_lock<std::mutex> exit_lock(exit_mutex);
    std::unique_ptr<TestTaskImpl> test_task(
            new TestTaskImpl("test_task", complete_callback));
    bool started = ts_.set_task_and_run(std::move(test_task));
    EXPECT_TRUE(started);
    EXPECT_TRUE(ts_.has_task());
    EXPECT_TRUE(ts_.is_running());
    exit_cond.wait(exit_lock);
    ts_.shutdown();
}

TEST_F(TestInterruptibleTaskThread, set_task_and_run_with_interrupt) {
    auto complete_callback = [&](bool status) {
        EXPECT_FALSE(status);
        EXPECT_FALSE(ts_.is_running());
        EXPECT_TRUE(ts_.was_interrupted());
        EXPECT_FALSE(ts_.has_task());
    };
    std::unique_ptr<TestTaskImpl> test_task(
            new TestTaskImpl("test_task", complete_callback));
    bool started = ts_.set_task_and_run(std::move(test_task));
    EXPECT_TRUE(started);
    ts_.set_interrupt();
    ts_.shutdown();
}
}  // namespace com::dag_scheduler
