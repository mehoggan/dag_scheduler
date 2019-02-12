#ifndef INTERRUPTIBLE_TASK_THREAD_H_INCLUDED
#define INTERRUPTIBLE_TASK_THREAD_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/logging.h"
#include "dag_scheduler/task.h"

#include <chrono>

namespace com
{
  namespace dag_scheduler
  {
    class DLLSPEC_DAGTASKS interruptible_task_thread
    {
    public:
      /**
       * @brief 
       */
      interruptible_task_thread();

      /**
       * @brief 
       *
       * @param[in] tag
       */
      explicit interruptible_task_thread(const log_tag &tag);

      /**
       * @brief 
       */
      ~interruptible_task_thread();

      interruptible_task_thread(const interruptible_task_thread &) = delete;

      interruptible_task_thread &operator=(
        const interruptible_task_thread &) = delete;

      /**
       * @brief 
       *
       * @param[out] other
       */
      interruptible_task_thread(interruptible_task_thread &&other);

      /**
       * @brief 
       *
       * @param[out] rhs
       *
       * @return 
       */
      interruptible_task_thread &operator=(interruptible_task_thread &&rhs);

      /**
       * @brief 
       *
       * @param[out] task
       * @param[in] complete_callback
       * @param[in] delay_between_stages
       *
       * @return 
       */
      bool set_task_and_run(std::unique_ptr<task> &&task,
        const std::function<void (bool status)> &complete_callback,
        const std::chrono::nanoseconds delay_between_stages =
          std::chrono::nanoseconds(1000000));

      /**
       * @brief 
       */
      void set_interrupt();

      /**
       * @brief 
       *
       * @return 
       */
      bool was_interrupted() const;

      /**
       * @brief 
       *
       * @return 
       */
      bool is_running() const;

      /**
       * @brief 
       *
       * @return 
       */
      bool has_task() const;

      /**
       * @brief 
       */
      void shutdown();

    private:
      std::unique_ptr<task> task_;
      mutable std::mutex task_lock_;
      log_tag LOG_TAG;
      volatile std::atomic_bool interrupt_;
      volatile std::atomic_bool running_;
      std::thread thread_;
    };
  }
}

#endif
