#ifndef TEST_TASK_H_INCLUDED
#define TEST_TASK_H_INCLUDED

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
      TestTaskImpl();

      TestTaskImpl(const std::string &label);

      virtual ~TestTaskImpl();

      virtual bool run();

      virtual bool is_running() const;

      virtual bool kill();

      virtual void cleanup();

      virtual std::unique_ptr<task> clone();

    protected:
      TestTaskImpl(const TestTaskImpl &other);

    private:
      std::atomic_bool running_;
      int *nasty_user_defined_pointer_;
    };
  }
}
#endif
