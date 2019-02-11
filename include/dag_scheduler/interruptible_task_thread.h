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
      interruptible_task_thread();

      explicit interruptible_task_thread(const log_tag &tag);

      ~interruptible_task_thread();

      interruptible_task_thread(const interruptible_task_thread &) = delete;

      interruptible_task_thread &operator=(
        const interruptible_task_thread &) = delete;

      interruptible_task_thread(interruptible_task_thread &&other);

      interruptible_task_thread &operator=(interruptible_task_thread &&rhs);

      bool set_task_and_run(std::unique_ptr<task> &&task,
        const std::function<void (bool status)> &complete_callback,
        const std::chrono::nanoseconds delay_between_stages =
          std::chrono::nanoseconds(1000000));

      void set_interrupt();

      bool was_interrupted() const;

      bool is_running() const;

      bool has_task() const;

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
