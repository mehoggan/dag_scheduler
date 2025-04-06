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

#include <chrono>

#include "dag_scheduler/Logging.h"
#include "dag_scheduler/Task.h"

namespace com::dag_scheduler {
class InterruptibleTaskThread {  // cspell:disable-line
public:
    /**
     * @brief
     */
    InterruptibleTaskThread();  // cspell:disable-line

    /**
     * @brief
     *
     * @param[in] tag
     */
    explicit InterruptibleTaskThread(const LogTag& tag);  // cspell:disable-line

    /**
     * @brief
     */
    ~InterruptibleTaskThread();  // cspell:disable-line

    InterruptibleTaskThread(                           // cspell:disable-line
            const InterruptibleTaskThread&) = delete;  // cspell:disable-line

    InterruptibleTaskThread& operator=(                // cspell:disable-line
            const InterruptibleTaskThread&) = delete;  // cspell:disable-line

    /**
     * @brief
     *
     * @param[out] other
     */
    InterruptibleTaskThread(                   // cspell:disable-line
            InterruptibleTaskThread&& other);  // cspell:disable-line

    /**
     * @brief
     *
     * @param[out] rhs
     *
     * @return
     */
    InterruptibleTaskThread& operator=(      // cspell:disable-line
            InterruptibleTaskThread&& rhs);  // cspell:disable-line

    /**
     * @brief
     *
     * @param[out] task
     * @param[in] complete_callback
     * @param[in] delay_between_stages
     *
     * @return
     */
    bool set_task_and_run(std::unique_ptr<Task>&& task);

    /**
     * @brief
     *
     * @param[in] should_interrupt
     *
     * @return
     */
    void set_interrupt(bool should_interrupt = true);

    /**
     * @brief
     *
     * @return
     */
    bool was_interrupted() const;

    /**
     * @brief
     *
     * @return
     */
    bool is_running() const;

    /**
     * @brief
     *
     * @return
     */
    bool has_task() const;

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
