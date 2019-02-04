#include <gtest/gtest.h>

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/stop_watch.h"

#include <iostream>

namespace com
{
  namespace dag_scheduler
  {
    class TestStopWatch :
      public ::testing::Test,
      public logged_class<TestStopWatch>
    {
    public:
      TestStopWatch() :
        logged_class<TestStopWatch>(*this)
      {}

    protected:
      virtual void SetUp() {}

      virtual void TearDown() {}
    };

    TEST_F(TestStopWatch, output_validate)
    {
      stop_watch sw(log_tag(__FUNCTION__), "no-op-0");
      sw.mark();
      sw = stop_watch(log_tag(__FUNCTION__), "no-op-1", false);
      sw.mark();
      // Expect no output because we are using the copy ctor which is a reset.
      sw = stop_watch(log_tag(__FUNCTION__), "no-op-2");
      sw.mark();
      sw.start();
      sw.mark();
      sw.mark();
      sw.mark();
      sw.mark();
      sw.mark();
      sw.mark();
      sw.stop();
    }
  }
}
