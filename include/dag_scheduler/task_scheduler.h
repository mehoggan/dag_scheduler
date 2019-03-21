#ifndef TASK_SCHEDULER_H_INCLUDED
#define TASK_SCHEDULER_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/concurrent_task_queue.h"
#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"

#include <gtest/gtest_prod.h>

#include <tuple>

namespace com
{
  namespace dag_scheduler
  {
    class DLLSPEC_DAGTASKS task_scheduler :
      public logged_class<task_scheduler>,
      public boost::noncopyable
    {
    public:
      /**
       * @brief 
       */
      task_scheduler();

      /**
       * @brief 
       *
       * @return 
       */
      bool startup();

      /**
       * @brief 
       *
       * @param t
       */
      void queue_task(std::unique_ptr<task> &&t);

      /**
       * @brief 
       *
       * @param t
       *
       * @return 
       */
      bool kill_task(const task &t);

      /**
       * @brief 
       */
      void pause();

      /**
       * @brief 
       */
      void resume();

      /**
       * @brief
       *
       * @return 
       */
      bool is_paused();

      /**
       * @brief 
       */
      void shutdown();

      /**
       * @brief 
       *
       * @return 
       */
      bool is_shutdown();

    private:
      concurrent_task_queue queue_;
      volatile std::atomic_bool pause_;
      volatile std::atomic_bool kill_;
    };
  }
}

#endif
