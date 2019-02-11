#include <gtest/gtest.h>

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"
#include "dag_scheduler/task_scheduler.h"

#include "utils/test_task.h"

#include <atomic>
#include <thread>

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

        bool was_ran() const
        {
          return ran_.load();
        }

        LocalTestTaskImpl(LocalTestTaskImpl &&other) :
          TestTaskImpl(std::move(other))
        {}

        LocalTestTaskImpl &operator=(LocalTestTaskImpl &&other)
        {
          TestTaskImpl::operator=(std::move(other));
          ran_.store(false);
          sleep_time_ = std::move(other.sleep_time_);
          return (*this);
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

      std::chrono::milliseconds give_ts_thread_time(500);
      std::this_thread::sleep_for(give_ts_thread_time);

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
      std::thread ts_thread([&]() {ASSERT_TRUE(ts.startup());});
      std::chrono::milliseconds give_ts_thread_time(500);
      std::this_thread::sleep_for(give_ts_thread_time);
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

    TEST(TestTaskScheduler, queue_task_and_let_it_run)
    {
      task_scheduler ts;
      std::thread ts_thread([&]() {ASSERT_TRUE(ts.startup());});
      // Give time for thread to start up before we fire and kill.
      std::unique_ptr<task> ltti(new detail::LocalTestTaskImpl(
        std::chrono::milliseconds(1000)));

      ts.queue_task(std::move(ltti));

      // From a unit test perspective we need to give the task time to
      // be scheduled and then run. One second should be sufficient.
      auto wait_time_sec = std::chrono::seconds(1);
      std::this_thread::sleep_for(wait_time_sec);

      ts.shutdown();
      ts_thread.join();
    }
  }
}
