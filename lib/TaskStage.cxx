////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "dag_scheduler/TaskStage.h"

#include <cassert>

namespace com::dag_scheduler {
TaskStage::TaskStage() : TaskStage("") { label_ = uuid_.asString(); }

TaskStage::TaskStage(const std::string& label) : label_(label) {}

TaskStage::~TaskStage() {}

TaskStage::TaskStage(TaskStage&& other)
        : label_(std::move(other.label_)), uuid_(std::move(other.uuid_)) {
    assert(!other.isRunning() && "You cannot move a running TaskStage");
}

TaskStage& TaskStage::operator=(TaskStage&& other) {
    assert(!other.isRunning() && "You cannot move a running TaskStage");

    label_ = std::move(other.label_);
    uuid_ = std::move(other.uuid_);

    return (*this);
}

const std::string& TaskStage::label() const { return label_; }

const UUID& TaskStage::getUUID() const { return uuid_; }

bool operator==(const TaskStage& lhs, const TaskStage& rhs) {
    return lhs.uuid_ == rhs.uuid_;
}

bool operator!=(const TaskStage& lhs, const TaskStage& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out, const TaskStage& task) {
    out << "label = " << task.label_;
    if (task.label_ != task.uuid_.asString()) {
        out << " uuid = " << task.uuid_;
    }

    return out;
}

std::stringstream& operator<<(std::stringstream& out,
                              const TaskStage& task_stage) {
    out << "label = " << task_stage.label_;
    if (task_stage.label_ != task_stage.uuid_.asString()) {
        out << " uuid = " << task_stage.uuid_;
    }

    return out;
}

TaskStage::TaskStage(const std::string& label, const UUID& uuid)
        : label_(label), uuid_(uuid.asString()) {}
}  // namespace com::dag_scheduler
