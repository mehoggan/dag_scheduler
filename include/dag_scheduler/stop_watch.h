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
       * The copy constructor behaves like a full reset of the \ref stop_watch
       * in the since that in sets the current time to now and puts it into a
       * running state.
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
       * \ref stop_watch in the since that in sets the current time to now and
       * puts it into a running state.
       *
       * @param[in] rh The stop watch to copy into this.
       *
       * @return A reference to (*this).
       */
      stop_watch &operator=(const stop_watch &rhs);

      //! move copy ctor for stop_watch.
      /*!
       * This is equivalent to calling reset.
       */
      /**
       * @brief 
       *
       * @param other
       */
      stop_watch(stop_watch &&other);

      //! move assignment operator for stop_watch.
      /*!
       * This is equivalent to calling reset.
       */
      stop_watch &operator=(stop_watch &&rhs);

      //! Used to start the timer.
      /*!
       * If the timer is not already started then start it. Otherwise ignore
       * the request from user.
       */
      void start();

      //! Used to stop the timer.
      /*!
       * If the timer is not already stoped it stops it else
       */
      std::chrono::nanoseconds stop();

      //! Used to compute current time.
      /*!
       * If for whatever reason scoping cannot occur user may call this
       * function in a thread safe manner.
       */
      std::chrono::nanoseconds mark();

      //! Used to reset the timer's start time to current time.
      /*!
       * To restart timer to get offsets from the current time.
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
