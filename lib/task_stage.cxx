#include "dag_scheduler/task_stage.h"

#include "dag_scheduler/logging.h"

namespace com
{
  namespace dag_scheduler
  {
    TaskStage::TaskStage() :
      TaskStage("")
    {
      label_ = uuid_.as_string();
    }

    TaskStage::TaskStage(const std::string &label) :
      label_(label)
    {}

    TaskStage::~TaskStage()
    {}

    TaskStage::TaskStage(TaskStage &&other) :
      label_(std::move(other.label_)),
      uuid_(std::move(other.uuid_))
    {
      assert(not other.is_running() &&
        "You cannot move a running TaskStage");
    }

    TaskStage &TaskStage::operator=(TaskStage &&other)
    {
      assert(not other.is_running() &&
        "You cannot move a running TaskStage");

      label_ = std::move(other.label_);
      uuid_ = std::move(other.uuid_);

      return (*this);
    }

    const std::string &TaskStage::label() const
    {
      return label_;
    }

    const UUID &TaskStage::get_uuid() const
    {
      return uuid_;
    }

    bool operator==(const TaskStage &lhs, const TaskStage &rhs)
    {
      return lhs.uuid_ == rhs.uuid_;
    }

    bool operator!=(const TaskStage &lhs, const TaskStage &rhs)
    {
      return !(lhs == rhs);
    }

    std::ostream &operator<<(std::ostream &out, const TaskStage &t)
    {
      out << "label = " << t.label_;
      if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
      }

      return out;
    }

    std::stringstream &operator<<(std::stringstream &out, const TaskStage &t)
    {
      out << "label = " << t.label_;
      if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
      }

      return out;
    }
  }
}
