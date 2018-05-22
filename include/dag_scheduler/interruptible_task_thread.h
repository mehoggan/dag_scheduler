#ifndef INTERRUPTIBLE_TASK_THREAD_H_INCLUDED
#define INTERRUPTIBLE_TASK_THREAD_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"

namespace com
{
  namespace dag_scheduler
  {
    class DLLSPEC_DAGTASKS interruptible_task_thread :
      public logged_class<interruptible_task_thread>
    {
    public:
      interruptible_task_thread();

      ~interruptible_task_thread();

      void set_interrupt();

      bool was_interrupted() const;

      const std::unique_ptr<task> &get_task() const;

    private:
      std::thread thread_;
      volatile std::atomic_bool interrupt_;
      std::unique_ptr<task> task_;
    };
  }
}

#endif
