////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef DAG_SCHEDULER_INTERRUPTIBLETASKTHREAD_H  // cspell:disable-line
#define DAG_SCHEDULER_INTERRUPTIBLETASKTHREAD_H  // cspell:disable-line

#include "dag_scheduler/Logging.h"
#include "dag_scheduler/Task.h"

#include <thread>

namespace com::dag_scheduler {
class InterruptibleTaskThread {
public:
    /**
     * @brief
     */
    InterruptibleTaskThread();

    /**
     * @brief
     *
     * @param[in] tag
     */
    explicit InterruptibleTaskThread(const LogTag& tag);

    /**
     * @brief
     */
    ~InterruptibleTaskThread();

    InterruptibleTaskThread(const InterruptibleTaskThread&) = delete;

    InterruptibleTaskThread& operator=(const InterruptibleTaskThread&) =
            delete;

    /**
     * @brief
     *
     * @param[out] other
     */
    InterruptibleTaskThread(InterruptibleTaskThread&& other);

    /**
     * @brief
     *
     * @param[out] rhs
     *
     * @return
     */
    InterruptibleTaskThread& operator=(InterruptibleTaskThread&& rhs);

    /**
     * @brief
     *
     * @param[out] task
     * @param[in] complete_callback
     * @param[in] delay_between_stages
     *
     * @return
     */
    bool setTaskAndRun(std::unique_ptr<Task>&& task);

    /**
     * @brief
     *
     * @param[in] should_interrupt
     *
     * @return
     */
    void setInterrupt(bool should_interrupt = true);

    /**
     * @brief
     *
     * @return
     */
    bool wasInterrupted() const;

    /**
     * @brief
     *
     * @return
     */
    bool isRunning() const;

    /**
     * @brief
     *
     * @return
     */
    bool hasTask() const;

    /**
     * @brief
     */
    void shutdown();

private:
    std::unique_ptr<Task> task_;
    mutable std::mutex task_lock_;
    LogTag LOG_TAG;
    volatile std::atomic_bool interrupt_;
    volatile std::atomic_bool running_;
    std::thread thread_;
};
}  // namespace com::dag_scheduler

#endif
