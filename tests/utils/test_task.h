#ifndef TEST_TASK_H_INCLUDED
#define TEST_TASK_H_INCLUDED

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"

#include <gtest/gtest_prod.h>

#include <atomic>

namespace com
{
  namespace dag_scheduler
  {
    class TestTaskImpl : public task
    {
    private:
      FRIEND_TEST(TestTask,
        iterate_stages_succeeds_if_all_stages_ran_no_kill_task);

    public:
      TestTaskImpl();

      TestTaskImpl(const std::string &label);

      virtual ~TestTaskImpl();

      TestTaskImpl(TestTaskImpl &&other);

      TestTaskImpl &operator=(TestTaskImpl &&other);
    };
  }
}
#endif
