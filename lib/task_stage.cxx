#include "dag_scheduler/task_stage.h"

#include "dag_scheduler/logging.h"

namespace com
{
  namespace dag_scheduler
  {
    task_stage::task_stage() :
      task_stage("")
    {
      label_ = uuid_.as_string();
    }

    task_stage::task_stage(const std::string &label) :
      label_(label)
    {}

    task_stage::~task_stage()
    {}

    task_stage::task_stage(task_stage &&other) :
      label_(std::move(other.label_)),
      uuid_(std::move(other.uuid_))
    {
      assert(not other.is_running() &&
        "You cannot move a running task_stage");
    }

    task_stage &task_stage::operator=(task_stage &&other)
    {
      assert(not other.is_running() &&
        "You cannot move a running task_stage");

      label_ = std::move(other.label_);
      uuid_ = std::move(other.uuid_);

      return (*this);
    }

    const std::string &task_stage::label() const
    {
      return label_;
    }

    const uuid &task_stage::get_uuid() const
    {
      return uuid_;
    }

    bool operator==(const task_stage &lhs, const task_stage &rhs)
    {
      return lhs.uuid_ == rhs.uuid_;
    }

    bool operator!=(const task_stage &lhs, const task_stage &rhs)
    {
      return !(lhs == rhs);
    }

    std::ostream &operator<<(std::ostream &out, const task_stage &t)
    {
      out << "label =" << t.label_;
      if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
      }

      return out;
    }

    std::stringstream &operator<<(std::stringstream &out, const task_stage &t)
    {
      out << "label =" << t.label_;
      if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
      }

      return out;
    }
  }
}
