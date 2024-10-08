#include "dag_scheduler/base_task_stage.h"

#include "dag_scheduler/logging.h"

namespace com {
  namespace dag_scheduler {
    BaseTaskStage::BaseTaskStage() : BaseTaskStage("") {
      label_ = uuid_.as_string();
    }

    BaseTaskStage::BaseTaskStage(const std::string &label) : label_(label) {}

    BaseTaskStage::~BaseTaskStage() {}

    BaseTaskStage::BaseTaskStage(BaseTaskStage &&other) :
      label_(std::move(other.label_)), uuid_(std::move(other.uuid_)) {
      assert(
        not other.is_running() && "You cannot move a running BaseTaskStage"
      );
    }

    BaseTaskStage &BaseTaskStage::operator=(BaseTaskStage &&other) {
      assert(
        not other.is_running() && "You cannot move a running BaseTaskStage"
      );

      label_ = std::move(other.label_);
      uuid_ = std::move(other.uuid_);

      return (*this);
    }

    const std::string &BaseTaskStage::label() const { return label_; }

    const UUID &BaseTaskStage::get_uuid() const { return uuid_; }

    bool BaseTaskStage::run() { return true; }

    bool BaseTaskStage::is_running() const { return false; }

    bool BaseTaskStage::end() { return false; }

    void BaseTaskStage::cleanup() {}

    bool operator==(const BaseTaskStage &lhs, const BaseTaskStage &rhs) {
      return lhs.uuid_ == rhs.uuid_;
    }

    bool operator!=(const BaseTaskStage &lhs, const BaseTaskStage &rhs) {
      return !(lhs == rhs);
    }

    std::ostream &operator<<(std::ostream &out, const BaseTaskStage &t) {
      out << "label = " << t.label_;
      if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
      }

      return out;
    }

    std::stringstream &
    operator<<(std::stringstream &out, const BaseTaskStage &t) {
      out << "label = " << t.label_;
      if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
      }

      return out;
    }
  } // namespace dag_scheduler
} // namespace com
