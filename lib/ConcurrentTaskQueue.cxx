////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include <dag_scheduler/ConcurrentTaskQueue.h>
#include <dag_scheduler/Logging.h>

#include <cassert>
#include <utility>

namespace com::dag_scheduler {
ConcurrentTaskQueue::QueueNotEmpty::QueueNotEmpty(
        const std::deque<std::unique_ptr<Task>>& queue)
        : queue_(queue) {}

bool ConcurrentTaskQueue::QueueNotEmpty::operator()() const {
    return !queue_.empty();
}

ConcurrentTaskQueue::ConcurrentTaskQueue() {}

std::size_t ConcurrentTaskQueue::size() {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

void ConcurrentTaskQueue::push(std::unique_ptr<Task>&& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_back(std::move(data));
    condition_variable_.notify_all();
}

bool ConcurrentTaskQueue::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

void ConcurrentTaskQueue::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::deque<std::unique_ptr<Task>> empty;
    std::swap(queue_, empty);
    condition_variable_.notify_all();
}

bool ConcurrentTaskQueue::tryPop(std::unique_ptr<Task>& popped_value) {
    std::lock_guard<std::mutex> lock(mutex_);
    bool ret_value = false;
    if (!queue_.empty()) {
        popped_value = std::move(queue_.front());
        queue_.pop_front();
        ret_value = true;
    }

    return ret_value;
}

std::unique_ptr<Task> ConcurrentTaskQueue::wait_and_pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_variable_.wait(lock, QueueNotEmpty(queue_));
    assert(!queue_.empty());
    std::unique_ptr<Task> popped_value = std::move(queue_.front());
    queue_.pop_front();
    return popped_value;
}

void ConcurrentTaskQueue::remove_task_from_queue(
        const UUID& to_remove, std::unique_ptr<Task>& ret_ptr) {
    LogTag lt(__FUNCTION__);
    Logging::add_std_cout_logger(lt);

    std::unique_lock<std::mutex> lock(mutex_);
    if (!queue_.empty()) {
        auto iterator = std::remove_if(
                queue_.begin(), queue_.end(), [&](std::unique_ptr<Task>& task) {
                    auto ret = task->get_uuid().as_string() ==
                               to_remove.as_string();
                    if (ret) {
                        ret_ptr = std::move(task);
                    }
                    return ret;
                });
        queue_.resize(std::size_t(iterator - queue_.begin()));
    }
}
}  // namespace com::dag_scheduler
