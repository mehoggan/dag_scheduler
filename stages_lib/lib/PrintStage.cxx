#include "stages_lib/PrintStage.h"

#include <boost/dll/alias.hpp>
#include <iostream>
#include <memory>

#include "dag_scheduler/Logging.h"

BOOST_DLL_ALIAS_SECTIONED(com::stages_lib::PrintStage::make_stage,
                          print_stage,
                          Stages)

namespace com::stages_lib {
PrintStage::PrintStage() : PrintStage("") { label_ = uuid_.as_string(); }

PrintStage::PrintStage(const std::string& label)
        : com::dag_scheduler::TaskStage(label) {}

PrintStage::PrintStage(const std::string& label,
                       const dag_scheduler::UUID& uuid)
        : TaskStage(label, uuid) {}

PrintStage::~PrintStage() {}

PrintStage::PrintStage(PrintStage&& other) {
    assert(!other.is_running() && "You cannot move a running PrintStage");
    label_ = std::move(other.label_);
    uuid_ = std::move(other.uuid_);
}

PrintStage& PrintStage::operator=(PrintStage&& other) {
    assert(!other.is_running() && "You cannot move a running PrintStage");

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

bool PrintStage::is_running() const { return running_.load(); }

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

std::ostream& operator<<(std::ostream& out, const PrintStage& t) {
    out << "label = " << t.label_;
    if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
    }

    return out;
}

std::stringstream& operator<<(std::stringstream& out, const PrintStage& t) {
    out << "label = " << t.label_;
    if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
    }

    return out;
}

std::unique_ptr<com::dag_scheduler::TaskStage> PrintStage::make_stage(
        const std::string& name) {
    return std::make_unique<PrintStage>(name);
}
}  // namespace com::stages_lib
