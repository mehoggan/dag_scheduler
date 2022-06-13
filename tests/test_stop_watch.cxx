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
      public LoggedClass<TestStopWatch>
    {
    public:
      TestStopWatch() :
        LoggedClass<TestStopWatch>(*this)
      {}

    protected:
      virtual void SetUp() {}

      virtual void TearDown() {}
    };

    TEST_F(TestStopWatch, output_validate)
    {
      StopWatch sw(LogTag(__FUNCTION__), "no-op-0");
      sw.mark();
      sw = StopWatch(LogTag(__FUNCTION__), "no-op-1", false);
      sw.mark();
      // Expect no output because we are using the copy ctor which is a reset.
      sw = StopWatch(LogTag(__FUNCTION__), "no-op-2");
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
