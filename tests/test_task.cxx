#include <gtest/gtest.h>

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"

#include <atomic>

namespace com
{
  namespace dag_scheduler
  {

    class TestTaskImpl :
      public logged_class<TestTaskImpl>,
      public task
    {
    public:
      TestTaskImpl() :
        logged_class<TestTaskImpl>(*this),
        task(),
        running_(false),
        nasty_user_defined_pointer_(new int(10))
      {}

      TestTaskImpl(const std::string &label) :
        logged_class<TestTaskImpl>(*this),
        task(label),
        running_(false),
        nasty_user_defined_pointer_(new int(10))
      {}

      virtual ~TestTaskImpl()
      {
        cleanup();
      }

      virtual bool run()
      {
        running_.store(true);
        return true;
      }

      virtual bool is_running() const
      {
        return running_.load();
      }

      virtual bool kill()
      {
        bool ret = false;
        if (running_.load()) {
          running_.store(false);
          ret = true;
        }
        return ret;
      }

      virtual void cleanup()
      {
        delete nasty_user_defined_pointer_;
      }

      virtual std::unique_ptr<task> clone()
      {
        return std::unique_ptr<task>(new TestTaskImpl(*this));
      }

    private:
      TestTaskImpl(const TestTaskImpl &other) :
        logged_class<TestTaskImpl>(*this),
        task()
      {
        uuid_ = const_cast<uuid &>(other.get_uuid()).clone();
        label_ = other.label();
        nasty_user_defined_pointer_ =
          new int(*(other.nasty_user_defined_pointer_));
      }

    private:
      std::atomic_bool running_;
      int *nasty_user_defined_pointer_;
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
      std::unique_ptr<task> test(new TestTaskImpl);
      EXPECT_NE(nullptr, test);
      EXPECT_EQ(test->get_uuid().as_string(), test->label());
    }

    TEST_F(TestTask, label_ctor)
    {
      std::unique_ptr<task> test(new TestTaskImpl("test_label"));
      EXPECT_NE(nullptr, test);
      EXPECT_NE(test->get_uuid().as_string(), test->label());
      EXPECT_EQ("test_label", test->label());
    }

    TEST_F(TestTask, run)
    {
      TestTaskImpl tt;
      EXPECT_TRUE(tt.run());

      std::unique_ptr<task> tt_ptr(new TestTaskImpl);
      EXPECT_TRUE(tt_ptr->run());
    }

    TEST_F(TestTask, kill_if_is_running)
    {
      TestTaskImpl tt;
      EXPECT_FALSE(tt.kill());

      std::unique_ptr<task> tt_ptr(new TestTaskImpl);
      EXPECT_FALSE(tt_ptr->kill());
      EXPECT_TRUE(tt_ptr->run());
      EXPECT_TRUE(tt_ptr->is_running());
      EXPECT_TRUE(tt_ptr->kill());
    }

    TEST_F(TestTask, clone)
    {
      TestTaskImpl tt;
      std::unique_ptr<task> tt_clone = tt.clone();
      EXPECT_EQ(tt.get_uuid(), tt_clone->get_uuid());
      EXPECT_EQ(tt.label(), tt_clone->label());
    }
  }
}
