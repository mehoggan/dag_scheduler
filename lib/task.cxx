#include "dag_scheduler/task.h"

#include "dag_scheduler/logging.h"

namespace com
{
  namespace dag_scheduler
  {
    task::task() :
      task("")
    {
      label_ = uuid_.as_string();
    }

    task::task(const std::string &label) :
      label_(label)
    {}

    task::~task()
    {}

    const std::string &task::label() const
    {
      return label_;
    }

    const uuid &task::get_uuid() const
    {
      return uuid_;
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

      return out;
    }

    std::stringstream &operator<<(std::stringstream &out, const task &t)
    {
      out << "label =" << t.label_;
      if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
      }

      return out;
    }
  }
}
