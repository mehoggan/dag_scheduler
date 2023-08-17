#include "dag_scheduler/examples/print_stage.h"

#include "dag_scheduler/logging.h"

namespace com
{
  namespace dag_scheduler
  {
    PrintStage::PrintStage() :
      PrintStage("")
    {
      label_ = uuid_.as_string();
    }

    PrintStage::PrintStage(const std::string &label) :
      label_(label)
    {}

    PrintStage::~PrintStage()
    {}

    PrintStage::PrintStage(PrintStage &&other) :
      label_(std::move(other.label_)),
      uuid_(std::move(other.uuid_))
    {
      assert(not other.is_running() &&
        "You cannot move a running PrintStage");
    }

    PrintStage &PrintStage::operator=(PrintStage &&other)
    {
      assert(not other.is_running() &&
        "You cannot move a running PrintStage");

      label_ = std::move(other.label_);
      uuid_ = std::move(other.uuid_);

      return (*this);
    }

    const std::string &PrintStage::label() const
    {
      return label_;
    }

    const UUID &PrintStage::get_uuid() const
    {
      return uuid_;
    }

    bool operator==(const PrintStage &lhs, const PrintStage &rhs)
    {
      return lhs.uuid_ == rhs.uuid_;
    }

    bool operator!=(const PrintStage &lhs, const PrintStage &rhs)
    {
      return !(lhs == rhs);
    }

    std::ostream &operator<<(std::ostream &out, const PrintStage &t)
    {
      out << "label = " << t.label_;
      if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
      }

      return out;
    }

    std::stringstream &operator<<(std::stringstream &out, const PrintStage &t)
    {
      out << "label = " << t.label_;
      if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
      }

      return out;
    }
  }
}
