#include <gtest/gtest.h>

#include "dag_scheduler/task_scheduler.h"

#include <iostream>
#include <memory>

namespace com
{
  namespace dag_scheduler
  {
    class TestDagScheduler : public ::testing::Test
    {
    protected:
      virtual void SetUp() {}
      virtual void TearDown() {}
    };
  }
}
