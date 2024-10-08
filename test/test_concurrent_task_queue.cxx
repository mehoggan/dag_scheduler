#include <chrono>
#include <gtest/gtest.h>

#include "dag_scheduler/concurrent_task_queue.h"
#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"
#include "dag_scheduler/uuid.h"

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

namespace com {
  namespace dag_scheduler {
    class TestConcurrentTaskQueue :
      public ::testing::Test,
      public LoggedClass<TestConcurrentTaskQueue> {
    public:
      TestConcurrentTaskQueue() :
        LoggedClass<TestConcurrentTaskQueue>(*this) {}

    protected:
      virtual void SetUp() {}

      virtual void TearDown() {}
    };

    TEST_F(TestConcurrentTaskQueue, test_size_when_empty) {
      ConcurrentTaskQueue empty_queue;
      EXPECT_EQ(0u, empty_queue.size());
    }

    TEST_F(TestConcurrentTaskQueue, test_size_predictable_conccurent) {
      ConcurrentTaskQueue non_empty_queue;
      std::unique_ptr<Task> task_ptr(new Task);
      non_empty_queue.push(std::move(task_ptr));
      EXPECT_EQ(1u, non_empty_queue.size());

      std::thread t([&]() {
        std::unique_ptr<Task> task_ptr(new Task);
        non_empty_queue.push(std::move(task_ptr));
      });

      t.join();
      EXPECT_EQ(2u, non_empty_queue.size());
    }

    TEST_F(TestConcurrentTaskQueue, test_push_try_pop_and_data) {
      ConcurrentTaskQueue queue;

      std::vector<std::string> uuids;
      std::thread t([&]() {
        for (auto i : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}) {
          std::vector<std::unique_ptr<TaskStage>> stages;
          std::unique_ptr<Task> task_ptr(new Task(stages, std::to_string(i)));
          uuids.push_back(task_ptr->get_uuid().as_string());
          queue.push(std::move(task_ptr));
        }
      });

      t.join();
      std::unique_ptr<Task> j;
      ASSERT_TRUE(queue.try_pop(j));
      EXPECT_EQ(uuids[0], j->get_uuid().as_string());
    }

    TEST_F(TestConcurrentTaskQueue, test_push_wait_and_pop_and_data) {
      ConcurrentTaskQueue queue;
      std::condition_variable signal_condition;
      std::mutex signal_block;
      std::unique_lock<std::mutex> signal_lock(signal_block);

      std::vector<std::string> uuids;
      std::thread t([&]() {
        for (auto i : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}) {
          std::vector<std::unique_ptr<TaskStage>> stages;
          std::unique_ptr<Task> task_ptr(new Task(stages, std::to_string(i)));
          ASSERT_TRUE(task_ptr.get() != nullptr);
          auto uuid_str = task_ptr->get_uuid().as_string();
          uuids.push_back(uuid_str);
          queue.push(std::move(task_ptr));
        }
        std::this_thread::sleep_for(std::chrono::seconds(4));
        signal_lock.unlock();
        signal_condition.notify_one();
      });

      std::unique_ptr<Task> task_ptr;
      bool found = queue.wait_for_and_pop(task_ptr, std::chrono::seconds(2));

      signal_condition.wait(signal_lock);
      ASSERT_TRUE(task_ptr != nullptr);
      ASSERT_TRUE(found);
      EXPECT_EQ(uuids[0], task_ptr->get_uuid().as_string());

      t.join();
    }

    TEST_F(TestConcurrentTaskQueue, test_empty) {
      ConcurrentTaskQueue queue;
      EXPECT_TRUE(queue.empty());
      std::unique_ptr<Task> task_ptr(new Task);
      queue.push(std::move(task_ptr));
      EXPECT_FALSE(queue.empty());
      queue.clear();
      EXPECT_TRUE(queue.empty());
    }

    TEST_F(TestConcurrentTaskQueue, test_remove_task_from_queue) {
      ConcurrentTaskQueue queue;
      std::unique_ptr<Task> task_ptr(new Task);
      const UUID &ref = task_ptr->get_uuid();
      queue.push(std::move(task_ptr));
      std::unique_ptr<Task> removed;
      queue.remove_task_from_queue(ref, removed);
      ASSERT_TRUE(removed != nullptr);
      EXPECT_EQ(removed->get_uuid().as_string(), ref.as_string());
      EXPECT_TRUE(queue.empty());
    }

    TEST_F(
      TestConcurrentTaskQueue, test_remove_task_from_queue_remaining_ok
    ) {
      ConcurrentTaskQueue queue;

      std::unique_ptr<Task> task_ptr(new Task);
      const UUID &ref = task_ptr->get_uuid();
      queue.push(std::move(task_ptr));

      std::unique_ptr<Task> task_ptr_remains(new Task);
      const UUID &ref_remains = task_ptr_remains->get_uuid();
      queue.push(std::move(task_ptr_remains));

      std::unique_ptr<Task> removed;
      queue.remove_task_from_queue(ref, removed);
      ASSERT_TRUE(removed != nullptr);
      EXPECT_EQ(removed->get_uuid().as_string(), ref.as_string());
      EXPECT_FALSE(queue.empty());

      std::unique_ptr<Task> remains_check;
      queue.try_pop(remains_check);
      EXPECT_EQ(
        ref_remains.as_string(), remains_check->get_uuid().as_string()
      );
    }
  } // namespace dag_scheduler
} // namespace com
