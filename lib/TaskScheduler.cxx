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
            queue_.wait_for_and_pop(next_task, refresh_time);
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
                        std::size_t unused_index = first_unused_thread();
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
                                    thread_pool_[unused_index]
                                            ->set_task_and_run(
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

void TaskScheduler::queue_task(std::unique_ptr<Task>&& t) {
    queue_.push(std::move(t));
}

bool TaskScheduler::kill_task(const Task& t) { return kill_task(t.get_uuid()); }

bool TaskScheduler::kill_task(const UUID& u) {
    std::unique_ptr<Task> to_kill;
    queue_.remove_task_from_queue(u, to_kill);
    return true;
}

void TaskScheduler::pause() { pause_.store(true); }

void TaskScheduler::resume() { pause_.store(false); }

bool TaskScheduler::is_paused() { return pause_.load(); }

void TaskScheduler::shutdown() {
    pause();
    kill_.store(true);
}

bool TaskScheduler::is_shutdown() { return kill_.load(); }

std::size_t TaskScheduler::first_unused_thread() {
    std::lock_guard<std::mutex> lock(thread_pool_lock_);
    std::size_t first_unused_index = static_cast<std::size_t>(-1);
    for (std::size_t i = 0; i < thread_pool_.size(); ++i) {
        if (!thread_pool_[i]->is_running()) {
            first_unused_index = i;
            break;
        }
    }
    return first_unused_index;
}
}  // namespace com::dag_scheduler
