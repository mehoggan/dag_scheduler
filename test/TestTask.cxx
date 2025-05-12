////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "utils/TestTask.h"

#include <gtest/gtest.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "dag_scheduler/LoggedClass.hpp"
#include "dag_scheduler/Task.h"
#include "dag_scheduler/TaskCallbackPlugin.h"

namespace com::dag_scheduler {
class TestTask : public ::testing::Test, public LoggedClass<TestTask> {
public:
    TestTask() : LoggedClass<TestTask>(*this) {}

    static void getGenericConfig(rapidjson::Document& config_doc) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        writer.StartObject();
        writer.String("test_value_int");
        writer.Int(-1);
        writer.String("test_value_str");
        writer.String("test_string");
        writer.String("test_value_bool");
        writer.Bool(true);
        writer.String("test_value_double");
        writer.Double(-1.0);
        writer.EndObject();
        config_doc.Parse(buffer.GetString());
    }

    static std::string getExpectedConfigStr() {
        return std::string("{") + std::string("\"test_value_int\":-1,") +
               std::string("\"test_value_str\":\"test_string\",") +
               std::string("\"test_value_bool\":true,") +
               std::string("\"test_value_double\":-1.0") + std::string("}");
    }

    static void getGenericInitialInputs(
            rapidjson::Document& initial_inputs_doc) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        writer.StartObject();
        writer.String("test_value_int");
        writer.Int(-1);
        writer.String("test_value_str");
        writer.String("test_string");
        writer.String("test_value_bool");
        writer.Bool(true);
        writer.String("test_value_double");
        writer.Double(-1.0);
        writer.EndObject();
        initial_inputs_doc.Parse(buffer.GetString());
    }

    static std::string getExpectedGenericInitialInputsStr() {
        return std::string("{") + std::string("\"test_value_int\":-1,") +
               std::string("\"test_value_str\":\"test_string\",") +
               std::string("\"test_value_bool\":true,") +
               std::string("\"test_value_double\":-1.0") + std::string("}");
    }

protected:
    void SetUp() override {}
    void TearDown() override {}
};

class TestTaskCallbackPlugin : public TaskCallbackPlugin,
                               public ::testing::Test,
                               public LoggedClass<TestTaskCallbackPlugin> {
public:
    TestTaskCallbackPlugin() : LoggedClass<TestTaskCallbackPlugin>(*this) {}

    ~TestTaskCallbackPlugin() override {}

    void completed(bool completed, Task& task) override {
        Logging::info(LOG_TAG,
                      "completed called with",
                      (completed ? "true" : "false"),
                      "and",
                      task);
    }

    std::unique_ptr<TaskCallbackPlugin> clone() const override {
        auto cloned_callback = std::make_unique<TestTaskCallbackPlugin>();
        return cloned_callback;
    }

    void TestBody() override {}
};

TEST_F(TestTask, default_ctor) {
    std::unique_ptr<Task> test(new TestTaskImpl);
    EXPECT_NE(nullptr, test);
    EXPECT_EQ(test->getUUID().asString(), test->label());
    {
        std::string empty_config;
        test->jsonConfigStr(empty_config);
        EXPECT_EQ("{}", empty_config);
    }

    {
        std::string empty_initial_json_inputs;
        test->jsonInitialInputsStr(empty_initial_json_inputs);
        EXPECT_EQ("{}", empty_initial_json_inputs);
    }
}

TEST_F(TestTask, default_ctor_clone) {
    std::unique_ptr<Task> test(new TestTaskImpl);
    std::unique_ptr<Task>&& test_clone = test->clone();
    EXPECT_NE(nullptr, test_clone);
    EXPECT_EQ(test_clone->getUUID().asString(), test->getUUID().asString());
    {
        std::string empty_config;
        test->jsonConfigStr(empty_config);
        EXPECT_EQ("{}", empty_config);
    }

    {
        std::string empty_initial_json_inputs;
        test->jsonInitialInputsStr(empty_initial_json_inputs);
        EXPECT_EQ("{}", empty_initial_json_inputs);
    }
}

TEST_F(TestTask, label_ctor) {
    std::unique_ptr<Task> test(new TestTaskImpl("test_label"));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->getUUID().asString(), test->label());
    EXPECT_EQ("test_label", test->label());
    {
        std::string empty_config;
        test->jsonConfigStr(empty_config);
        EXPECT_EQ("{}", empty_config);
    }

    {
        std::string empty_initial_json_inputs;
        test->jsonInitialInputsStr(empty_initial_json_inputs);
        EXPECT_EQ("{}", empty_initial_json_inputs);
    }
}

TEST_F(TestTask, label_ctor_with_inputs) {
    rapidjson::Document initial_inputs;
    TestTask::getGenericInitialInputs(initial_inputs);
    std::unique_ptr<Task> test(new TestTaskImpl("test_label", initial_inputs));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->getUUID().asString(), test->label());
    EXPECT_EQ("test_label", test->label());
    {
        std::string empty_config;
        test->jsonConfigStr(empty_config);
        EXPECT_EQ("{}", empty_config);
    }

    {
        std::string json_inputs;
        test->jsonInitialInputsStr(json_inputs);
        std::string expected_initial_json_inputs =
                TestTask::getExpectedGenericInitialInputsStr();
        EXPECT_EQ(expected_initial_json_inputs, json_inputs);
    }
}

TEST_F(TestTask, label_ctor_clone) {
    std::unique_ptr<Task> test(new TestTaskImpl("test_label"));
    std::unique_ptr<Task>&& test_clone = test->clone();
    EXPECT_NE(nullptr, test_clone);
    EXPECT_EQ(test_clone->getUUID().asString(), test->getUUID().asString());
    EXPECT_EQ("test_label", test_clone->label());
    std::string empty_config;
    test_clone->jsonConfigStr(empty_config);
    EXPECT_EQ("{}", empty_config);
}

TEST_F(TestTask, call_back_ctor_sets_appropiate_callback) {
    std::function<void(bool)> complete_callback = [](bool status) {
        EXPECT_TRUE(status);
    };
    std::unique_ptr<Task> test(
            new TestTaskImpl("test_label", complete_callback));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->getUUID().asString(), test->label());
    EXPECT_EQ("test_label", test->label());
    test->complete(true);
    std::string empty_config;
    test->jsonConfigStr(empty_config);
    EXPECT_EQ("{}", empty_config);
}

TEST_F(TestTask, call_back_ctor_sets_appropiate_callback_clone) {
    std::function<void(bool)> complete_callback = [](bool status) {
        EXPECT_TRUE(status);
    };
    std::unique_ptr<Task> test(
            new TestTaskImpl("test_label", complete_callback));
    std::unique_ptr<Task>&& test_clone = test->clone();
    EXPECT_NE(nullptr, test_clone);
    EXPECT_EQ(test_clone->getUUID().asString(), test->getUUID().asString());
    EXPECT_EQ("test_label", test_clone->label());
    test_clone->complete(true);
    std::string empty_config;
    test_clone->jsonConfigStr(empty_config);
    EXPECT_EQ("{}", empty_config);
}

TEST_F(TestTask, call_back_ctor_sets_appropiate_callback_plugin) {
    std::unique_ptr<TaskCallbackPlugin> complete_callback_plugin =
            std::make_unique<TestTaskCallbackPlugin>();
    std::unique_ptr<Task> test(new TestTaskImpl(
            "test_label", std::move(complete_callback_plugin)));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->getUUID().asString(), test->label());
    EXPECT_EQ("test_label", test->label());
    EXPECT_TRUE(test->callbackPluginIsSet());
    test->complete(true);
    std::string empty_config;
    test->jsonConfigStr(empty_config);
    EXPECT_EQ("{}", empty_config);
}

TEST_F(TestTask, call_back_ctor_sets_appropiate_callback_plugin_clone) {
    std::unique_ptr<TaskCallbackPlugin> complete_callback_plugin =
            std::make_unique<TestTaskCallbackPlugin>();
    std::unique_ptr<Task> test(new TestTaskImpl(
            "test_label", std::move(complete_callback_plugin)));
    std::unique_ptr<Task>&& test_clone = test->clone();
    EXPECT_NE(nullptr, test_clone);
    EXPECT_EQ(test_clone->getUUID().asString(), test->getUUID().asString());
    EXPECT_EQ("test_label", test_clone->label());
    EXPECT_TRUE(test_clone->callbackPluginIsSet());
    test_clone->complete(true);
    std::string empty_config;
    test_clone->jsonConfigStr(empty_config);
    EXPECT_EQ("{}", empty_config);
}

TEST_F(TestTask, label_ctor_with_initial_inputs) {
    rapidjson::Document json_initial_inputs;
    TestTask::getGenericConfig(json_initial_inputs);
    std::unique_ptr<Task> test(
            new TestTaskImpl("test_label", json_initial_inputs));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->getUUID().asString(), test->label());
    EXPECT_EQ("test_label", test->label());
    std::string actual_initial_inputs;
    test->jsonInitialInputsStr(actual_initial_inputs);
    std::string expected_initial_json_inputs =
            TestTask::getExpectedGenericInitialInputsStr();
    EXPECT_EQ(actual_initial_inputs, expected_initial_json_inputs);
}

TEST_F(TestTask, label_ctor_with_initial_inputs_clone) {
    rapidjson::Document json_initial_inputs;
    TestTask::getGenericInitialInputs(json_initial_inputs);
    std::unique_ptr<Task> test(
            new TestTaskImpl("test_label", json_initial_inputs));
    std::unique_ptr<Task>&& test_clone = test->clone();
    EXPECT_NE(nullptr, test_clone);
    EXPECT_EQ(test_clone->getUUID().asString(), test->getUUID().asString());
    EXPECT_EQ("test_label", test_clone->label());
    std::string actual_initial_inputs;
    test_clone->jsonInitialInputsStr(actual_initial_inputs);
    std::string expected_initial_json_inputs =
            TestTask::getExpectedGenericInitialInputsStr();
    EXPECT_EQ(actual_initial_inputs, expected_initial_json_inputs);
}

TEST_F(TestTask,
       call_back_ctor_sets_appropiate_callback_with_initial_inputs_empty_) {
    rapidjson::Document json_config;
    TestTask::getGenericConfig(json_config);
    rapidjson::Document json_initial_inputs;
    TestTask::getGenericInitialInputs(json_initial_inputs);
    std::function<void(bool)> complete_callback = [](bool status) {
        EXPECT_TRUE(status);
    };
    std::unique_ptr<Task> test(new TestTaskImpl("test_label",
                                                json_initial_inputs,
                                                json_config,
                                                complete_callback));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->getUUID().asString(), test->label());
    EXPECT_EQ("test_label", test->label());
    test->complete(true);
    std::string actual_config;
    test->jsonConfigStr(actual_config);
    std::string expected_config = TestTask::getExpectedConfigStr();
    EXPECT_EQ(actual_config, expected_config);
}

TEST_F(TestTask,
       call_back_ctor_sets_appropiate_callback_with_config_empty_stages_clone) {
    rapidjson::Document json_config;
    TestTask::getGenericConfig(json_config);
    rapidjson::Document json_initial_inputs;
    TestTask::getGenericInitialInputs(json_initial_inputs);
    std::function<void(bool)> complete_callback = [](bool status) {
        EXPECT_TRUE(status);
    };
    std::unique_ptr<Task> test(new TestTaskImpl("test_label",
                                                json_initial_inputs,
                                                json_config,
                                                complete_callback));
    std::unique_ptr<Task>&& test_clone = test->clone();
    EXPECT_NE(nullptr, test_clone);
    EXPECT_EQ(test_clone->getUUID().asString(), test->getUUID().asString());
    EXPECT_EQ("test_label", test_clone->label());
    test_clone->complete(true);
    std::string actual_config;
    test_clone->jsonConfigStr(actual_config);
    std::string expected_config = TestTask::getExpectedConfigStr();
    EXPECT_EQ(actual_config, expected_config);
}

TEST_F(TestTask,
       call_back_ctor_sets_appropiate_callback_plugin_with_config_empty_) {
    rapidjson::Document json_config;
    TestTask::getGenericConfig(json_config);
    rapidjson::Document json_initial_inputs;
    TestTask::getGenericInitialInputs(json_initial_inputs);
    std::unique_ptr<TaskCallbackPlugin> complete_callback_plugin =
            std::make_unique<TestTaskCallbackPlugin>();
    std::unique_ptr<Task> test(
            new TestTaskImpl("test_label",
                             json_initial_inputs,
                             json_config,
                             std::move(complete_callback_plugin)));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->getUUID().asString(), test->label());
    EXPECT_EQ("test_label", test->label());
    EXPECT_TRUE(test->callbackPluginIsSet());
    test->complete(true);
    std::string actual_config;
    test->jsonConfigStr(actual_config);
    std::string expected_config = TestTask::getExpectedConfigStr();
    EXPECT_EQ(actual_config, expected_config);
}

TEST_F(TestTask,
       call_back_ctor_sets_appropiate_callback_plugin_with_config_clone) {
    rapidjson::Document json_config;
    TestTask::getGenericConfig(json_config);
    rapidjson::Document json_initial_inputs;
    TestTask::getGenericInitialInputs(json_initial_inputs);
    std::unique_ptr<TaskCallbackPlugin> complete_callback_plugin =
            std::make_unique<TestTaskCallbackPlugin>();
    std::unique_ptr<Task> test(
            new TestTaskImpl("test_label",
                             json_initial_inputs,
                             json_config,
                             std::move(complete_callback_plugin)));
    std::unique_ptr<Task>&& test_clone = test->clone();
    EXPECT_NE(nullptr, test_clone);
    EXPECT_EQ(test_clone->getUUID().asString(), test->getUUID().asString());
    EXPECT_EQ("test_label", test_clone->label());
    EXPECT_TRUE(test_clone->callbackPluginIsSet());
    test_clone->complete(true);
    std::string actual_config;
    test_clone->jsonConfigStr(actual_config);
    std::string expected_config = TestTask::getExpectedConfigStr();
    EXPECT_EQ(actual_config, expected_config);
}

TEST_F(TestTask, iterateStagesSucceedsIfAllStagesRanNoKillTask) {
    const auto expected = {"A", "B", "C"};
    TestTaskImpl test_task;
    auto expected_it = expected.begin();
    ASSERT_EQ(test_task.stages_.size(), expected.size());
    bool ran_all = test_task.iterateStages([&](TaskStage& next) {
        EXPECT_EQ(*(expected_it), next.label());
        bool has_ran = next.run();
        ++expected_it;
        return has_ran;
    });
    EXPECT_TRUE(ran_all);
}

TEST_F(TestTask, iterateStagesFailsIfOneStageFailed) {
    TestTaskImpl test_task;
    bool ran_all = test_task.iterateStages([&](TaskStage& next) {
        bool has_ran = next.run();
        return (next.label() == "B" ? false : has_ran);
    });
    EXPECT_FALSE(ran_all);
}

TEST_F(TestTask, iterateStagesFailsIfTaskKilled) {
    TestTaskImpl test_task;
    bool ran_all = test_task.iterateStages([&](TaskStage& next) {
        next.end();
        bool keep_running = (next.label() == "C") ? (!test_task.kill()) : true;
        return keep_running;
    });
    EXPECT_FALSE(ran_all);
}

TEST_F(TestTask, userCannotMoveTaskWhileIteratingStages) {
    // Test move copy ctor.
    ASSERT_DEATH(
            {
                TestTaskImpl test_task;
                test_task.iterateStages([&](TaskStage& next) {
                    next.end();
                    TestTaskImpl destroy(std::move(test_task));
                    return true;
                });
            },
            "");

    // Test move assignment operator.
    ASSERT_DEATH(
            {
                TestTaskImpl test_task;
                test_task.iterateStages([&](TaskStage& next) {
                    next.end();
                    TestTaskImpl destroy;
                    destroy = std::move(test_task);
                    return true;
                });
            },
            "");
}
}  // namespace com::dag_scheduler
