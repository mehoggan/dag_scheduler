#include <dag_scheduler/concurrent_task_queue.h>
#include <dag_scheduler/logging.h>

#include <cassert>
#include <utility>

namespace com
{
  namespace dag_scheduler
  {
    concurrent_task_queue::queue_not_empty::queue_not_empty(
      const std::deque<std::unique_ptr<task>>& queue) :
      queue_(queue)
    {}

    bool concurrent_task_queue::queue_not_empty::operator()() const
    {
      return !queue_.empty();
    }

    concurrent_task_queue::concurrent_task_queue()
    {}

    std::size_t concurrent_task_queue::size()
    {
      std::lock_guard<std::mutex> lock(mutex_);
      return queue_.size();
    }

    void concurrent_task_queue::push(std::unique_ptr<task> &&data)
    {
      std::lock_guard<std::mutex> lock(mutex_);
      queue_.push_back(std::move(data));
      condition_variable_.notify_all();
    }

    bool concurrent_task_queue::empty() const
    {
      std::lock_guard<std::mutex> lock(mutex_);
      return queue_.empty();
    }

    void concurrent_task_queue::clear()
    {
      std::lock_guard<std::mutex> lock(mutex_);
      std::deque<std::unique_ptr<task>> empty;
      std::swap(queue_, empty);
      condition_variable_.notify_all();
    }

    bool concurrent_task_queue::try_pop(std::unique_ptr<task> &popped_value)
    {
      std::lock_guard<std::mutex> lock(mutex_);
      bool ret = false;
      if(!queue_.empty()) {
        popped_value = std::move(queue_.front());
        queue_.pop_front();
        ret = true;
      }

      return ret;
    }

    std::unique_ptr<task> concurrent_task_queue::wait_and_pop()
    {
      std::unique_lock<std::mutex> lock(mutex_);
      condition_variable_.wait(lock, queue_not_empty(queue_));
      assert(!queue_.empty());
      std::unique_ptr<task> popped_value = std::move(queue_.front());
      queue_.pop_front();
      return popped_value;
    }


    void concurrent_task_queue::remove_task_from_queue(const uuid &to_remove,
      std::unique_ptr<task> &ret_ptr)
    {
      log_tag lt(__FUNCTION__);
      logging::add_std_cout_logger(lt);

      std::unique_lock<std::mutex> lock(mutex_);
      if (not queue_.empty()) {
        auto iterator = std::remove_if(queue_.begin(), queue_.end(),
          [&] (std::unique_ptr<task> &task) {
            auto ret = task.get()->get_uuid().as_string() ==
              to_remove.as_string();
            if (ret) {
              ret_ptr = std::move(task);
            }
            return  ret;
          });
        queue_.resize(std::size_t(iterator - queue_.begin()));
      }
    }
  }
}
