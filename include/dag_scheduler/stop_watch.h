#ifndef DAG_SCOPED_TIMER_H_INCLUDED
#define DAG_SCOPED_TIMER_H_INCLUDED

#include "dag_scheduler/logging.h"

#include <atomic>
#include <chrono>
#include <string>

namespace com
{
  namespace dag_scheduler
  {
    /**
     * @brief A class that sets up stderror and stdout loggers for a derived
     *        class.
     */
    class stop_watch
    {
    public:
      /**
       * @brief ctor which starts timer conditionally at construction time.
       *
       * @param[in] tag The tag used to log the time. If the log_tag has not
       *                been added to the logger it will be as a std::cout
       *                logger.
       * @param[in] memo The note the user wants printed in the results.
       * @param[in] start_on_construction A flag which determines if the
       *                                  stop watch start at construction
       *                                  time.
       */
      stop_watch(const log_tag &tag, const std::string &memo,
        bool start_on_construction = true);

      /**
       * @brief dtor
       */
      ~stop_watch();

      /**
       * @brief copy ctor for stop_watch.
       *
       * The copy constructor behaves like a full reset of the
       * \ref stop_watch in the since that in sets the current time to now
       * and puts it into a running state.
       *
       * @param other The stop watch to copy into this.
       */
      stop_watch(const stop_watch &other);

      /**
       *
       */
      /**
       * @brief assignement operator for stop_watch.
       *
       * The assignment operator behaves like a full reset of the
       * \ref stop_watch in the since that in sets the current time to now
       * and puts it into a running state.
       *
       * @param[in] rh The stop_watch to copy into this.
       *
       * @return A reference to (*this).
       */
      stop_watch &operator=(const stop_watch &rhs);

      /**
       * @brief move copy constructor for stop_watch
       *
       * The move copy constructor behaves like a full reset of the
       * \ref stop_watch in the since that it sets the current time to now
       * and puts it into a running state.
       *
       * @param[out] other The stop_watch to move into this.
       */
      stop_watch(stop_watch &&other);

      /**
       * @brief move assignement operator for stop_watch.
       *
       * The move assignment operator behaves like a full reset of the
       * \ref stop watch in the since that it sets the current time to now
       * and it puts it into a running state.
       *
       * @param[out] rhs The stop_watch to move into this.
       *
       * @return A reference to this.
       */
      stop_watch &operator=(stop_watch &&rhs);

      /**
       * @brief Used to start the stop_watch.
       *
       * If the stop_watch is not already running the start it. Otherwise
       * ignore the request from the user.
       */
      void start();

      /**
       * @brief Used to stop the stop_watch.
       *
       * If the stop_watch is not already stoped it stops it.
       *
       * @return The duration from the start time.
       */
      std::chrono::nanoseconds stop();

      /**
       * @brief Used to print the duration time from when start was last
       *        called.
       *
       * @return The duration from the start time.
       */
      std::chrono::nanoseconds mark();

      /**
       * @brief Identical to calling \ref stop_watch::stop then
       *        \ref stop_watch::start.
       */
      void reset();

    private:
      std::chrono::nanoseconds print(
        const std::chrono::high_resolution_clock::time_point &stop);

    private:
      log_tag LOG_TAG;
      std::string memo_;
      std::atomic_bool was_stopped_;
      std::atomic_bool was_started_;
      std::chrono::high_resolution_clock::time_point start_;
    };
  }
}

#endif
