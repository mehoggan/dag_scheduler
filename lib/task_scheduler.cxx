#include "dag_scheduler/task_scheduler.h"

#include "dag_scheduler/logging.h"
#include "dag_scheduler/logged_class.hpp"

namespace com
{
  namespace dag_scheduler
  {
    namespace detail
    {
      class thread_pool;

      class thread_pool_thread
      {
      private:
        friend class thread_pool;

      public:
        thread_pool_thread()
        {}

      private:
        std::condition_variable condition_;
        std::mutex mutex_;
      };

      class thread_pool :
        public logged_class<thread_pool>
      {
      public:
        explicit thread_pool(std::size_t size) :
          logged_class<thread_pool>(*this)
        {}
      };
    }

    task_scheduler::task_scheduler() :
      pause_(false),
      kill_(true)
    {}

    bool task_scheduler::startup()
    {
      kill_.store(false);
      auto refresh_time = std::chrono::milliseconds(5);
      std::thread runner([&]() {
          while (!kill_.load() && !pause_.load()) {
            std::unique_ptr<task> next_task = nullptr;
            while (queue_.wait_for_and_pop(next_task, refresh_time));
            if (next_task != nullptr) {
              // TODO (mhoggan): Add to thread pool.
            }
          }
        });
      runner.join();

      return kill_.load();
    }

    bool task_scheduler::queue_task(const task &t)
    {
      bool ret = false;

      return ret;
    }

    bool task_scheduler::kill_task(const task &t)
    {
      bool ret = false;

      return ret;
    }

    bool task_scheduler::kill_task_by_uuid(const uuid &u)
    {
      bool ret = false;

      return ret;
    }

    bool task_scheduler::pause()
    {
      bool ret = false;

      return ret;
    }

    bool task_scheduler::shutdown()
    {
      bool ret = false;

      return ret;
    }
  }
}
