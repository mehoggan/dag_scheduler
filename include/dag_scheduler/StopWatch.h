#ifndef DAG_SCOPED_TIMER_H_INCLUDED
#define DAG_SCOPED_TIMER_H_INCLUDED

#include <atomic>
#include <chrono>
#include <string>

#include "dag_scheduler/logging.h"

namespace com {
namespace dag_scheduler {
/**
 * @brief A class that sets up stderror and stdout loggers for a derived
 *        class.
 */
class StopWatch {
public:
    /**
     * @brief ctor which starts timer conditionally at construction time.
     *
     * @param[in] tag The tag used to log the time. If the LogTag has not
     *                been added to the logger it will be as a std::cout
     *                logger.
     * @param[in] memo The note the user wants printed in the results.
     * @param[in] start_on_construction A flag which determines if the
     *                                  stop watch start at construction
     *                                  time.
     */
    StopWatch(const LogTag& tag,
              const std::string& memo,
              bool start_on_construction = true);

    /**
     * @brief dtor
     */
    ~StopWatch();

    /**
     * @brief copy ctor for StopWatch.
     *
     * The copy constructor behaves like a full reset of the
     * \ref StopWatch in the since that in sets the current time to now
     * and puts it into a running state.
     *
     * @param other The stop watch to copy into this.
     */
    StopWatch(const StopWatch& other);

    /**
     *
     */
    /**
     * @brief assignement operator for StopWatch.
     *
     * The assignment operator behaves like a full reset of the
     * \ref StopWatch in the since that in sets the current time to now
     * and puts it into a running state.
     *
     * @param[in] rh The StopWatch to copy into this.
     *
     * @return A reference to (*this).
     */
    StopWatch& operator=(const StopWatch& rhs);

    /**
     * @brief move copy constructor for StopWatch
     *
     * The move copy constructor behaves like a full reset of the
     * \ref StopWatch in the since that it sets the current time to now
     * and puts it into a running state.
     *
     * @param[out] other The StopWatch to move into this.
     */
    StopWatch(StopWatch&& other);

    /**
     * @brief move assignement operator for StopWatch.
     *
     * The move assignment operator behaves like a full reset of the
     * \ref stop watch in the since that it sets the current time to now
     * and it puts it into a running state.
     *
     * @param[out] rhs The StopWatch to move into this.
     *
     * @return A reference to this.
     */
    StopWatch& operator=(StopWatch&& rhs);

    /**
     * @brief Used to start the StopWatch.
     *
     * If the StopWatch is not already running the start it. Otherwise
     * ignore the request from the user.
     */
    void start();

    /**
     * @brief Used to stop the StopWatch.
     *
     * If the StopWatch is not already stoped it stops it.
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
     * @brief Identical to calling \ref StopWatch::stop then
     *        \ref StopWatch::start.
     */
    void reset();

private:
    std::chrono::nanoseconds print(
            const std::chrono::high_resolution_clock::time_point& stop);

private:
    LogTag LOG_TAG;
    std::string memo_;
    std::atomic_bool was_stopped_;
    std::atomic_bool was_started_;
    std::chrono::high_resolution_clock::time_point start_;
};
}  // namespace dag_scheduler
}  // namespace com

#endif
