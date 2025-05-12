////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "dag_scheduler/Task.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <boost/dll/alias.hpp>
#include <iostream>
#include <memory>

#include "dag_scheduler/Logging.h"
#include "dag_scheduler/TaskCallbackPlugin.h"

void defaultTaskCallback(bool dummy) noexcept {
    (void)dummy;
    std::cout << "Default task callback called." << std::endl;
}

BOOST_DLL_ALIAS_SECTIONED(defaultTaskCallback, task_callback, TaskCb)

namespace com::dag_scheduler {
Task::Task() : LoggedClass<Task>(*this), iterating_(false), kill_(false) {
    label_ = uuid_.asString();
    rapidjson::Document json_empty;
    setJsonConfig(json_empty);
    setJsonInitialInputs(json_empty);
}

Task::Task(std::vector<std::unique_ptr<TaskStage>>& stages)
        : Task(stages, "") {
    label_ = uuid_.asString();
    rapidjson::Document json_empty;
    setJsonConfig(json_empty);
    setJsonInitialInputs(json_empty);
}

Task::Task(std::vector<std::unique_ptr<TaskStage>>& stages,
           const rapidjson::Document& json_initial_inputs)
        : Task(stages, "") {
    label_ = uuid_.asString();
    rapidjson::Document json_empty;
    setJsonConfig(json_empty);
    setJsonInitialInputs(json_initial_inputs);
}

Task::Task(std::vector<std::unique_ptr<TaskStage>>& stages,
           const std::string& label)
        : LoggedClass<Task>(*this)
        , iterating_(false)
        , kill_(false)
        , stages_(std::move(stages))
        , label_(label) {
    rapidjson::Document json_empty;
    setJsonConfig(json_empty);
    setJsonInitialInputs(json_empty);
}

Task::Task(std::vector<std::unique_ptr<TaskStage>>& stages,
           const std::string& label,
           const rapidjson::Document& json_initial_inputs)
        : LoggedClass<Task>(*this)
        , iterating_(false)
        , kill_(false)
        , stages_(std::move(stages))
        , label_(label) {
    rapidjson::Document json_empty;
    setJsonConfig(json_empty);
    setJsonInitialInputs(json_initial_inputs);
}

Task::Task(std::vector<std::unique_ptr<TaskStage>>& stages,
           const std::string& label,
           std::function<void(bool)> complete_callback)
        : LoggedClass<Task>(*this)
        , iterating_(false)
        , kill_(false)
        , stages_(std::move(stages))
        , label_(label)
        , complete_callback_(complete_callback) {
    rapidjson::Document json_empty;
    setJsonConfig(json_empty);
    setJsonInitialInputs(json_empty);
}

Task::Task(std::vector<std::unique_ptr<TaskStage>>& stages,
           const std::string& label,
           std::function<void(bool)> complete_callback,
           const rapidjson::Document& json_initial_inputs)
        : LoggedClass<Task>(*this)
        , iterating_(false)
        , kill_(false)
        , stages_(std::move(stages))
        , label_(label)
        , complete_callback_(complete_callback) {
    rapidjson::Document json_empty;
    setJsonConfig(json_empty);
    setJsonInitialInputs(json_initial_inputs);
}

Task::Task(std::vector<std::unique_ptr<TaskStage>>& stages,
           const std::string& label,
           std::unique_ptr<TaskCallbackPlugin>&& complete_callback_plugin)
        : LoggedClass<Task>(*this)
        , iterating_(false)
        , kill_(false)
        , stages_(std::move(stages))
        , label_(label)
        , complete_callback_plugin_(std::move(complete_callback_plugin)) {
    rapidjson::Document json_empty;
    setJsonConfig(json_empty);
    setJsonInitialInputs(json_empty);
}

Task::Task(std::vector<std::unique_ptr<TaskStage>>& stages,
           const std::string& label,
           std::unique_ptr<TaskCallbackPlugin>&& complete_callback_plugin,
           const rapidjson::Document& json_initial_inputs)
        : LoggedClass<Task>(*this)
        , iterating_(false)
        , kill_(false)
        , stages_(std::move(stages))
        , label_(label)
        , complete_callback_plugin_(std::move(complete_callback_plugin)) {
    rapidjson::Document json_empty;
    setJsonConfig(json_empty);
    setJsonInitialInputs(json_initial_inputs);
}

Task::Task(std::vector<std::unique_ptr<TaskStage>>& stages,
           const rapidjson::Document& json_config,
           const rapidjson::Document& json_initial_inputs)
        : LoggedClass<Task>(*this)
        , iterating_(false)
        , kill_(false)
        , stages_(std::move(stages)) {
    setJsonConfig(json_config);
    setJsonInitialInputs(json_initial_inputs);
}

Task::Task(std::vector<std::unique_ptr<TaskStage>>& stages,
           const std::string& label,
           const rapidjson::Document& json_config,
           const rapidjson::Document& json_initial_inputs)
        : LoggedClass<Task>(*this)
        , iterating_(false)
        , kill_(false)
        , stages_(std::move(stages))
        , label_(label) {
    setJsonConfig(json_config);
    setJsonInitialInputs(json_initial_inputs);
}

Task::Task(std::vector<std::unique_ptr<TaskStage>>& stages,
           const std::string& label,
           std::function<void(bool)> complete_callback,
           const rapidjson::Document& json_config,
           const rapidjson::Document& json_initial_inputs)
        : LoggedClass<Task>(*this)
        , iterating_(false)
        , kill_(false)
        , stages_(std::move(stages))
        , label_(label)
        , complete_callback_(complete_callback) {
    setJsonConfig(json_config);
    setJsonInitialInputs(json_initial_inputs);
}

Task::Task(std::vector<std::unique_ptr<TaskStage>>& stages,
           const std::string& label,
           std::unique_ptr<TaskCallbackPlugin>&& complete_callback_plugin,
           const rapidjson::Document& json_config,
           const rapidjson::Document& json_initial_inputs)
        : LoggedClass<Task>(*this)
        , iterating_(false)
        , kill_(false)
        , stages_(std::move(stages))
        , label_(label)
        , complete_callback_plugin_(std::move(complete_callback_plugin)) {
    setJsonConfig(json_config);
    setJsonInitialInputs(json_initial_inputs);
}

Task::~Task() {}

Task::Task(Task&& other)
        : LoggedClass<Task>(*this)
        , iterating_(false)
        , kill_(false)
        , stages_(std::move(other.stages_))
        , label_(std::move(other.label_))
        , uuid_(other.uuid_.clone())
        , json_config_(std::move(other.json_config_))
        , json_initial_inputs_(std::move(other.json_initial_inputs_)) {
    assert(!other.iterating_.load() &&
           "You cannot move an iterating"
           "task");
}

Task& Task::operator=(Task&& other) {
    assert(!other.iterating_.load() &&
           "You cannot move an iterating"
           "task");

    iterating_.store(false);
    kill_.store(false);
    stages_ = std::move(other.stages_);
    label_ = std::move(other.label_);
    uuid_ = other.uuid_.clone();
    json_config_ = std::move(other.json_config_);
    json_initial_inputs_ = std::move(other.json_initial_inputs_);

    return (*this);
}

const std::string& Task::label() const { return label_; }

const UUID& Task::getUUID() const { return uuid_; }

bool Task::iterateStages(const std::function<bool(TaskStage&)>& next_stage) {
    bool ran_all = false;

    if (!iterating_.load()) {
        iterating_.store(true);
        {
            ran_all = std::all_of(stages_.begin(),
                                  stages_.end(),
                                  [&](class std::unique_ptr<TaskStage>& next) {
                                      bool has_ran = next_stage(*next);
                                      next->cleanup();
                                      bool ret_val = (has_ran && next->end() &&
                                                      (!kill_.load()));
                                      return ret_val;
                                  });
        }
        iterating_.store(false);
    }

    return ran_all;
}

bool Task::kill() {
    kill_.store(true);
    return kill_.load();
}

void Task::complete(bool status) {
    if (callbackIsSet()) {
        Logging::info(LOG_TAG, "Going to call user specified function...");
        complete_callback_(status);
    }

    if (callbackPluginIsSet()) {
        Logging::info(
                LOG_TAG, "Going to call user specified callback", "plugin...");
        complete_callback_plugin_->completed(status, (*this));
    }
}

bool Task::callbackIsSet() const {
    bool ret_val = false;
    if (complete_callback_) {
        ret_val = true;
    }
    return ret_val;
}

bool Task::callbackPluginIsSet() const {
    return complete_callback_plugin_ != nullptr;
}

const rapidjson::Document& Task::jsonConfig() const { return (*json_config_); }

void Task::jsonConfigStr(std::string& out_str) const {
    if (json_config_) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json_config_->Accept(writer);
        out_str = buffer.GetString();
        if (out_str == "null") {
            out_str = "{}";
        }
    }
}

void Task::jsonInitialInputsStr(std::string& out_str) const {
    if (json_initial_inputs_) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json_initial_inputs_->Accept(writer);
        out_str = buffer.GetString();
        if (out_str == "null") {
            out_str = "{}";
        }
    }
}

std::unique_ptr<Task> Task::clone() const {
    std::vector<std::unique_ptr<TaskStage>> cloned_stages;
    std::for_each(
            stages_.begin(),
            stages_.end(),
            [&cloned_stages](const std::unique_ptr<TaskStage>& next_stage) {
                std::unique_ptr<TaskStage>&& task_stage_ptr =
                        next_stage->clone();
                cloned_stages.push_back(std::move(task_stage_ptr));
            });

    rapidjson::Document json_config;
    if (json_config_) {
        rapidjson::Document& this_document = *(json_config_);
        json_config.CopyFrom(this_document, json_config.GetAllocator());
    }

    rapidjson::Document json_initial_inputs;
    if (json_initial_inputs_) {
        rapidjson::Document& this_document = *(json_initial_inputs_);
        json_initial_inputs.CopyFrom(this_document,
                                     json_initial_inputs.GetAllocator());
    }

    std::unique_ptr<Task> task_ptr;
    if (complete_callback_plugin_) {
        std::unique_ptr<TaskCallbackPlugin>&& callback_plugin =
                complete_callback_plugin_->clone();
        task_ptr = std::make_unique<Task>(cloned_stages,
                                          label_,
                                          std::move(callback_plugin),
                                          json_config,
                                          json_initial_inputs);
    } else {
        task_ptr = std::make_unique<Task>(cloned_stages,
                                          label_,
                                          complete_callback_,
                                          json_config,
                                          json_initial_inputs);
    }

    task_ptr->updateUUID(uuid_);

    return task_ptr;
}

bool operator==(const Task& lhs, const Task& rhs) {
    return lhs.uuid_ == rhs.uuid_;
}

bool operator!=(const Task& lhs, const Task& rhs) { return !(lhs == rhs); }

std::ostream& operator<<(std::ostream& out, const Task& task) {
    out << "label = " << task.label_;
    if (task.label_ != task.uuid_.asString()) {
        out << " uuid = " << task.uuid_;
    }
    std::for_each(task.stages_.begin(),
                  task.stages_.end(),
                  [&](const std::unique_ptr<TaskStage>& stage) {
                      out << (*stage) << " ";
                  });
    std::string json_config_string;
    task.jsonConfigStr(json_config_string);
    out << "configuration = " << json_config_string;
    std::string json_initial_inputs_string;
    task.jsonConfigStr(json_initial_inputs_string);
    out << "initial_json_inputs = " << json_initial_inputs_string;

    return out;
}

std::stringstream& operator<<(std::stringstream& out, const Task& task) {
    out << "label = " << task.label_;
    if (task.label_ != task.uuid_.asString()) {
        out << " uuid = " << task.uuid_;
    }
    std::for_each(task.stages_.begin(),
                  task.stages_.end(),
                  [&](const std::unique_ptr<TaskStage>& stage) {
                      out << (*stage) << " ";
                  });
    std::string json_config_string;
    task.jsonConfigStr(json_config_string);
    out << "configuration:" << json_config_string;
    std::string json_initial_inputs_string;
    task.jsonConfigStr(json_initial_inputs_string);
    out << "initial_json_inputs = " << json_initial_inputs_string;

    return out;
}

void Task::setJsonConfig(const rapidjson::Document& json_config) {
    if (json_config_) {
        json_config_.reset();
    }
    json_config_ = std::make_unique<rapidjson::Document>();
    json_config_->CopyFrom(json_config, json_config_->GetAllocator());
}

void Task::setJsonInitialInputs(
        const rapidjson::Document& json_initial_inputs) {
    if (json_initial_inputs_) {
        json_initial_inputs_.reset();
    }
    json_initial_inputs_ = std::make_unique<rapidjson::Document>();
    json_initial_inputs_->CopyFrom(json_initial_inputs,
                                   json_initial_inputs_->GetAllocator());
}

void Task::updateUUID(const UUID& uuid) { uuid_ = UUID(uuid.asString()); }
}  // namespace com::dag_scheduler
