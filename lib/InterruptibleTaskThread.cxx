////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "dag_scheduler/InterruptibleTaskThread.h"

#include <condition_variable>
#include <functional>

#include "dag_scheduler/Logging.h"
#include "dag_scheduler/Task.h"

/* ✓ */
namespace com::dag_scheduler {
InterruptibleTaskThread::InterruptibleTaskThread()
        : InterruptibleTaskThread(LogTag(__FUNCTION__)) {}

InterruptibleTaskThread::InterruptibleTaskThread(const LogTag& tag)
        : LOG_TAG(tag), interrupt_(false), running_(false) {
    Logging::addStdCoutLogger(tag);
    Logging::addStdCerrLogger(tag);
}

InterruptibleTaskThread::~InterruptibleTaskThread() { shutdown(); }

InterruptibleTaskThread::InterruptibleTaskThread(
        InterruptibleTaskThread&& other)
        : LOG_TAG(std::move(other.LOG_TAG))
        , interrupt_(false)
        , running_(false) {
    assert(!other.isRunning() && "Cannot move a running task.");

    task_ = std::move(other.task_);
}

InterruptibleTaskThread& InterruptibleTaskThread::operator=(
        InterruptibleTaskThread&& rhs) {
    assert(!rhs.isRunning() && "Cannot move a running task.");

    LOG_TAG = std::move(rhs.LOG_TAG);
    interrupt_.store(false);
    running_.store(false);
    task_ = std::move(rhs.task_);

    return (*this);
}

bool InterruptibleTaskThread::setTaskAndRun(std::unique_ptr<Task>&& task) {
    bool thread_started = false;
    std::mutex return_mutex;
    std::unique_lock<std::mutex> return_lock(return_mutex);
    std::condition_variable return_cond;

    {
        std::lock_guard<std::mutex> lock(task_lock_);
        task_ = std::move(task);
    }

    std::function<bool(InterruptibleTaskThread*)> threaded_func =
            [&](InterruptibleTaskThread* self) {
                thread_started = true;
                return_cond.notify_one();
                bool all_ran = false;
                if (self->task_ != nullptr) {
                    self->running_.store(true);
                    all_ran = self->task_->iterateStages([&](TaskStage& next) {
                        bool stage_status = false;
                        {
                            std::lock_guard<std::mutex> lock(task_lock_);
                            Logging::info(LOG_TAG, "Going to run stage", next);
                            stage_status = next.run();
                            std::this_thread::sleep_for(
                                    std::chrono::milliseconds(1));
                        }

                        const bool this_was_interrupted =
                                self->wasInterrupted();
                        const bool cont =
                                stage_status && (!this_was_interrupted);

                        if (cont) {
                            Logging::info(LOG_TAG, "Ran stage", next);
                        } else {
                            Logging::error(LOG_TAG, "Failed to execute", next);
                            all_ran = false;
                        }

                        return cont;
                    });
                    std::unique_ptr<com::dag_scheduler::Task> tmp_task =
                            std::move(self->task_);
                    self->task_.reset();
                    self->running_.store(false);
                    tmp_task->complete(all_ran);
                }
                return all_ran;
            };

    InterruptibleTaskThread* threaded_ptr = this;
    thread_ = std::thread(threaded_func, threaded_ptr);

    return_cond.wait(return_lock);
    return thread_started;
}

void InterruptibleTaskThread::setInterrupt(bool should_interrupt) {
    {
        std::lock_guard<std::mutex> lock(task_lock_);
        (task_ != nullptr) ? task_->kill() : true;
    }
    interrupt_.store(should_interrupt);
}

bool InterruptibleTaskThread::wasInterrupted() const {
    return interrupt_.load();
}

bool InterruptibleTaskThread::isRunning() const { return running_.load(); }

bool InterruptibleTaskThread::hasTask() const {
    {
        std::lock_guard<std::mutex> lock(task_lock_);
        return task_ != nullptr;
    }
}

void InterruptibleTaskThread::shutdown() {
    if (running_.load()) {
        setInterrupt();
    }
    if (thread_.joinable()) {
        thread_.join();
    }
}
}  // namespace com::dag_scheduler
