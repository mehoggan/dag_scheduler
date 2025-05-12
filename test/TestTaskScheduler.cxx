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

#include <atomic>
#include <thread>

#include "dag_scheduler/Task.h"
#include "dag_scheduler/TaskScheduler.h"
#include "utils/TestTask.h"

namespace com::dag_scheduler {
namespace detail {
class LocalTestTaskImpl : public TestTaskImpl {
public:
    explicit LocalTestTaskImpl(const std::chrono::milliseconds& sleep_time,
                               std::function<void(bool)> complete_callback_)
            : TestTaskImpl(__FUNCTION__, complete_callback_)
            , ran_(false)
            , sleep_time_(sleep_time) {}

    bool wasRan() const { return ran_.load(); }

    LocalTestTaskImpl(LocalTestTaskImpl&& other)
            : TestTaskImpl(std::move(other)) {}

    LocalTestTaskImpl& operator=(LocalTestTaskImpl&& other) {
        TestTaskImpl::operator=(std::move(other));
        ran_.store(false);
        sleep_time_ = std::move(other.sleep_time_);  // NOLINT
        return (*this);
    }

private:
    std::atomic_bool ran_;
    std::chrono::milliseconds sleep_time_;
};
}  // namespace detail

TEST(TestTaskScheduler, defaultCtor) {
    TaskScheduler task_scheduler;
    EXPECT_TRUE(task_scheduler.isPaused());
    EXPECT_TRUE(task_scheduler.isShutdown()) << "User must call startup";
}

TEST(TestTaskScheduler, startup) {
    TaskScheduler task_scheduler;

    std::thread ts_thread([&]() { ASSERT_TRUE(task_scheduler.startup()); });

    std::chrono::milliseconds give_ts_thread_time(500);
    std::this_thread::sleep_for(give_ts_thread_time);

    EXPECT_FALSE(task_scheduler.isPaused());
    EXPECT_FALSE(task_scheduler.isShutdown()) << "User must call startup";

    task_scheduler.shutdown();

    EXPECT_TRUE(task_scheduler.isPaused());
    EXPECT_TRUE(task_scheduler.isShutdown()) << "User must call startup";

    ts_thread.join();
}

TEST(TestTaskScheduler, pauseResume) {
    TaskScheduler task_scheduler;
    std::thread ts_thread([&]() { ASSERT_TRUE(task_scheduler.startup()); });
    std::chrono::milliseconds give_ts_thread_time(500);
    std::this_thread::sleep_for(give_ts_thread_time);
    for (auto index : {0u, 1u, 2u, 3u, 4u, 5u}) {
        (void)index;
        task_scheduler.pause();
        EXPECT_TRUE(task_scheduler.isPaused());
        task_scheduler.resume();
        EXPECT_FALSE(task_scheduler.isPaused());
    }
    task_scheduler.shutdown();
    ts_thread.join();
}

TEST(TestTaskScheduler, queueTask) {
    TaskScheduler task_scheduler;

    std::unique_ptr<Task> local_task(new detail::LocalTestTaskImpl(
            std::chrono::milliseconds(3), std::function<void(bool)>()));
    task_scheduler.queueTask(std::move(local_task));
    ASSERT_EQ(nullptr, local_task.get());
}

TEST(TestTaskScheduler, killTask) {
    std::unique_ptr<Task> local_task(new detail::LocalTestTaskImpl(
            std::chrono::milliseconds(3), std::function<void(bool)>()));
    TaskScheduler task_scheduler;
    EXPECT_TRUE(task_scheduler.killTask(*(local_task)));
}

TEST(TestTaskScheduler, queueTaskAndLetItRun) {
    std::function<void(bool)> complete_callback = [&](bool status) {
        EXPECT_TRUE(status);
    };

    TaskScheduler task_scheduler;
    std::thread ts_thread([&]() { ASSERT_TRUE(task_scheduler.startup()); });
    // Give time for thread to start up before we fire and kill.
    std::unique_ptr<Task> local_task(new detail::LocalTestTaskImpl(
            std::chrono::milliseconds(1000), complete_callback));

    task_scheduler.queueTask(std::move(local_task));

    // From a unit test perspective we need to give the task time to
    // be scheduled and then run. One second should be sufficient.
    std::this_thread::sleep_for(std::chrono::seconds(1));

    task_scheduler.shutdown();
    ts_thread.join();
}
}  // namespace com::dag_scheduler
