#ifndef TASK_CALLBACK_PLUGIN_H_INCLUDED
#define TASK_CALLBACK_PLUGIN_H_INCLUDED

#include "dag_scheduler/task.h"

namespace com
{
  namespace dag_scheduler
  {
    class TaskCallbackPlugin
    {
    public:
      /**
       * @brief virtual distructor.
       */
      virtual ~TaskCallbackPlugin();

      /**
       * @brief After all possible \ref TaskStages in a \ref Task have been
       * executed, this method is called by the \ref Task itself with a
       * non-constant referecnce \ref completed_task of itself.
       *
       * After all possible \ref TaskStages in a \ref Task have been
       * executed, this method is called by the \ref Task itself with a
       * non-constant referecnce \ref completed_task of itself.
       *
       * @param[in] status Did all stages complete?
       * @param[in] completed_task The task that was completed.
       */
      virtual void completed(bool status, Task &completed_task);
    };
  }
}

#endif
