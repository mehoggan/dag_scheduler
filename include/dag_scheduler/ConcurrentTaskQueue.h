////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef DAG_SCHEDULER_CONCURRENTTASKQUEUE_H
#define DAG_SCHEDULER_CONCURRENTTASKQUEUE_H

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <utility>

#include "dag_scheduler/Task.h"

namespace com::dag_scheduler {
class ConcurrentTaskQueue {
private:
    struct QueueNotEmpty {
    public:
        explicit QueueNotEmpty(const std::deque<std::unique_ptr<Task>>& queue);

        bool operator()() const;

    private:
        const std::deque<std::unique_ptr<Task>>& queue_;
    };

public:
    /**
     * @brief Default ctor.
     */
    ConcurrentTaskQueue();

    ConcurrentTaskQueue(const ConcurrentTaskQueue&) = delete;

    ConcurrentTaskQueue& operator=(const ConcurrentTaskQueue&) = delete;

    ConcurrentTaskQueue(ConcurrentTaskQueue&&) = delete;

    ConcurrentTaskQueue& operator=(ConcurrentTaskQueue&&) = delete;

    /**
     * @brief A function to check how many items are on the queue.
     *
     * WARNING: The accuracy of this function depends on the use case
     * of the queue. If there are many pushes and pops the accuracy of this
     * number is not safe.
     *
     * @return The number of elements of type \ref task contained in
     *         the queue.
     */
    std::size_t size();

    /**
     * @brief A function that pushes a generic type onto the queue.
     *
     * A member function of \ref ConcurrentTaskQueue<Task> that pushes
     * a instance of \ref task onto the queue FIFO thread safe manner.
     *
     * @param[in] data The \ref task to push onto the queue.
     */
    void push(std::unique_ptr<Task>&& data);

    /**
     * @brief A function that checks to see if the queue has data in it.
     *
     * A member function of \ref ConcurrentTaskQueue<Task> that checks
     * to see if the queue has data in it.
     *
     * WARNING: The accuracy of this function only applies to the exact
     * moment the function that uses it. Storing the return value in a
     * variable and checking its value later MIGHT NOT be true.
     *
     * @return true if the queue has data in it, and false otherwise.
     */
    bool empty() const;

    /**
     * @brief A function that empties the contents of the queue.
     *
     * A member function of \ref ConcurrentTaskQueue<Task> removes all
     * data from the queue in thread safe manner.
     */
    void clear();

    /**
     * @brief A function that removes \ref task from queue if it is not
     *        empty.
     *
     * A member function of \ref ConcurrentTaskQueue<Task> that attempts
     * to remove a \ref task from the queue if it is not empty. If the
     * \ref ConcurrentTaskQueue<Task> is not empty the item is removed
     * and returned.
     *
     * @param[out] popped_value The \ref task to assign to if queue is
                                NOT empty.
     *
     * @return True if queue was not empty and item was removed.
     */
    bool tryPop(std::unique_ptr<Task>& popped_value);

    /**
     * @brief A function that removes \ref task from queue if it is not
     *        empty
     *
     * A member function of \ref ConcurrentTaskQueue<Task> that attempts
     * to remove a \ref task from the queue if it is not empty. If the
     * \ref ConcurrentTaskQueue<Task> is not empty the item is removed
     * and returned based on if there are items on the queue.
     *
     * WARNING: Will block indefinitely or until the underlying mutex is lost
     *
     * @return popped_value The \ref task to assign to if queue is
     *                      NOT empty.
     */
    std::unique_ptr<Task> waitAndPop();

    /**
     * @brief A function that removes \ref task from queue if it is not
     *        empty
     *
     * A member function of \ref ConcurrentTaskQueue<Task> that attempts
     * to remove a \ref task from the queue if it is not empty. If the
     * \ref ConcurrentTaskQueue<Task> is not empty the item is removed
     * and returned based on if there are items on the queue.
     *
     * Used to deal with the Lost Wake Up Problem (Antonym of Spurious
     * Wakeup Problem) which allows the user to control when the mutex
     * is lost.
     *
     * @tparam Rep The type for the count of ticks.
     * @tparam Period A compile-time rational constant representing the
     *                number of seconds from one tick to the next.
     * @param popped_value[out] The \ref task to assign to if queue is
     *                          NOT empty.
     * @param wait_duration[in] The amount of time used to wait to check
     *                          if the queue has data in it.
     *
     * @return "true" if queue was not empty and item was removed.
     */
    template <typename Rep, typename Period>
    bool waitForAndPop(
            std::unique_ptr<Task>& popped_value,
            const std::chrono::duration<Rep, Period>& wait_duration) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!condition_variable_.wait_for(
                    lock, wait_duration, QueueNotEmpty(queue_))) {
            return false;
        }

        popped_value = std::move(queue_.front());
        queue_.pop_front();
        return true;
    }

    /**
     * @brief A function to remove an item from the queue.
     *
     * A member function of \ref ConcurrentTaskQueue<Task> that attempts
     * to remove a defined item from anywhere in the queue. If the user
     * calls this that task will be removed permanently from the queue and
     * not returned
     *
     * @param[in] to_remove A reference to a task to be removed from the
     *                      queue.
     * @param[out] ret A \ref std::unique_ptr to the contents removed from
     *                 the queue.
     *
     * @return "true" if item was found and removed, false otherwise.
     */
    void removeTaskFromQueue(const UUID& to_remove,
                             std::unique_ptr<Task>& ret_ptr);

private:
    std::deque<std::unique_ptr<Task>> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condition_variable_;
};
}  // namespace com::dag_scheduler
#endif
