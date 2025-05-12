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
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(TestInterruptibleTaskThread, ctor) {
    EXPECT_FALSE(ts_.isRunning());
    EXPECT_FALSE(ts_.wasInterrupted());
    EXPECT_FALSE(ts_.hasTask());
}

TEST_F(TestInterruptibleTaskThread, moveCtorNotRunning) {
    InterruptibleTaskThread task_thread(std::move(ts_));
    EXPECT_FALSE(task_thread.isRunning());
    EXPECT_FALSE(task_thread.wasInterrupted());
    EXPECT_FALSE(task_thread.hasTask());
    EXPECT_FALSE(ts_.isRunning());
    EXPECT_FALSE(ts_.wasInterrupted());
    EXPECT_FALSE(ts_.hasTask());
}

TEST_F(TestInterruptibleTaskThread, moveAssignmentNotRunning) {
    InterruptibleTaskThread task_thread;
    task_thread = std::move(ts_);
    EXPECT_FALSE(task_thread.isRunning());
    EXPECT_FALSE(task_thread.wasInterrupted());
    EXPECT_FALSE(task_thread.hasTask());
    EXPECT_FALSE(ts_.isRunning());
    EXPECT_FALSE(ts_.wasInterrupted());
    EXPECT_FALSE(ts_.hasTask());
}

TEST_F(TestInterruptibleTaskThread, setTaskAndRunSuccessful) {
    std::condition_variable exit_cond;
    auto complete_callback = [&](bool status) {
        EXPECT_TRUE(status);
        EXPECT_FALSE(ts_.isRunning());
        EXPECT_FALSE(ts_.wasInterrupted());
        EXPECT_FALSE(ts_.hasTask());
        exit_cond.notify_one();
    };
    std::mutex exit_mutex;
    std::unique_lock<std::mutex> exit_lock(exit_mutex);
    std::unique_ptr<TestTaskImpl> test_task(
            new TestTaskImpl("test_task", complete_callback));
    bool started = ts_.setTaskAndRun(std::move(test_task));
    EXPECT_TRUE(started);
    EXPECT_TRUE(ts_.hasTask());
    EXPECT_TRUE(ts_.isRunning());
    exit_cond.wait(exit_lock);
    ts_.shutdown();
}

TEST_F(TestInterruptibleTaskThread, setTaskAndRunWithInterrupt) {
    auto complete_callback = [&](bool status) {
        EXPECT_FALSE(status);
        EXPECT_FALSE(ts_.isRunning());
        EXPECT_TRUE(ts_.wasInterrupted());
        EXPECT_FALSE(ts_.hasTask());
    };
    std::unique_ptr<TestTaskImpl> test_task(
            new TestTaskImpl("test_task", complete_callback));
    bool started = ts_.setTaskAndRun(std::move(test_task));
    EXPECT_TRUE(started);
    ts_.setInterrupt();
    ts_.shutdown();
}
}  // namespace com::dag_scheduler
