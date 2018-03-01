#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <queue>

#include <boost/noncopyable.hpp>

#include <condition_variable>
#include <mutex>
#include <thread>
#include <type_traits>

namespace com
{
  namespace dag_scheduler
  {
    template<typename DataType>
    class concurrent_queue
      : public boost::noncopyable
    {
    private:
      struct queue_not_empty
      {
        std::queue<DataType>& queue_;

        queue_not_empty(std::queue<DataType>& queue)
          : queue_(queue)
        {}

        bool operator()() const
        {
          return !queue_.empty();
        }
      };

    public:
      concurrent_queue()
      {
        static_assert(std::is_move_assignable<DataType>::value &&
          std::is_move_constructible<DataType>::value,
          "Your DataType does is not movable.");
      }

      //! A function to check how many items are on the queue.
      /*!
        WARNING: The accuracy of this function depends on the use case
        of the queue. If there are many pushes and pops the accuracy of this
        number is not safe.
      */
      std::size_t size()
      {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
      }

      //! A function that pushes a generic type onto the queue.
      /*!
        A member function of \ref concurrent_queue<DataType> that pushes
        a instance of \ref DataType onto the queue FIFO thread safe manner.

        \param[in] data The \ref DataType to push onto the queue.
      */
      void push(DataType const& data)
      {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(data));
        condition_variable_.notify_all();
      }

      //! A function that checks to see if the queue has data in it.
      /*!
        A member function of \ref concurrent_queue<DataType> that checks
        to see if the queue has data in it.

        WARNING: The accuracy of this function only applies to the exact
        moment the function that uses it. Storing the return value in a
        variable and checking its value later MIGHT NOT be true.

        \return True if the queue has data in it, and false otherwise.
      */
      bool empty() const
      {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
      }

      //! A function that empties the contents of the queue.
      /*!
        A member function of \ref concurrent_queue<DataType> removes all
        data from the queue in thread safe manner.
      */
      void clear()
      {
        std::lock_guard<std::mutex> lock(mutex_);
        std::queue<DataType> empty;
        std::swap(queue_, empty);
        condition_variable_.notify_all();
      }

      //! A function that removes \ref DataType from queue if it is not empty
      /*!
        A member function of \ref concurrent_queue<DataType> that attempts
        to remove a \ref DataType from the queue if it is not empty. If the
        \ref concurrent_queue<DataType> is not empty the item is removed
        and returned.

        \param[out] popped_value The \ref DataType to assign to if queue is
                                 NOT empty.
        \return True if queue was not empty and item was removed.
      */
      bool try_pop(DataType& popped_value)
      {
        std::lock_guard<std::mutex> lock(mutex_);
        bool ret = false;
        if(!queue_.empty()) {
          popped_value = std::move(queue_.front());
          queue_.pop();
          ret = true;
        }

        return ret;
      }

      //! A function that removes \ref DataType from queue if it is not empty
      /*!
        A member function of \ref concurrent_queue<DataType> that attempts
        to remove a \ref DataType from the queue if it is not empty. If the
        \ref concurrent_queue<DataType> is not empty the item is removed
        and returned based on if there are items on the queue.

        WARNING: Will block indefinetly or until the underlying mutex is lost

        \param[out] popped_value The \ref DataType to assign to if queue is
                                 NOT empty.
        \return True if queue was not empty and item was removed.
      */
      void wait_and_pop(DataType& popped_value)
      {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_variable_.wait(lock, queue_not_empty(queue_));
        assert(!queue_.empty());
        popped_value = std::move(queue_.front());
        queue_.pop();
      }

      //! A function that removes \ref DataType from queue if it is not empty
      /*!
        A member function of \ref concurrent_queue<DataType> that attempts
        to remove a \ref DataType from the queue if it is not empty. If the
        \ref concurrent_queue<DataType> is not empty the item is removed
        and returned based on if there are items on the queue.

        Used to deal with the Lost Wake Up Problem (Antonym of Spurious
        Wakeup Problem) which allows the user to control when the mutex
        is lost.

        \param[out] popped_value The \ref DataType to assign to if queue is
                                 NOT empty.
        \param[in] wait_duration The amount of time used to wait to check
                                 if the queue has data in it.
        \return True if queue was not empty and item was removed.
      */
      template <typename Rep, typename Period>
      bool wait_for_and_pop(DataType& popped_value,
        const std::chrono::duration<Rep, Period> &wait_duration)
      {
        std::unique_lock<std::mutex> lock(mutex_);
        if(!condition_variable_.wait_for(lock, wait_duration,
          queue_not_empty(queue_))) {
          return false;
        }

        popped_value = std::move(queue_.front());
        queue_.pop();
        return true;
      }

      //! A function that prints the content of the queue at a specific time.
      /*!
        A friend function of \ref concurrent_queue<DataType> that outputs
        the contents of the queue at the moment the function is evaluated
        to the stream \ref out.

        \param[in] in The stream to write the contents to.
        \param[in] in The \ref concurrent_queue<DataType> for which to print
                      its contents.

        \return The \ref out parameter provided as input.
      */
      friend std::ostream &operator<<(std::ostream &out,
        const concurrent_queue<DataType> &q)
      {
        std::queue<DataType> tmp = q.queue_;

        {
          std::unique_lock<std::mutex> lock(q.mutex_);
          tmp = q.queue_;
        }

        while (!tmp.empty()) {
          out << tmp.front();
          tmp.pop();
        }

        assert(tmp.empty() && "Failed to empty queue.");

        return out;
      }

    private:
       std::queue<DataType> queue_;
       mutable std::mutex mutex_;
       std::condition_variable condition_variable_;
    };
  }
}

#endif

