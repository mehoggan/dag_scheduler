////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef TASK_SCHEDULER_H_INCLUDED
#define TASK_SCHEDULER_H_INCLUDED

#include <gtest/gtest_prod.h>

#include <array>
#include <memory>
#include <mutex>
#include <tuple>

#include "dag_scheduler/ConcurrentTaskQueue.h"
#include "dag_scheduler/InterruptibleTaskThread.h"
#include "dag_scheduler/LoggedClass.hpp"
#include "dag_scheduler/Task.h"

namespace com::dag_scheduler {
class TaskScheduler : public LoggedClass<TaskScheduler>,
                      public boost::noncopyable {
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
    void queue_task(std::unique_ptr<Task>&& t);

    /**
     * @brief
     *
     * @param t
     *
     * @return
     */
    bool kill_task(const Task& t);

    /**
     * @brief
     *
     * @param u
     *
     * @return
     */
    bool kill_task(const UUID& u);

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
    bool is_paused();

    /**
     * @brief
     */
    void shutdown();

    /**
     * @brief
     *
     * @return
     */
    bool is_shutdown();

private:
    std::size_t first_unused_thread();

private:
    ConcurrentTaskQueue queue_;
    volatile std::atomic_bool pause_;
    volatile std::atomic_bool kill_;
    std::array<std::unique_ptr<InterruptibleTaskThread>, 10> thread_pool_;
    std::mutex thread_pool_lock_;
};
}  // namespace com::dag_scheduler

#endif
