#include "dag_scheduler/interruptible_task_thread.h"

#include "dag_scheduler/logging.h"

#include <condition_variable>
#include <vector>

/* ✓ */
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

    /* Tested [✓] */
    interruptible_task_thread::interruptible_task_thread(
      interruptible_task_thread &&other) :
      LOG_TAG(std::move(other.LOG_TAG)),
      interrupt_(false),
      running_(false)
    {
      assert(not other.is_running() && "Cannot move a running task.");

      task_ = std::move(other.task_);
    }

    /* Tested [✓] */
    interruptible_task_thread &interruptible_task_thread::operator=(
      interruptible_task_thread &&rhs)
    {
      assert(not rhs.is_running() && "Cannot move a running task.");

      LOG_TAG = std::move(rhs.LOG_TAG);
      interrupt_.store(false);
      running_.store(false);
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

      {
        std::lock_guard<std::mutex> lock(task_lock_);
        task_ = std::move(task);
      }

      thread_ = std::thread([&] {
        thread_started = true;
        running_.store(true);
        /*
         * We wait for task to be moved into then we return to consumers.
         * We do this to ensure that client can query state of object and
         * get correct state.
         */
        return_cond.notify_one();
        bool all_ran = false;
        if (task_ != nullptr) {
          all_ran = task_->iterate_stages([&](task_stage & next) {
            bool stage_status = false;
            {
              std::lock_guard<std::mutex> lock(task_lock_);
              logging::info(LOG_TAG, "Going to run stage", next);
              stage_status = next.run();
              std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            const bool this_was_interrupted = was_interrupted();
            const bool cont = stage_status && (not this_was_interrupted);
            if (cont) {
              logging::info(LOG_TAG, "Ran stage", next);
            } else {
              logging::error(LOG_TAG, "Failed to execute", next);
            }
            return cont;
          });
        }
        task_.reset(nullptr);
        running_.store(false);
        complete_callback(all_ran);

        return all_ran;
      });

      return_cond.wait(return_lock);
      return thread_started;
    }

    /* Tested [X] */
    void interruptible_task_thread::set_interrupt()
    {
      {
        std::lock_guard<std::mutex> lock(task_lock_);
        (task_ != nullptr) ? task_->kill() : true;
      }
      interrupt_.store(true);
    }

    /* Tested [X] */
    bool interruptible_task_thread::was_interrupted() const
    {
      return interrupt_.load();
    }

    /* Tested [X] */
    bool interruptible_task_thread::is_running() const
    {
      return running_;
    }

    /* Tested [X] */
    bool interruptible_task_thread::has_task() const
    {
      {
        std::lock_guard<std::mutex> lock(task_lock_);
        return task_ != nullptr;
      }
    }

    /* Tested [X] */
    void interruptible_task_thread::shutdown()
    {
      if (running_.load()) {
        set_interrupt();
      }
      if (thread_.joinable()) {
        thread_.join();
      }
    }
  }
}
