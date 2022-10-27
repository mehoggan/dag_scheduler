#include <gtest/gtest.h>

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task_stage.h"

#include "utils/test_task_stage.h"

#include <atomic>

namespace com
{
  namespace dag_scheduler
  {
    class TestTaskStage :
      public ::testing::Test,
      public LoggedClass<TestTaskStage>
    {
    public:
      TestTaskStage() :
        LoggedClass<TestTaskStage>(*this)
      {}

    protected:
      virtual void SetUp() {}
      virtual void TearDown() {}
    };

    TEST_F(TestTaskStage, default_ctor)
    {
      std::unique_ptr<TaskStage> test(new TestTaskStageImpl);
      EXPECT_NE(nullptr, test);
      EXPECT_EQ(test->get_uuid().as_string(), test->label());
    }

    TEST_F(TestTaskStage, label_ctor)
    {
      std::unique_ptr<TaskStage> test(new TestTaskStageImpl("test_label"));
      EXPECT_NE(nullptr, test);
      EXPECT_NE(test->get_uuid().as_string(), test->label());
      EXPECT_EQ("test_label", test->label());
    }

    TEST_F(TestTaskStage, run)
    {
      TestTaskStageImpl tt;
      EXPECT_TRUE(tt.run());

      std::unique_ptr<TaskStage> tt_ptr(new TestTaskStageImpl);
      EXPECT_TRUE(tt_ptr->run());
    }

    TEST_F(TestTaskStage, kill_if_is_running)
    {
      TestTaskStageImpl tt;
      EXPECT_TRUE(tt.end());

      std::unique_ptr<TaskStage> tt_ptr(new TestTaskStageImpl);
      EXPECT_TRUE(tt_ptr->end());
      EXPECT_FALSE(tt_ptr->is_running());
      EXPECT_TRUE(tt_ptr->run());
      EXPECT_FALSE(tt_ptr->is_running());
      EXPECT_TRUE(tt_ptr->end());
    }
  }
}
