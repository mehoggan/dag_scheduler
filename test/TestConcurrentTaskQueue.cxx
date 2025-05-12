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

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

#include "dag_scheduler/ConcurrentTaskQueue.h"
#include "dag_scheduler/LoggedClass.hpp"
#include "dag_scheduler/Task.h"
#include "dag_scheduler/Uuid.h"

namespace com::dag_scheduler {
class TestConcurrentTaskQueue : public ::testing::Test,
                                public LoggedClass<TestConcurrentTaskQueue> {
public:
    TestConcurrentTaskQueue() : LoggedClass<TestConcurrentTaskQueue>(*this) {}

protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(TestConcurrentTaskQueue, testSizeWhenEmpty) {
    ConcurrentTaskQueue empty_queue;
    EXPECT_EQ(0u, empty_queue.size());
}

TEST_F(TestConcurrentTaskQueue, testSizePredictableConcurrent) {
    ConcurrentTaskQueue non_empty_queue;
    std::unique_ptr<Task> task_ptr(new Task);
    non_empty_queue.push(std::move(task_ptr));
    EXPECT_EQ(1u, non_empty_queue.size());

    std::thread thread([&]() {
        std::unique_ptr<Task> task_ptr(new Task);
        non_empty_queue.push(std::move(task_ptr));
    });

    thread.join();
    EXPECT_EQ(2u, non_empty_queue.size());
}

TEST_F(TestConcurrentTaskQueue, testPushTryPopAndData) {
    ConcurrentTaskQueue queue;

    std::vector<std::string> uuids;
    std::thread thread([&]() {
        for (auto index : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}) {
            std::vector<std::unique_ptr<TaskStage>> stages;
            std::unique_ptr<Task> task_ptr(
                    new Task(stages, std::to_string(index)));
            uuids.push_back(task_ptr->getUUID().asString());
            queue.push(std::move(task_ptr));
        }
    });

    thread.join();
    std::unique_ptr<Task> task;
    ASSERT_TRUE(queue.tryPop(task));
    EXPECT_EQ(uuids[0], task->getUUID().asString());
}

TEST_F(TestConcurrentTaskQueue, testPushWaitAndPopAndData) {
    ConcurrentTaskQueue queue;
    std::condition_variable signal_condition;
    std::mutex signal_block;
    std::unique_lock<std::mutex> signal_lock(signal_block);

    std::vector<std::string> uuids;
    std::thread thread([&]() {
        for (auto index : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}) {
            std::vector<std::unique_ptr<TaskStage>> stages;
            std::unique_ptr<Task> task_ptr(
                    new Task(stages, std::to_string(index)));
            ASSERT_TRUE(task_ptr.get() != nullptr);
            auto uuid_str = task_ptr->getUUID().asString();
            uuids.push_back(uuid_str);
            queue.push(std::move(task_ptr));
        }
        std::this_thread::sleep_for(std::chrono::seconds(4));
        signal_lock.unlock();
        signal_condition.notify_one();
    });

    std::unique_ptr<Task> task_ptr;
    bool found = queue.waitForAndPop(task_ptr, std::chrono::seconds(2));

    signal_condition.wait(signal_lock);
    ASSERT_TRUE(task_ptr != nullptr);
    ASSERT_TRUE(found);
    EXPECT_EQ(uuids[0], task_ptr->getUUID().asString());

    thread.join();
}

TEST_F(TestConcurrentTaskQueue, testEmpty) {
    ConcurrentTaskQueue queue;
    EXPECT_TRUE(queue.empty());
    std::unique_ptr<Task> task_ptr(new Task);
    queue.push(std::move(task_ptr));
    EXPECT_FALSE(queue.empty());
    queue.clear();
    EXPECT_TRUE(queue.empty());
}

TEST_F(TestConcurrentTaskQueue, testRemoveTaskFromQueue) {
    ConcurrentTaskQueue queue;
    std::unique_ptr<Task> task_ptr(new Task);
    const UUID& uuid_ref = task_ptr->getUUID();
    queue.push(std::move(task_ptr));
    std::unique_ptr<Task> removed;
    queue.removeTaskFromQueue(uuid_ref, removed);
    ASSERT_TRUE(removed != nullptr);
    EXPECT_EQ(removed->getUUID().asString(), uuid_ref.asString());
    EXPECT_TRUE(queue.empty());
}

TEST_F(TestConcurrentTaskQueue, testRemoveTaskFromQueueRemainingOk) {
    ConcurrentTaskQueue queue;

    std::unique_ptr<Task> task_ptr(new Task);
    const UUID& uuid_ref = task_ptr->getUUID();
    queue.push(std::move(task_ptr));

    std::unique_ptr<Task> task_ptr_remains(new Task);
    const UUID& ref_remains = task_ptr_remains->getUUID();
    queue.push(std::move(task_ptr_remains));

    std::unique_ptr<Task> removed;
    queue.removeTaskFromQueue(uuid_ref, removed);
    ASSERT_TRUE(removed != nullptr);
    EXPECT_EQ(removed->getUUID().asString(), uuid_ref.asString());
    EXPECT_FALSE(queue.empty());

    std::unique_ptr<Task> remains_check;
    queue.tryPop(remains_check);
    EXPECT_EQ(ref_remains.asString(), remains_check->getUUID().asString());
}
}  // namespace com::dag_scheduler
