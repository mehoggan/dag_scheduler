#include "dag_scheduler/interruptible_task_thread.h"

#include "dag_scheduler/logging.h"

#include <condition_variable>
#include <vector>

namespace com
{
  namespace dag_scheduler
  {
    /* Tested [✓] */
    interruptible_task_thread::interruptible_task_thread() :
      interruptible_task_thread(log_tag(__FUNCTION__))
    {}

    /* Tested [✓] */
    interruptible_task_thread::interruptible_task_thread(const log_tag &tag) :
      LOG_TAG(tag),
      interrupt_(false),
      running_(false)
    {
      logging::add_std_cout_logger(tag);
      logging::add_std_cerr_logger(tag);
    }

    /* Tested [✓] */
    interruptible_task_thread::~interruptible_task_thread()
    {
      shutdown();
    }

    /* Tested [X] */
    interruptible_task_thread::interruptible_task_thread(
      interruptible_task_thread &&other) :
      LOG_TAG(std::move(other.LOG_TAG)),
      interrupt_(false),
      running_(false)
    {
      other.shutdown();
      task_ = std::move(other.task_);
    }

    /* Tested [X] */
    interruptible_task_thread &interruptible_task_thread::operator=(
      interruptible_task_thread &&rhs)
    {
      LOG_TAG = std::move(rhs.LOG_TAG);
      interrupt_.store(false);
      running_.store(false);

      rhs.shutdown();

      task_ = std::move(rhs.task_);
      return (*this);
    }

    /* Tested [X] */
    bool interruptible_task_thread::set_task_and_run(
      std::unique_ptr<task> &&task,
      const std::function<void (bool status)> &complete_callback)
    {
      bool thread_started = false;
      std::mutex return_mutex;
      std::unique_lock<std::mutex> return_lock(return_mutex);
      std::condition_variable return_cond;

      if (not running_.load()) {
        thread_ = std::thread([&]() {
          thread_started = true;
          running_.store(true);
          logging::info(LOG_TAG, "Initializing thread with interrupt set to",
            (interrupt_.load() ? "true" : "false"));
          {
            std::lock_guard<std::mutex> lock(task_lock_);
            task_ = std::move(task);
          }

          /*
          * We wait for task to be moved into then we return to consumers.
          * We do this to ensure that client can query state of object and
          * get correct state.
          */
          return_cond.notify_one();

          while (not was_interrupted()) {
            /*
            * TODO (mhoggan): Tasks should have steps. Otherwise we
            * will not be able to run long running tasks that are
            * interruptible.
            *
            * For example this could read.
            *
            * std::all_of(task.steps.begin, task.steps.end,
            *   [&](step &next) {
            *     step.execute();
            *     return (!was_interrupted());
            *   });
            */ 
            {
              std::lock_guard<std::mutex> lock(task_lock_);
              bool task_run = (task_ != nullptr) ? task_->run() : false;
              (void)task_run;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
          }

          {
            std::lock_guard<std::mutex> lock(task_lock_);
            if (task_) {
              task_->cleanup();
            }
          }

          running_.store(false);
          {
            std::lock_guard<std::mutex> lock(task_lock_);
            task_.reset(nullptr);
          }
          complete_callback(true /* TODO (mhoggan): Use task status. */);
        });
      } else {
        logging::error(LOG_TAG, "Task already running. Call",
          "\'set_interrupt\' before trying to start a new task.");
      }

      return_cond.wait(return_lock);
      return thread_started;
    }

    /* Tested [✓] */
    void interruptible_task_thread::set_interrupt()
    {
      {
        std::lock_guard<std::mutex> lock(task_lock_);
        bool task_killed = (task_ != nullptr) ? task_->kill() : true;
        (task_ != nullptr) ? task_->cleanup() : (void)task_killed;
      }
      interrupt_.store(true);
    }

    /* Tested [✓] */
    bool interruptible_task_thread::was_interrupted() const
    {
      return interrupt_.load();
    }

    /* Tested [✓] */
    bool interruptible_task_thread::is_running() const
    {
      return running_;
    }

    /* Tested [✓] */
    bool interruptible_task_thread::has_task() const
    {
      {
        std::lock_guard<std::mutex> lock(task_lock_);
        return task_ != nullptr;
      }
    }

    /* Tested [✓] */
    void interruptible_task_thread::shutdown()
    {
      logging::info(LOG_TAG, "Shutdown called.");
      if (running_.load()) {
        set_interrupt();
      }
      if (thread_.joinable()) {
        thread_.join();
      }
    }
  }
}
