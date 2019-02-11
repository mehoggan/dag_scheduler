#include "dag_scheduler/task.h"

#include "dag_scheduler/logging.h"

namespace com
{
  namespace dag_scheduler
  {
    task::task() :
      iterating_(false),
      kill_(false)
    {
      label_ = uuid_.as_string();
    }

    task::task(std::vector<std::unique_ptr<task_stage>> &stages) :
      task(stages, "")
    {
      label_ = uuid_.as_string();
    }

    task::task(std::vector<std::unique_ptr<task_stage>> &stages,
      const std::string &label) :
      iterating_(false),
      kill_(false),
      stages_(std::move(stages)),
      label_(label)
    {}

    task::~task()
    {}

    task::task(task &&other) :
      iterating_(false),
      kill_(false),
      stages_(std::move(other.stages_)),
      label_(std::move(other.label_)),
      uuid_(other.uuid_.clone())
    {
      assert(not other.iterating_.load() && "You cannot move an itterating"
        "task");
    }

    task &task::operator=(task &&other)
    {
      assert(not other.iterating_.load() && "You cannot move an itterating"
        "task");

      iterating_.store(false);
      kill_.store(false);
      stages_ = std::move(other.stages_);
      label_ = std::move(other.label_);
      uuid_ = other.uuid_.clone();

      return (*this);
    }

    const std::string &task::label() const
    {
      return label_;
    }

    const uuid &task::get_uuid() const
    {
      return uuid_;
    }

    bool task::iterate_stages(
      const std::function<bool(task_stage &)> &next_stage)
    {
      bool ran_all = false;

      if (not iterating_.load()) {
        iterating_.store(true);
        {
          ran_all = std::all_of(stages_.begin(), stages_.end(),
            [&](class std::unique_ptr<task_stage> &next) {
              bool ran = next_stage(*next);
              next->cleanup();
              bool ret = (ran && next->end() && (not kill_.load()));
              return ret;
            });
        }
        iterating_.store(false);
      }

      return ran_all;
    }

    bool task::kill()
    {
      kill_.store(true);
      return kill_.load();
    }

    bool operator==(const task &lhs, const task &rhs)
    {
      return lhs.uuid_ == rhs.uuid_;
    }

    bool operator!=(const task &lhs, const task &rhs)
    {
      return !(lhs == rhs);
    }

    std::ostream &operator<<(std::ostream &out, const task &t)
    {
      out << "label =" << t.label_;
      if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
      }
      std::for_each(t.stages_.begin(), t.stages_.end(),
        [&](const std::unique_ptr<task_stage> &s) {
          out << (*s) << " ";
        });

      return out;
    }

    std::stringstream &operator<<(std::stringstream &out, const task &t)
    {
      out << "label =" << t.label_;
      if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
      }
      std::for_each(t.stages_.begin(), t.stages_.end(),
        [&](const std::unique_ptr<task_stage> &s) {
          out << (*s) << " ";
        });

      return out;
    }
  }
}
