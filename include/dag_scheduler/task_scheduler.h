#ifndef TASK_SCHEDULER_H_INCLUDED
#define TASK_SCHEDULER_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/concurrent_queue.hpp"
#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"

namespace com
{
  namespace dag_scheduler
  {
    namespace detail
    {
      class task_scheduler;

      class interruptible_thread :
        public logged_class<task_scheduler>
      {
      public:
        interruptible_thread();

        bool set_task(std::unique_ptr<task> &t);

        bool has_task();

        void get_task(std::function<void (std::unique_ptr<task> &)> &cb);

        bool check_for_interrupt();

        void set_interrupt();

      private:
        std::atomic_bool interrupt_check_;
        std::thread thread_;
        std::mutex mutex_;
        std::condition_variable cv_;
        std::mutex task_lock_;
        std::unique_ptr<task> t_;
      };

      class thread_pool :
        public logged_class<thread_pool>,
        boost::noncopyable
      {
      public:
        explicit thread_pool(std::size_t size = 10);

        ~thread_pool();

        bool add_task(std::unique_ptr<task> &t);

        bool kill_task(const task &t);

      private:
        std::vector<std::unique_ptr<interruptible_thread>> pool_;
        std::mutex pool_lock_;
      };
    }

    class DLLSPEC_DAGTASKS task_scheduler :
      public logged_class<task_scheduler>,
      public boost::noncopyable
    {
    public:
      task_scheduler();

      bool startup();

      bool queue_task(std::unique_ptr<task> &&t);

      bool kill_task(const task &t);

      void pause();

      void shutdown();

    private:
      concurrent_queue<std::unique_ptr<task>> queue_;
      std::atomic_bool pause_;
      std::atomic_bool kill_;
      detail::thread_pool pool_;
    };
  }
}

#endif
