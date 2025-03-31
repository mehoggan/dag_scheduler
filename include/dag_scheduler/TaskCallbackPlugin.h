////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef DAG_SCHEDULER_TASKCALLBACKPLUGIN_H  // cspell:disable-line
#define DAG_SCHEDULER_TASKCALLBACKPLUGIN_H  // cspell:disable-line

#include "dag_scheduler/LoggedClass.hpp"
#include "dag_scheduler/Task.h"

namespace com::dag_scheduler {
class TaskCallbackPlugin {
public:
    /**
     * @brief virtual destructor.
     */
    virtual ~TaskCallbackPlugin();

    /**
     * @brief After all possible \ref TaskStages in a \ref Task have been
     * executed, this method is called by the \ref Task itself with a
     * non-constant reference \ref completed_task of itself.
     *
     * After all possible \ref TaskStages in a \ref Task have been
     * executed, this method is called by the \ref Task itself with a
     * non-constant reference \ref completed_task of itself.
     *
     * @param[in] status Did all stages complete?
     * @param[in] completed_task The task that was completed.
     */
    virtual void completed(bool status, Task& completed_task);

    /**
     * @brief Needed by the cloning of a \ref DAG. Your task callback plugin
     * must implement this method.
     *
     * Needed by the cloning of a \ref DAG. Your task callback plugin
     * must implement this method.
     *
     * @return A cloned version of (*this).
     */
    virtual std::unique_ptr<TaskCallbackPlugin> clone() const;
};
}  // namespace com::dag_scheduler

#endif
