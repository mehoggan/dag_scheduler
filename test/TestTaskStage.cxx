#include "utils/TestTaskStage.h"

#include <gtest/gtest.h>

#include <atomic>

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

TEST_F(TestTaskStage, default_ctor) {
    std::unique_ptr<TaskStage> test(new TestTaskStageImpl);
    EXPECT_NE(nullptr, test);
    EXPECT_EQ(test->get_uuid().as_string(), test->label());
}

TEST_F(TestTaskStage, label_ctor) {
    std::unique_ptr<TaskStage> test(new TestTaskStageImpl("test_label"));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->get_uuid().as_string(), test->label());
    EXPECT_EQ("test_label", test->label());
}

TEST_F(TestTaskStage, run) {
    TestTaskStageImpl tt;
    EXPECT_TRUE(tt.run());

    std::unique_ptr<TaskStage> tt_ptr(new TestTaskStageImpl);
    EXPECT_TRUE(tt_ptr->run());
}

TEST_F(TestTaskStage, kill_if_is_running) {
    TestTaskStageImpl tt;
    EXPECT_TRUE(tt.end());

    std::unique_ptr<TaskStage> tt_ptr(new TestTaskStageImpl);
    EXPECT_TRUE(tt_ptr->end());
    EXPECT_FALSE(tt_ptr->is_running());
    EXPECT_TRUE(tt_ptr->run());
    EXPECT_FALSE(tt_ptr->is_running());
    EXPECT_TRUE(tt_ptr->end());
}
}  // namespace com::dag_scheduler
