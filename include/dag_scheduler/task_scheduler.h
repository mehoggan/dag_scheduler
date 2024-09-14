#ifndef TASK_SCHEDULER_H_INCLUDED
#define TASK_SCHEDULER_H_INCLUDED

#include "dag_scheduler/concurrent_task_queue.h"
#include "dag_scheduler/interruptible_task_thread.h"
#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"

#include <gtest/gtest_prod.h>

#include <array>
#include <memory>
#include <mutex>
#include <tuple>

namespace com {
  namespace dag_scheduler {
    class TaskScheduler :
      public LoggedClass<TaskScheduler>,
      public boost::noncopyable {
    public:
      /**
       * @brief
       */
      TaskScheduler();

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
      void queue_task(std::unique_ptr<Task> &&t);

      /**
       * @brief
       *
       * @param t
       *
       * @return
       */
      bool kill_task(const Task &t);

      /**
       * @brief
       *
       * @param u
       *
       * @return
       */
      bool kill_task(const UUID &u);

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
      std::size_t first_unused_thread();

    private:
      ConcurrentTaskQueue queue_;
      volatile std::atomic_bool pause_;
      volatile std::atomic_bool kill_;
      std::array<std::unique_ptr<InterruptibleTaskThread>, 10> thread_pool_;
      std::mutex thread_pool_lock_;
    };
  } // namespace dag_scheduler
} // namespace com

#endif
