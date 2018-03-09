#include "dag_scheduler/task_scheduler.h"

#include "dag_scheduler/logging.h"

#include <vector>

namespace com
{
  namespace dag_scheduler
  {
    namespace detail
    {
      interruptible_thread::interruptible_thread() :
        logged_class<interruptible_thread>(*this),
        interrupt_check_(false),
        t_(nullptr)
      {
        const std::chrono::milliseconds wait_duration(2000);
        thread_ = std::thread([&] {
          while (!check_for_interrupt()) {
            std::unique_lock<std::mutex> lock(mutex_);
            if(cv_.wait_for(lock, wait_duration, [&]() {
                return t_ != nullptr;
              })) {
              if (t_ != nullptr) {
                try {
                  {
                    std::unique_lock<std::mutex> ulock(task_lock_);
                    logging::info(LOG_TAG, "Going to run task",
                      t_->label());
                    t_->run();
                  }
                } catch (std::exception &e) {
                  if (t_ != nullptr) {
                    {
                      std::unique_lock<std::mutex> ulock(task_lock_);
                      t_->kill();
                      t_->cleanup();
                      logging::fatal(LOG_TAG, "Task", t_->label(),
                        "terminated with an exception of", e.what());
                        t_.reset();
                    }
                  }
                }
              }
            }
          }
        });
      }

      bool interruptible_thread::set_task(std::unique_ptr<task> &t)
      {
        std::atomic_bool ret(false);

        if (task_lock_.try_lock()) {
          t_ = std::move(t);
          ret.store(true);
          task_lock_.unlock();
        }

        return ret.load();
      }

      bool interruptible_thread::has_task() const
      {
        std::atomic_bool ret(false);
        if (task_lock_.try_lock()) {
          ret.store(t_ != nullptr);
          task_lock_.unlock();
        }

        return ret.load();
      }

      void interruptible_thread::get_task(
        const std::function<void (std::unique_ptr<task> &)> &cb)
      {
        std::unique_lock<std::mutex> lock(task_lock_);
        cb(t_);
      }

      bool interruptible_thread::check_for_interrupt()
      {
        return interrupt_check_.load();
      }

      void interruptible_thread::set_interrupt()
      {
        interrupt_check_.store(true);
        std::unique_lock<std::mutex> lock(task_lock_);
        if (t_ != nullptr) {
          t_->kill();
        }
      }

      std::thread &interruptible_thread::get_thread_object()
      {
        return thread_;
      }

      thread_pool::thread_pool() :
        logged_class<thread_pool>(*this)
      {}

      thread_pool::~thread_pool()
      {}

      void thread_pool::start_up(std::size_t size)
      {
        pool_.resize(size);
      }

      void thread_pool::join()
      {
        std::for_each(pool_.begin(), pool_.end(),
            [&](std::unique_ptr<interruptible_thread> &thread) {
              if (thread->get_thread_object().joinable()) {
                thread->get_thread_object().join();
              }
          });
      }

      bool thread_pool::kill()
      {
        bool ret = true;

        std::unique_lock<std::mutex> lock(pool_lock_);
        std::for_each(pool_.begin(), pool_.end(),
          [&](std::unique_ptr<interruptible_thread> &thread) {
            thread->set_interrupt();
          });

        return ret;
      }

      bool thread_pool::add_task(std::unique_ptr<task> &t)
      {
        std::atomic_bool ret(false);
        std::unique_lock<std::mutex> lock(pool_lock_);
        auto it = std::find_if(pool_.begin(), pool_.end(),
          [&](std::unique_ptr<interruptible_thread> &u) {
            return !u->has_task();
          });
        if (it != pool_.end()) {
          ret.store(true);
          (*it)->set_task(t);
        }
        return ret;
      }

      bool thread_pool::has_tasks() const
      {
        std::atomic_bool ret(false);
        std::unique_lock<std::mutex> lock(pool_lock_);
        std::for_each(pool_.cbegin(), pool_.cend(),
          [&](const std::unique_ptr<interruptible_thread> &u) {
            if (u->has_task()) {
              ret = true;
            }
          });

        return ret;
      }

      std::size_t thread_pool::current_task_count() const
      {
        std::atomic<std::size_t> ret(0);

        std::unique_lock<std::mutex> lock(pool_lock_);
        std::for_each(pool_.cbegin(), pool_.cend(),
          [&](const std::unique_ptr<interruptible_thread> &u) {
            if (u->has_task()) {
              ret.store(ret.load() + 1u);
            }
          });

        return ret;
      }

      bool thread_pool::kill_task(const task &t)
      {
        std::atomic_bool ret(false);
        std::unique_lock<std::mutex> lock(pool_lock_);
        std::for_each(pool_.begin(), pool_.end(),
          [&](std::unique_ptr<interruptible_thread> &u) {

            std::function<void (std::unique_ptr<task> &)> cb =
              [&](std::unique_ptr<task> &t_check) {
                if (t_check && t_check->get_uuid() == t.get_uuid()) {
                  ret.store(t_check->kill());
                }
            };
            u->get_task(cb);
          });

        return ret;
      }
    }

    task_scheduler::task_scheduler() :
      logged_class<task_scheduler>(*this),
      pause_(false),
      kill_(true)
    {}

    bool task_scheduler::startup()
    {
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
                    bool task_added = pool_.add_task(next_task);
                    if (task_added) {
                      break;
                    }
                  }
                }
              }
            }
          }
        });
      runner.join();

      return kill_.load();
    }

    bool task_scheduler::queue_task(std::unique_ptr<task> &&t)
    {
      bool ret = false;

      queue_.push(std::move(t));

      return ret;
    }

    bool task_scheduler::kill_task(const task &t)
    {
      return pool_.kill_task(t);
    }

    void task_scheduler::pause()
    {
      pause_.store(true);
    }

    void task_scheduler::shutdown()
    {
      pause();
      kill_.store(true);
    }
  }
}
