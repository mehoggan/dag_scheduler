////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef DAG_SCHEDULER_TASKSCHEDULER_H  // cspell:disable-line
#define DAG_SCHEDULER_TASKSCHEDULER_H  // cspell:disable-line

#include <gtest/gtest_prod.h>

#include <array>
#include <memory>
#include <mutex>

#include "dag_scheduler/ConcurrentTaskQueue.h"
#include "dag_scheduler/InterruptibleTaskThread.h"
#include "dag_scheduler/LoggedClass.hpp"
#include "dag_scheduler/Task.h"

namespace com::dag_scheduler {
class TaskScheduler : public LoggedClass<TaskScheduler>,
                      public boost::noncopyable {  // cspell:disable-line
public:
    /**
     * @brief
     */
    TaskScheduler();

    /**
     * @brief
     *
     * @return
     */
    bool startup();

    /**
     * @brief
     *
     * @param t
     */
    void queueTask(std::unique_ptr<Task>&& task);

    /**
     * @brief
     *
     * @param t
     *
     * @return
     */
    bool killTask(const Task& task);

    /**
     * @brief
     *
     * @param u
     *
     * @return
     */
    bool killTask(const UUID& uuid);

    /**
     * @brief
     */
    void pause();

    /**
     * @brief
     */
    void resume();

    /**
     * @brief
     *
     * @return
     */
    bool isPaused();

    /**
     * @brief
     */
    void shutdown();

    /**
     * @brief
     *
     * @return
     */
    bool isShutdown();

private:
    std::size_t firstUnusedThread();

private:
    ConcurrentTaskQueue queue_;
    volatile std::atomic_bool pause_;
    volatile std::atomic_bool kill_;
    std::array<std::unique_ptr<InterruptibleTaskThread>, 10> thread_pool_;
    std::mutex thread_pool_lock_;
};
}  // namespace com::dag_scheduler

#endif
