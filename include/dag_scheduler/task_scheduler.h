#ifndef TASK_SCHEDULER_H_INCLUDED
#define TASK_SCHEDULER_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/concurrent_queue.hpp"
#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"

#include <gtest/gtest_prod.h>

namespace com
{
  namespace dag_scheduler
  {
    class DLLSPEC_DAGTASKS task_scheduler :
      public logged_class<task_scheduler>,
      public boost::noncopyable
    {
    public:
      task_scheduler();

      bool startup();

      void queue_task(std::unique_ptr<task> &&t);

      bool kill_task(const task &t);

      void pause();

      void resume();

      bool is_paused();

      void shutdown();

      bool is_shutdown();

    private:
      concurrent_queue<std::unique_ptr<task>> queue_;
      std::atomic_bool pause_;
      std::atomic_bool kill_;
    };
  }
}

#endif
