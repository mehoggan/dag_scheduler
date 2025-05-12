////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "stages_lib/PrintStage.h"

#include <boost/dll/alias.hpp>
#include <iostream>
#include <memory>

BOOST_DLL_ALIAS_SECTIONED(com::stages_lib::PrintStage::makeStage,
                          print_stage,
                          Stages)

namespace com::stages_lib {
PrintStage::PrintStage() : PrintStage("") { label_ = uuid_.asString(); }

PrintStage::PrintStage(const std::string& label)
        : com::dag_scheduler::TaskStage(label) {}

PrintStage::PrintStage(const std::string& label,
                       const dag_scheduler::UUID& uuid)
        : TaskStage(label, uuid) {}

PrintStage::~PrintStage() {}

PrintStage::PrintStage(PrintStage&& other) {
    assert(!other.isRunning() && "You cannot move a running PrintStage");
    label_ = std::move(other.label_);
    uuid_ = std::move(other.uuid_);
}

PrintStage& PrintStage::operator=(PrintStage&& other) {
    assert(!other.isRunning() && "You cannot move a running PrintStage");

    label_ = std::move(other.label_);
    uuid_ = std::move(other.uuid_);

    return (*this);
}

bool PrintStage::run() {
    running_.store(true);
    std::cout << "Print Stage is running..." << std::endl;
    running_.store(false);
    return true;
}

bool PrintStage::isRunning() const { return running_.load(); }

bool PrintStage::end() { return true; }

void PrintStage::cleanup() {}

std::unique_ptr<dag_scheduler::TaskStage> PrintStage::clone() const {
    std::unique_ptr<dag_scheduler::TaskStage> print_stage_ptr =
            std::make_unique<PrintStage>(label_, uuid_);
    return print_stage_ptr;
}

bool operator==(const PrintStage& lhs, const PrintStage& rhs) {
    return lhs.uuid_ == rhs.uuid_;
}

bool operator!=(const PrintStage& lhs, const PrintStage& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out, const PrintStage& stage) {
    out << "label = " << stage.label_;
    if (stage.label_ != stage.uuid_.asString()) {
        out << " uuid = " << stage.uuid_;
    }

    return out;
}

std::stringstream& operator<<(std::stringstream& out,
                              const PrintStage& stage) {
    out << "label = " << stage.label_;
    if (stage.label_ != stage.uuid_.asString()) {
        out << " uuid = " << stage.uuid_;
    }

    return out;
}

std::unique_ptr<com::dag_scheduler::TaskStage> PrintStage::makeStage(
        const std::string& name) {
    return std::make_unique<PrintStage>(name);
}
}  // namespace com::stages_lib
