#include <gtest/gtest.h>

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"
#include "dag_scheduler/task_callback_plugin.h"

#include "utils/test_task.h"

#include <atomic>

namespace com
{
  namespace dag_scheduler
  {
    class TestTask :
      public ::testing::Test,
      public LoggedClass<TestTask>
    {
    public:
      TestTask() :
        LoggedClass<TestTask>(*this)
      {}

    protected:
      virtual void SetUp() {}
      virtual void TearDown() {}
    };

    class TestTaskCallbackPlugin :
      public TaskCallbackPlugin,
      public ::testing::Test,
      public LoggedClass<TestTaskCallbackPlugin>
    {
    public:
      TestTaskCallbackPlugin():
        LoggedClass<TestTaskCallbackPlugin>(*this)
      {}

      virtual ~TestTaskCallbackPlugin()
      {}

      virtual void completed(bool completed, Task &task)
      {
        Logging::info(LOG_TAG, "completed called with", (completed ? "true" :
          "false"), "and", task);
      }

      virtual void TestBody()
      {}
    };

    TEST_F(TestTask, default_ctor)
    {
      std::unique_ptr<Task> test(new TestTaskImpl);
      EXPECT_NE(nullptr, test);
      EXPECT_EQ(test->get_uuid().as_string(), test->label());
    }

    TEST_F(TestTask, label_ctor)
    {
      std::unique_ptr<Task> test(new TestTaskImpl("test_label"));
      EXPECT_NE(nullptr, test);
      EXPECT_NE(test->get_uuid().as_string(), test->label());
      EXPECT_EQ("test_label", test->label());
    }

    TEST_F(TestTask, call_back_ctor_sets_appropiate_callback)
    {
      std::function<void (bool)> complete_callback = [](bool status) {
        EXPECT_TRUE(status);
      };
      std::unique_ptr<Task> test(
          new TestTaskImpl("test_label", complete_callback));
      EXPECT_NE(nullptr, test);
      EXPECT_NE(test->get_uuid().as_string(), test->label());
      EXPECT_EQ("test_label", test->label());
      test->complete(true);
    }

    TEST_F(TestTask, call_back_ctor_sets_appropiate_callback_plugin)
    {
      std::unique_ptr<TaskCallbackPlugin> complete_callback_plugin =
        std::make_unique<TestTaskCallbackPlugin>();
      std::unique_ptr<Task> test(
        new TestTaskImpl("test_label", std::move(complete_callback_plugin)));
      EXPECT_NE(nullptr, test);
      EXPECT_NE(test->get_uuid().as_string(), test->label());
      EXPECT_EQ("test_label", test->label());
      EXPECT_TRUE(test->callback_plugin_is_set());
      test->complete(true);
    }

    TEST_F(TestTask, iterate_stages_succeeds_if_all_stages_ran_no_kill_task)
    {
      const auto expected = {"A", "B", "C"};
      TestTaskImpl tt;
      auto expected_it = expected.begin();
      ASSERT_EQ(tt.stages_.size(), expected.size());
      bool ran_all = tt.iterate_stages([&](TaskStage &next) {
        EXPECT_EQ(*(expected_it), next.label());
        bool ran = next.run();
        ++expected_it;
        return ran;
      });
      EXPECT_TRUE(ran_all);
    }

    TEST_F(TestTask, iterate_stages_fails_if_one_stage_failed)
    {
      TestTaskImpl tt;
      bool ran_all = tt.iterate_stages([&](TaskStage &next) {
        bool ran = next.run();
        return (next.label() == "B" ? false : ran);
      });
      EXPECT_FALSE(ran_all);
    }

    TEST_F(TestTask, iterate_stages_fails_if_task_killed)
    {
      TestTaskImpl tt;
      bool ran_all = tt.iterate_stages([&](TaskStage &next) {
        next.end();
        bool keep_running = (next.label() == "C") ? (not tt.kill()) : true;
        return keep_running;
      });
      EXPECT_FALSE(ran_all);
    }

    TEST_F(TestTask, user_cannot_move_task_while_iterating_stages)
    {
      // Test move copy ctor.
      ASSERT_DEATH(
        {
          TestTaskImpl tt;
          tt.iterate_stages([&](TaskStage &next) {
            next.end();
            TestTaskImpl destroy(std::move(tt));
            return true;
          });
        },
        ""
      );

      // Test move assignment operator.
      ASSERT_DEATH(
        {
          TestTaskImpl tt;
          tt.iterate_stages([&](TaskStage &next) {
            next.end();
            TestTaskImpl destroy;
            destroy = std::move(tt);
            return true;
          });
        },
        ""
      );
    }
  }
}
