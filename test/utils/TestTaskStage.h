////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef TEST_TASK_STAGE_H_INCLUDED
#define TEST_TASK_STAGE_H_INCLUDED

#include <atomic>
#include <chrono>

#include "dag_scheduler/LoggedClass.hpp"
#include "dag_scheduler/TaskStage.h"

namespace com::dag_scheduler {
class TestTaskStageImpl : public TaskStage {
public:
    TestTaskStageImpl();

    explicit TestTaskStageImpl(const std::string& label,
                               const std::chrono::nanoseconds& run_sleep_time =
                                       std::chrono::nanoseconds(100000000));

    TestTaskStageImpl(const std::string& label,
                      const UUID& uuid,
                      const std::chrono::nanoseconds& run_sleep_time =
                              std::chrono::nanoseconds(100000000));

    ~TestTaskStageImpl() override;

    TestTaskStageImpl(const TestTaskStageImpl&) = delete;

    TestTaskStageImpl& operator=(const TestTaskStageImpl&) = delete;

    TestTaskStageImpl(TestTaskStageImpl&& other);

    TestTaskStageImpl& operator=(TestTaskStageImpl&& other);

    bool run() override;

    bool is_running() const override;

    bool end() override;

    void cleanup() override;

    std::unique_ptr<TaskStage> clone() const override;

private:
    std::atomic_bool running_;
    int* nasty_user_defined_pointer_;
    std::chrono::nanoseconds run_sleep_time_;
};
}  // namespace com::dag_scheduler
#endif
