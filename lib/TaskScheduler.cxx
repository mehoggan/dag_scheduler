////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "dag_scheduler/TaskScheduler.h"

#include <string>

#include "dag_scheduler/Logging.h"

namespace com::dag_scheduler {
TaskScheduler::TaskScheduler()
        : LoggedClass<TaskScheduler>(*this), pause_(true), kill_(true) {
    for (std::uint8_t id = 0; id < thread_pool_.size(); ++id) {
        thread_pool_[id] = std::make_unique<InterruptibleTaskThread>(
                LogTag(std::to_string(id)));
    }
}

bool TaskScheduler::startup() {
    pause_.store(false);
    kill_.store(false);
    auto refresh_time = std::chrono::milliseconds(5);
    while (!kill_.load()) {
        bool should_get_next = !(kill_.load() || pause_.load());
        if (should_get_next) {
            std::unique_ptr<Task> next_task = nullptr;
            queue_.waitForAndPop(next_task, refresh_time);
            if (next_task) {
                Logging::info(LOG_TAG, "next task =", (*next_task));
                while (true) {
                    if (kill_.load()) {
                        if (next_task) {
                            next_task->kill();
                        }
                        break;
                    }
                    if (!pause_.load()) {
                        std::size_t unused_index = firstUnusedThread();
                        if (unused_index != static_cast<std::size_t>(-1)) {
                            if (kill_.load()) {
                                if (next_task) {
                                    next_task->kill();
                                }
                                next_task.reset(nullptr);
                                break;
                            }
                            {
                                std::lock_guard<std::mutex> lock(
                                        thread_pool_lock_);
                                if (next_task) {
                                    thread_pool_[unused_index]->setTaskAndRun(
                                            std::move(next_task));
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

void TaskScheduler::queueTask(std::unique_ptr<Task>&& task) {
    queue_.push(std::move(task));
}

bool TaskScheduler::killTask(const Task& task) {
    return killTask(task.getUUID());
}

bool TaskScheduler::killTask(const UUID& uuid) {
    std::unique_ptr<Task> to_kill;
    queue_.removeTaskFromQueue(uuid, to_kill);
    return true;
}

void TaskScheduler::pause() { pause_.store(true); }

void TaskScheduler::resume() { pause_.store(false); }

bool TaskScheduler::isPaused() { return pause_.load(); }

void TaskScheduler::shutdown() {
    pause();
    kill_.store(true);
}

bool TaskScheduler::isShutdown() { return kill_.load(); }

std::size_t TaskScheduler::firstUnusedThread() {
    std::lock_guard<std::mutex> lock(thread_pool_lock_);
    std::size_t first_unused_index = static_cast<std::size_t>(-1);
    for (std::size_t i = 0; i < thread_pool_.size(); ++i) {
        if (!thread_pool_[i]->isRunning()) {
            first_unused_index = i;
            break;
        }
    }
    return first_unused_index;
}
}  // namespace com::dag_scheduler
