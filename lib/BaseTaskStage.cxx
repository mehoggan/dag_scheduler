////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "dag_scheduler/BaseTaskStage.h"

#include "dag_scheduler/Logging.h"

namespace com::dag_scheduler {
BaseTaskStage::BaseTaskStage() : BaseTaskStage("") {
    label_ = uuid_.as_string();
}

BaseTaskStage::BaseTaskStage(const std::string& label) : label_(label) {}

BaseTaskStage::~BaseTaskStage() {}

BaseTaskStage::BaseTaskStage(BaseTaskStage&& other) noexcept
        : label_(std::move(other.label_)), uuid_(std::move(other.uuid_)) {
    assert(!other.is_running() && "You cannot move a running BaseTaskStage");
}

BaseTaskStage& BaseTaskStage::operator=(BaseTaskStage&& other) noexcept {
    assert(!other.is_running() && "You cannot move a running BaseTaskStage");

    label_ = std::move(other.label_);
    uuid_ = std::move(other.uuid_);

    return (*this);
}

const std::string& BaseTaskStage::label() const { return label_; }

const UUID& BaseTaskStage::get_uuid() const { return uuid_; }

bool BaseTaskStage::run() { return true; }

bool BaseTaskStage::is_running() const { return false; }

bool BaseTaskStage::end() { return false; }

void BaseTaskStage::cleanup() {}

bool operator==(const BaseTaskStage& lhs, const BaseTaskStage& rhs) {
    return lhs.uuid_ == rhs.uuid_;
}

bool operator!=(const BaseTaskStage& lhs, const BaseTaskStage& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out, const BaseTaskStage& stage) {
    out << "label = " << stage.label_;
    if (stage.label_ != stage.uuid_.as_string()) {
        out << " uuid = " << stage.uuid_;
    }

    return out;
}

std::stringstream& operator<<(std::stringstream& out,
                              const BaseTaskStage& stage) {
    out << "label = " << stage.label_;
    if (stage.label_ != stage.uuid_.as_string()) {
        out << " uuid = " << stage.uuid_;
    }

    return out;
}
}  // namespace com::dag_scheduler
