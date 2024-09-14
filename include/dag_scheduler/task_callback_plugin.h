#ifndef TASK_CALLBACK_PLUGIN_H_INCLUDED
#define TASK_CALLBACK_PLUGIN_H_INCLUDED

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"

namespace com {
  namespace dag_scheduler {
    class TaskCallbackPlugin {
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

      /**
       * @brief Needed by the cloning of a \ref DAG. Your task callback plugin
       * must implement this method.
       *
       * Needed by the cloning of a \ref DAG. Your task callback plugin
       * must implement this method.
       *
       * @return A cloned version of (*this).
       */
      virtual std::unique_ptr<TaskCallbackPlugin> clone() const;
    };
  } // namespace dag_scheduler
} // namespace com

#endif
