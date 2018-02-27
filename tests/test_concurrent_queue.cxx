#include <gtest/gtest.h>

#include "dag_scheduler/concurrent_queue.hpp"

#include <iostream>

namespace com
{
  namespace dag_scheduler
  {
    class TestConcurrentQueue : public ::testing::Test
    {
    protected:
      virtual void SetUp() {}

      virtual void TearDown() {}
    };

    TEST_F(TestConcurrentQueue, test_size_when_empty)
    {
      concurrent_queue<int> empty_queue;
      EXPECT_EQ(0u, empty_queue.size());
    }


    TEST_F(TestConcurrentQueue, test_size_predictable_conccurent)
    {
      concurrent_queue<int> non_empty_queue;
      non_empty_queue.push(1);
      EXPECT_EQ(1u, non_empty_queue.size());

      std::thread t([&]() {
          non_empty_queue.push(2);
        });

      t.join();
      EXPECT_EQ(2u, non_empty_queue.size());
    }

    TEST_F(TestConcurrentQueue, test_push_try_pop_and_data)
    {
      concurrent_queue<int> queue;

      std::thread t([&]() {
          for (auto i : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}) {
            queue.push(i);
          }
        });

      t.join();
      int j = -1;
      ASSERT_TRUE(queue.try_pop(j));
      EXPECT_EQ(0, j) << queue << std::endl;
    }

    TEST_F(TestConcurrentQueue, test_push_wait_and_pop_and_data)
    {
      concurrent_queue<int> queue;

      std::thread t([&]() {
          for (auto i : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}) {
            queue.push(i);
          }
        });

      int j = -1;
      bool found = queue.wait_for_and_pop(j, std::chrono::milliseconds(5));
      ASSERT_TRUE(found);
      EXPECT_EQ(0, j) << queue << std::endl;

      t.join();
    }
  }
}
