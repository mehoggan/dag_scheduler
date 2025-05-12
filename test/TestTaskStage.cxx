////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "utils/TestTaskStage.h"

#include <gtest/gtest.h>

#include "dag_scheduler/LoggedClass.hpp"
#include "dag_scheduler/TaskStage.h"

namespace com::dag_scheduler {
class TestTaskStage : public ::testing::Test,
                      public LoggedClass<TestTaskStage> {
public:
    TestTaskStage() : LoggedClass<TestTaskStage>(*this) {}

protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TestTaskStage, defaultCtor) {
    std::unique_ptr<TaskStage> test(new TestTaskStageImpl);
    EXPECT_NE(nullptr, test);
    EXPECT_EQ(test->getUUID().asString(), test->label());
}

TEST_F(TestTaskStage, labelCtor) {
    std::unique_ptr<TaskStage> test(new TestTaskStageImpl("test_label"));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->getUUID().asString(), test->label());
    EXPECT_EQ("test_label", test->label());
}

TEST_F(TestTaskStage, run) {
    TestTaskStageImpl test_stage;
    EXPECT_TRUE(test_stage.run());

    std::unique_ptr<TaskStage> tt_ptr(new TestTaskStageImpl);
    EXPECT_TRUE(tt_ptr->run());
}

TEST_F(TestTaskStage, killIfIsRunning) {
    TestTaskStageImpl test_task;
    EXPECT_TRUE(test_task.end());

    std::unique_ptr<TaskStage> tt_ptr(new TestTaskStageImpl);
    EXPECT_TRUE(tt_ptr->end());
    EXPECT_FALSE(tt_ptr->isRunning());
    EXPECT_TRUE(tt_ptr->run());
    EXPECT_FALSE(tt_ptr->isRunning());
    EXPECT_TRUE(tt_ptr->end());
}
}  // namespace com::dag_scheduler
