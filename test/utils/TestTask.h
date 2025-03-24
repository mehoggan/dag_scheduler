////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef TEST_DAG_SCHEDULER_TEST_TASK_H
#define TEST_DAG_SCHEDULER_TEST_TASK_H

#include <gtest/gtest_prod.h>
#include <rapidjson/document.h>

#include <atomic>
#include <functional>

#include "dag_scheduler/LoggedClass.hpp"
#include "dag_scheduler/Task.h"
#include "dag_scheduler/TaskCallbackPlugin.h"

namespace com::dag_scheduler {
class TestTaskImpl : public Task {
private:
    FRIEND_TEST(TestTask,
                iterate_stages_succeeds_if_all_stages_ran_no_kill_task);

public:
    TestTaskImpl();

    explicit TestTaskImpl(const std::string& label);

    TestTaskImpl(const std::string& label,
                 const rapidjson::Document& json_initial_inputs);

    TestTaskImpl(const std::string& label,
                 std::function<void(bool)> complete_callback);

    TestTaskImpl(const std::string& label,
                 const rapidjson::Document& json_initial_inputs,
                 std::function<void(bool)> complete_callback);

    TestTaskImpl(
            const std::string& label,
            std::unique_ptr<TaskCallbackPlugin>&& complete_callback_plugin);

    TestTaskImpl(
            const std::string& label,
            const rapidjson::Document& json_initial_inputs,
            std::unique_ptr<TaskCallbackPlugin>&& complete_callback_plugin);

    TestTaskImpl(const std::string& label,
                 const rapidjson::Document& json_config,
                 const rapidjson::Document& json_initial_inputs);

    TestTaskImpl(const std::string& label,
                 const rapidjson::Document& json_config,
                 const rapidjson::Document& json_initial_inputs,
                 std::function<void(bool)> complete_callback);

    TestTaskImpl(
            const std::string& label,
            const rapidjson::Document& json_config,
            const rapidjson::Document& json_initial_inputs,
            std::unique_ptr<TaskCallbackPlugin>&& complete_callback_plugin);

    ~TestTaskImpl() override;

    TestTaskImpl(TestTaskImpl&& other);

    TestTaskImpl& operator=(TestTaskImpl&& other);
};
}  // namespace com::dag_scheduler
#endif
