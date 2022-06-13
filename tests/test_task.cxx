#include <gtest/gtest.h>

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"

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

    // TODO (mehoggan) enable these.
    TEST_F(TestTask, user_cannot_move_task_while_iterating_stages)
    {
      // ASSERT_DEATH(
      //   {
      //     TestTaskImpl tt;
      //     tt.iterate_stages([&](TaskStage &next) {
      //       next.end();
      //       TestTaskImpl destroy(std::move(tt));
      //       return true;
      //     });
      //   },
      //   ""
      // );

      // ASSERT_DEATH(
      //   {
      //     TestTaskImpl tt;
      //     tt.iterate_stages([&](TaskStage &next) {
      //       next.end();
      //       TestTaskImpl destroy;
      //       destroy = std::move(tt);
      //       return true;
      //     });
      //   },
      //   ""
      // );
    }
  }
}
