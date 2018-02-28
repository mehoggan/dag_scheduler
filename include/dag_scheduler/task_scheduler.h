#ifndef TASK_SCHEDULER_H_INCLUDED
#define TASK_SCHEDULER_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/concurrent_queue.hpp"

namespace com
{
  namespace dag_scheduler
  {
    class DLLSPEC_DAGTASKS task_scheduler
    {
    public:
      bool startup();
      bool queue_task(const task &t);
      bool kill_task(const task &t);
      bool kill_task_by_uuid(const uuid &u);
    };
  }
}

#endif
