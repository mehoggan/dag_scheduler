#include "dag_scheduler/task_scheduler.h"

#include "dag_scheduler/logging.h"

#include <vector>

namespace com
{
  namespace dag_scheduler
  {
    task_scheduler::task_scheduler() :
      logged_class<task_scheduler>(*this),
      pause_(true),
      kill_(true)
    {}

    bool task_scheduler::startup()
    {
      pause_.store(false);
      kill_.store(false);
      auto refresh_time = std::chrono::milliseconds(5);
      std::thread runner([&]() {
          while (!kill_.load()) {
            bool should_get_next = !(kill_.load() || pause_.load());
            if (should_get_next) {
              std::unique_ptr<task> next_task = nullptr;
              queue_.wait_for_and_pop(next_task, refresh_time);
              if (next_task != nullptr) {
                while (true) {
                  if (kill_.load()) {
                    break;
                  }
                  if (!pause_.load()) {
                  }
                }
              }
            }
          }
        });
      runner.join();

      return true;
    }

    void task_scheduler::queue_task(std::unique_ptr<task> &&t)
    {
      queue_.push(std::move(t));
    }

    bool task_scheduler::kill_task(const task &t)
    {
      return true;
    }

    void task_scheduler::pause()
    {
      pause_.store(true);
    }

    void task_scheduler::resume()
    {
      pause_.store(false);
    }

    bool task_scheduler::is_paused()
    {
      return pause_.load();
    }

    void task_scheduler::shutdown()
    {
      pause();
      kill_.store(true);
    }

    bool task_scheduler::is_shutdown()
    {
      return kill_.load();
    }
  }
}
