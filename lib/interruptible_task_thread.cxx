#include "dag_scheduler/interruptible_task_thread.h"

#include "dag_scheduler/logging.h"

#include <vector>

namespace com
{
  namespace dag_scheduler
  {
    interruptible_task_thread::interruptible_task_thread() :
      logged_class<interruptible_task_thread>(*this),
      interrupt_(false)
    {
      thread_ = std::thread([&]() {
          while (!was_interrupted()) {
          }
        });
    }

    interruptible_task_thread::~interruptible_task_thread()
    {
      set_interrupt();
      if (thread_.joinable()) {
        thread_.join();
      }
    }

    void interruptible_task_thread::set_interrupt()
    {
      interrupt_.store(true);
    }

    bool interruptible_task_thread::was_interrupted() const
    {
      return interrupt_.load();
    }

    const std::unique_ptr<task> &interruptible_task_thread::get_task() const
    {
      return task_;
    }
  }
}
