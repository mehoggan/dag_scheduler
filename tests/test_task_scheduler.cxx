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
        class LocalTestTaskImpl :
          public TestTaskImpl
        {
        public:
          LocalTestTaskImpl() :
            TestTaskImpl(),
            ran_(false)
          {}

          virtual bool run() override
          {
            ran_.store(true);
            const std::size_t sec = 5;
            logging::info(LOG_TAG, "Sleeping for", sec, "seconds.");
            std::this_thread::sleep_for(std::chrono::seconds(sec));
            logging::info(LOG_TAG, "Done sleeping for", sec, "seconds.");
            return ran_.load();
          }

          bool was_ran() const
          {
            return ran_.load();
          }

        private:
          std::atomic_bool ran_;
        };

        interruptible_thread it;
        EXPECT_FALSE(it.has_task());
        it.get_task([&](std::unique_ptr<task> &u) {
            EXPECT_EQ(nullptr, u.get());
          });
        // We sleep in run for N seconds this should hold true.
        std::unique_ptr<task> ltti(new LocalTestTaskImpl);
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
    }
  }
}
