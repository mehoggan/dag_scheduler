#include "dag_scheduler/stop_watch.h"

namespace com
{
  namespace dag_scheduler
  {
    stop_watch::stop_watch(const log_tag &tag, const std::string &memo,
      bool start_on_construction) :
      LOG_TAG(tag),
      memo_(memo),
      was_stopped_(true),
      was_started_(false),
      start_(std::chrono::high_resolution_clock::now())
    {
      logging::add_std_cout_logger(LOG_TAG);

      if (start_on_construction) {
        start();
      }
    }

    stop_watch::~stop_watch()
    {
      was_stopped_.store(true);
      was_started_.store(false);
    }

    stop_watch::stop_watch(const stop_watch &other) :
      LOG_TAG(other.LOG_TAG),
      memo_(other.memo_),
      was_stopped_(true),
      was_started_(false)
    {
      start();
    }

    stop_watch &stop_watch::operator=(const stop_watch &rhs)
    {
      LOG_TAG = rhs.LOG_TAG;
      memo_ = rhs.memo_;
      was_stopped_.store(true);
      was_started_.store(false);

      start();

      return (*this);
    }

    stop_watch::stop_watch(stop_watch &&other) :
      LOG_TAG(std::move(other.LOG_TAG)),
      memo_(std::move(other.memo_)),
      was_stopped_(true),
      was_started_(false)
    {
      start();
    }

    stop_watch &stop_watch::operator=(stop_watch &&rhs)
    {
      LOG_TAG = std::move(rhs.LOG_TAG);
      memo_ = std::move(rhs.memo_);
      was_stopped_.store(true);
      was_started_.store(false);

      start();

      return (*this);
    }

    void stop_watch::start()
    {
      if (was_stopped_.load()) {
        was_stopped_.store(false);
        was_started_.store(true);
        start_ = std::chrono::high_resolution_clock::now();
      }
    }

    std::chrono::nanoseconds stop_watch::stop()
    {
      if (was_started_.load()) {
        auto now = std::chrono::high_resolution_clock::now();
        return print(now);
        was_stopped_.store(true);
        was_started_.store(false);
      } else {
        return print(start_);
      }
    }

    std::chrono::nanoseconds stop_watch::mark()
    {
      if (was_started_.load()) {
        auto now = std::chrono::high_resolution_clock::now();
        return print(now);
      } else {
        return print(start_);
      }
    }

    void stop_watch::reset()
    {
      start_ = std::chrono::high_resolution_clock::now();
    }

    std::chrono::nanoseconds stop_watch::print(
      const std::chrono::high_resolution_clock::time_point &stop)
    {
      std::chrono::duration<unsigned long long int, std::nano> duration =
        (stop - start_);
      auto print_d = std::chrono::duration_cast<std::chrono::nanoseconds>(
        duration).count();
      auto log = std::string("Operation [") + memo_ + std::string("] took");
      logging::info(LOG_TAG, log, print_d, "nanoseconds.");

      return duration;
    }
  }
}
