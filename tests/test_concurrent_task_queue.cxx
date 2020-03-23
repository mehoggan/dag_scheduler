#include <gtest/gtest.h>

#include "dag_scheduler/concurrent_task_queue.h"
#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"
#include "dag_scheduler/uuid.h"

#include <iostream>
#include <string>

namespace com
{
  namespace dag_scheduler
  {
    class TestConcurrentTaskQueue :
      public ::testing::Test,
      public logged_class<TestConcurrentTaskQueue>
    {
    public:
      TestConcurrentTaskQueue() :
        logged_class<TestConcurrentTaskQueue>(*this)
      {}

    protected:
      virtual void SetUp() {}

      virtual void TearDown() {}
    };

    TEST_F(TestConcurrentTaskQueue, test_size_when_empty)
    {
      concurrent_task_queue empty_queue;
      EXPECT_EQ(0u, empty_queue.size());
    }

    TEST_F(TestConcurrentTaskQueue, test_size_predictable_conccurent)
    {
      concurrent_task_queue non_empty_queue;
      std::unique_ptr<task> task_ptr(new task);
      non_empty_queue.push(std::move(task_ptr));
      EXPECT_EQ(1u, non_empty_queue.size());

      std::thread t([&]() {
        std::unique_ptr<task> task_ptr(new task);
        non_empty_queue.push(std::move(task_ptr));
      });

      t.join();
      EXPECT_EQ(2u, non_empty_queue.size());
    }

    TEST_F(TestConcurrentTaskQueue, test_push_try_pop_and_data)
    {
      concurrent_task_queue queue;

      std::vector<std::string> uuids;
      std::thread t([&]() {
        for (auto i : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}) {
          std::vector<std::unique_ptr<task_stage>> stages;
          std::unique_ptr<task> task_ptr(new task(stages, std::to_string(i)));
          uuids.push_back(task_ptr->get_uuid().as_string());
          queue.push(std::move(task_ptr));
        }
      });

      t.join();
      std::unique_ptr<task> j;
      ASSERT_TRUE(queue.try_pop(j));
      EXPECT_EQ(uuids[0], j->get_uuid().as_string());
    }

    TEST_F(TestConcurrentTaskQueue, test_push_wait_and_pop_and_data)
    {
      concurrent_task_queue queue;

      std::vector<std::string> uuids;
      std::thread t([&]() {
        for (auto i : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}) {
          std::vector<std::unique_ptr<task_stage>> stages;
          std::unique_ptr<task> task_ptr(new task(stages, std::to_string(i)));
          auto uuid_str = task_ptr->get_uuid().as_string();
          uuids.push_back(uuid_str);
          queue.push(std::move(task_ptr));
        }
      });

      std::unique_ptr<task> j;
      bool found = queue.wait_for_and_pop(j, std::chrono::seconds(8));
      ASSERT_TRUE(found);
      EXPECT_EQ(uuids[0], j->get_uuid().as_string());

      t.join();
    }

    TEST_F(TestConcurrentTaskQueue, test_empty)
    {
      concurrent_task_queue queue;
      EXPECT_TRUE(queue.empty());
      std::unique_ptr<task> task_ptr(new task);
      queue.push(std::move(task_ptr));
      EXPECT_FALSE(queue.empty());
      queue.clear();
      EXPECT_TRUE(queue.empty());
    }
  }
}
