#include <gtest/gtest.h>

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"

namespace com
{
  namespace dag_scheduler
  {
    class test_task :
      public task,
      public logged_class<test_task>
    {
    public:
      test_task() :
        task(),
        logged_class<test_task>(*this)
      {}

      test_task(const std::string &label) :
        task(label),
        logged_class<test_task>(*this)
      {}

      virtual ~test_task()
      {}

      virtual bool run()
      {
        return true;
      }

      virtual bool kill()
      {
        return true;
      }

      virtual std::unique_ptr<task> clone()
      {
        return std::unique_ptr<task>(new test_task(*this));
      }

    private:
      test_task(const test_task &other) :
        task(),
        logged_class<test_task>(*this)
      {
        uuid_ = const_cast<uuid &>(other.get_uuid()).clone();
        label_ = other.label();
      }
    };

    class TestTask :
      public ::testing::Test,
      public logged_class<TestTask>
    {
    public:
      TestTask() :
        logged_class<TestTask>(*this)
      {}

    protected:
      virtual void SetUp() {}
      virtual void TearDown() {}
    };

    TEST_F(TestTask, default_ctor)
    {
      std::unique_ptr<task> test(new test_task());
      EXPECT_NE(nullptr, test);
      EXPECT_EQ(test->get_uuid().as_string(), test->label());
    }

    TEST_F(TestTask, label_ctor)
    {
      std::unique_ptr<task> test(new test_task("test_label"));
      EXPECT_NE(nullptr, test);
      EXPECT_NE(test->get_uuid().as_string(), test->label());
      EXPECT_EQ("test_label", test->label());
    }

    TEST_F(TestTask, run)
    {
      test_task tt;
      EXPECT_TRUE(tt.run());

      std::unique_ptr<task> tt_ptr(new test_task);
      EXPECT_TRUE(tt_ptr->run());
    }
  }
}
