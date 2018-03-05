#include <gtest/gtest.h>

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"
#include "dag_scheduler/task_scheduler.h"

#include <atomic>

namespace com
{
  namespace dag_scheduler
  {
    class TestTaskScheduler :
      public ::testing::Test,
      public logged_class<TestTaskScheduler>
    {
    public:
      TestTaskScheduler() :
        logged_class<TestTaskScheduler>(*this)
      {}

    protected:
      virtual void SetUp() {}
      virtual void TearDown() {}
    };

    TEST_F(TestTaskScheduler, default_ctor)
    {
    }
  }
}
