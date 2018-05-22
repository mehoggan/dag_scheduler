#include <gtest/gtest.h>

#include "dag_scheduler/interruptible_task_thread.h"
#include "dag_scheduler/logged_class.hpp"

#include "utils/test_task.h"

namespace com
{
  namespace dag_scheduler
  {
    TEST(TestInterruptibleTaskThread, default_ctor_and_interruptible)
    {
      interruptible_task_thread ts;
      std::chrono::milliseconds wait_time(500);
      std::this_thread::sleep_for(wait_time);
      EXPECT_FALSE(ts.was_interrupted());
      EXPECT_EQ(nullptr, ts.get_task());
      ts.set_interrupt();
      EXPECT_TRUE(ts.was_interrupted());
      EXPECT_EQ(nullptr, ts.get_task());
    }
  }
}
