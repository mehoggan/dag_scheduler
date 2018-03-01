#ifndef TASK_SCHEDULER_H_INCLUDED
#define TASK_SCHEDULER_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/concurrent_queue.hpp"
#include "dag_scheduler/task.h"

namespace com
{
  namespace dag_scheduler
  {
    class DLLSPEC_DAGTASKS task_scheduler
    {
    public:
      task_scheduler();

      bool startup();

      bool queue_task(const task &t);

      bool kill_task(const task &t);

      bool kill_task_by_uuid(const uuid &u);

      bool pause();

      bool shutdown();

    private:
      concurrent_queue<std::unique_ptr<task>> queue_;
      std::atomic_bool pause_;
      std::atomic_bool kill_;
    };
  }
}

#endif
