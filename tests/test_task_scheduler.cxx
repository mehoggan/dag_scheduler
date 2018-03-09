#include <gtest/gtest.h>

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"
#include "dag_scheduler/task_scheduler.h"

#include "utils/test_task.h"

#include <atomic>

namespace com
{
  namespace dag_scheduler
  {
    namespace detail
    {
      class LocalTestTaskImpl :
        public TestTaskImpl
      {
      public:
        LocalTestTaskImpl(const std::chrono::milliseconds &sleep_time) :
          TestTaskImpl(),
          ran_(false),
          sleep_time_(sleep_time)
        {}

        virtual bool run() override
        {
          ran_.store(true);

          std::size_t sleep_time = sleep_time_.count() / 1000;
          logging::info(LOG_TAG, "Sleeping for", sleep_time, "seconds.");
          std::this_thread::sleep_for(sleep_time_);
          logging::info(LOG_TAG, "Done sleeping for", sleep_time,
            "seconds.");

          return ran_.load();
        }

        bool was_ran() const
        {
          return ran_.load();
        }

      private:
        std::atomic_bool ran_;
        std::chrono::milliseconds sleep_time_;
      };

      class TestInterruptibleThread :
        public ::testing::Test,
        public logged_class<TestInterruptibleThread>
      {
      public:
        TestInterruptibleThread() :
          logged_class<TestInterruptibleThread>(*this)
        {}

      protected:
        virtual void SetUp() {}
        virtual void TearDown() {}
      };

      TEST_F(TestInterruptibleThread, default_ctor_no_task)
      {
        interruptible_thread it;
        EXPECT_FALSE(it.has_task());
        it.get_task([&](std::unique_ptr<task> &u) {
            EXPECT_EQ(nullptr, u.get());
          });
        EXPECT_FALSE(it.check_for_interrupt());
        it.set_interrupt();
        EXPECT_TRUE(it.check_for_interrupt());
        it.get_thread_object().join();
        EXPECT_FALSE(it.get_thread_object().joinable());
      }

      TEST_F(TestInterruptibleThread, default_ctor_with_task)
      {
        interruptible_thread it;
        EXPECT_FALSE(it.has_task());
        it.get_task([&](std::unique_ptr<task> &u) {
            EXPECT_EQ(nullptr, u.get());
          });
        // We sleep in run for N seconds this should hold true.
        std::unique_ptr<task> ltti(new LocalTestTaskImpl(
          std::chrono::milliseconds(5000)));
        it.set_task(ltti);
        it.get_task([&](std::unique_ptr<task> &u) {
            EXPECT_NE(nullptr, u.get());
          });

        EXPECT_FALSE(it.check_for_interrupt());
        it.set_interrupt();
        EXPECT_TRUE(it.check_for_interrupt());
        it.get_thread_object().join();
        EXPECT_FALSE(it.get_thread_object().joinable());
      }

      class TestThreadPool :
        public ::testing::Test,
        public logged_class<TestThreadPool>
      {
      public:
        TestThreadPool() :
          logged_class<TestThreadPool>(*this)
        {}

      protected:
        virtual void SetUp() {}
        virtual void TearDown() {}
      };

      TEST_F(TestThreadPool, start_up_kill_no_tasks)
      {
        thread_pool tp;
        std::thread thread_pool_thread([&]() {
            tp.start_up();
          });

        EXPECT_FALSE(tp.has_tasks());
        EXPECT_EQ(0u, tp.current_task_count());

        tp.join();
        thread_pool_thread.join();
      }

      TEST_F(TestThreadPool, start_up_kill_task_count)
      {
        thread_pool tp;
        std::thread thread_pool_thread([&]() {
            tp.start_up();
          });

        for (auto i : {0, 1, 2}) {
          std::unique_ptr<task> ltti(new LocalTestTaskImpl(
            std::chrono::milliseconds(5000)));
          //tp.add_task(ltti);
          (void)i;
        }

        tp.join();
        thread_pool_thread.join();
      }
    }
  }
}
