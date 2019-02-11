#include <gtest/gtest.h>

#include "dag_scheduler/interruptible_task_thread.h"
#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/logging.h"

#include "utils/test_task.h"

namespace com
{
  namespace dag_scheduler
  {
    class TestInterruptibleTaskThread :
      public ::testing::Test,
      public logged_class<TestInterruptibleTaskThread>
    {
    public:
      TestInterruptibleTaskThread() :
        logged_class<TestInterruptibleTaskThread>(*this),
        ts_(LOG_TAG)
      {}

    protected:
      interruptible_task_thread ts_;

    protected:
      virtual void SetUp() {}

      virtual void TearDown() {}
    };

    TEST_F(TestInterruptibleTaskThread, ctor)
    {
      EXPECT_FALSE(ts_.is_running());
      EXPECT_FALSE(ts_.was_interrupted());
      EXPECT_FALSE(ts_.has_task());
    }

    TEST_F(TestInterruptibleTaskThread, move_ctor_not_running)
    {
      interruptible_task_thread ts(std::move(ts_));
      EXPECT_FALSE(ts.is_running());
      EXPECT_FALSE(ts.was_interrupted());
      EXPECT_FALSE(ts.has_task());
      EXPECT_FALSE(ts_.is_running());
      EXPECT_FALSE(ts_.was_interrupted());
      EXPECT_FALSE(ts_.has_task());
    }

    TEST_F(TestInterruptibleTaskThread, move_assignement_not_running)
    {
      interruptible_task_thread ts;
      ts = std::move(ts_);
      EXPECT_FALSE(ts.is_running());
      EXPECT_FALSE(ts.was_interrupted());
      EXPECT_FALSE(ts.has_task());
      EXPECT_FALSE(ts_.is_running());
      EXPECT_FALSE(ts_.was_interrupted());
      EXPECT_FALSE(ts_.has_task());
    }

    TEST_F(TestInterruptibleTaskThread, set_task_and_run_succesfull)
    {
      std::unique_ptr<TestTaskImpl> test_task(new TestTaskImpl("test_task"));
      bool started = ts_.set_task_and_run(std::move(test_task),
        [&](bool status) {
          EXPECT_TRUE(status);
          EXPECT_FALSE(ts_.is_running());
          EXPECT_FALSE(ts_.was_interrupted());
          EXPECT_FALSE(ts_.has_task());
        });
      EXPECT_TRUE(started);
      EXPECT_TRUE(ts_.has_task());
      EXPECT_TRUE(ts_.is_running());

      while (ts_.is_running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }

      ts_.shutdown();
    }

    TEST_F(TestInterruptibleTaskThread, set_task_and_run_with_interrupt_fails)
    {
      std::unique_ptr<TestTaskImpl> test_task(new TestTaskImpl("test_task"));
      ts_.set_task_and_run(std::move(test_task),
        [&](bool status) {
          EXPECT_FALSE(status);
          EXPECT_FALSE(ts_.is_running());
          EXPECT_TRUE(ts_.was_interrupted());
          EXPECT_FALSE(ts_.has_task());
        });
      ts_.set_interrupt();
      ts_.shutdown();
    }
  }
}
