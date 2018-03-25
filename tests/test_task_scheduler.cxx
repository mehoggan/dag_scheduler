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
        explicit LocalTestTaskImpl(
          const std::chrono::milliseconds &sleep_time) :
          ran_(false),
          sleep_time_(sleep_time)
        {}

        bool run() override
        {
          ran_.store(true);

          const long int sleep_time = sleep_time_.count() / 1000;
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
    }

    TEST(TestTaskScheduler, default_ctor)
    {
      task_scheduler ts;
      EXPECT_TRUE(ts.is_paused());
      EXPECT_TRUE(ts.is_shutdown()) << "User must call startup";
    }

    TEST(TestTaskScheduler, startup)
    {
      task_scheduler ts;

      std::thread ts_thread([&]() {
        ASSERT_TRUE(ts.startup());
      });

      const std::chrono::milliseconds millis(3);
      std::this_thread::sleep_for(millis);

      EXPECT_FALSE(ts.is_paused());
      EXPECT_FALSE(ts.is_shutdown()) << "User must call startup";

      ts.shutdown();

      EXPECT_TRUE(ts.is_paused());
      EXPECT_TRUE(ts.is_shutdown()) << "User must call startup";

      ts_thread.join();
    }

    TEST(TestTaskScheduler, pause_resume)
    {
      task_scheduler ts;

      std::thread ts_thread([&]() {
        ASSERT_TRUE(ts.startup());
      });

      for (auto i : {0u, 1u, 2u, 3u, 4u, 5u}) {
        (void)i;

        ts.pause();
        EXPECT_TRUE(ts.is_paused());

        ts.resume();
        EXPECT_FALSE(ts.is_paused());
      }
      ts.shutdown();

      ts_thread.join();
    }

    TEST(TestTaskScheduler, queue_task)
    {
      task_scheduler ts;

      std::unique_ptr<task> ltti(new detail::LocalTestTaskImpl(
        std::chrono::milliseconds(3)));
      ts.queue_task(std::move(ltti));
      ASSERT_EQ(nullptr, ltti.get());
    }

    TEST(TestTaskScheduler, kill_task)
    {
      std::unique_ptr<task> ltti(new detail::LocalTestTaskImpl(
        std::chrono::milliseconds(3)));
      task_scheduler ts;
      EXPECT_TRUE(ts.kill_task(*(ltti)));
    }
  }
}
