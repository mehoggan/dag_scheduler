#include "utils/test_task.h"

#include <gtest/gtest.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <atomic>

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"
#include "dag_scheduler/task_callback_plugin.h"

namespace com {
namespace dag_scheduler {
class TestTask : public ::testing::Test, public LoggedClass<TestTask> {
public:
    TestTask() : LoggedClass<TestTask>(*this) {}

    static void get_generic_config(rapidjson::Document& config_doc) {
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

    static std::string get_expected_config_str() {
        return std::string("{") + std::string("\"test_value_int\":-1,") +
               std::string("\"test_value_str\":\"test_string\",") +
               std::string("\"test_value_bool\":true,") +
               std::string("\"test_value_double\":-1.0") + std::string("}");
    }

    static void get_generic_initial_inputs(
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

    static std::string get_expected_generic_initial_inputs_str() {
        return std::string("{") + std::string("\"test_value_int\":-1,") +
               std::string("\"test_value_str\":\"test_string\",") +
               std::string("\"test_value_bool\":true,") +
               std::string("\"test_value_double\":-1.0") + std::string("}");
    }

protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

class TestTaskCallbackPlugin : public TaskCallbackPlugin,
                               public ::testing::Test,
                               public LoggedClass<TestTaskCallbackPlugin> {
public:
    TestTaskCallbackPlugin() : LoggedClass<TestTaskCallbackPlugin>(*this) {}

    virtual ~TestTaskCallbackPlugin() {}

    virtual void completed(bool completed, Task& task) override {
        Logging::info(LOG_TAG,
                      "completed called with",
                      (completed ? "true" : "false"),
                      "and",
                      task);
    }

    virtual std::unique_ptr<TaskCallbackPlugin> clone() const override {
        auto cloned_callback = std::make_unique<TestTaskCallbackPlugin>();
        return cloned_callback;
    }

    virtual void TestBody() override {}
};

TEST_F(TestTask, default_ctor) {
    std::unique_ptr<Task> test(new TestTaskImpl);
    EXPECT_NE(nullptr, test);
    EXPECT_EQ(test->get_uuid().as_string(), test->label());
    {
        std::string empty_config;
        test->json_config_str(empty_config);
        EXPECT_EQ("{}", empty_config);
    }

    {
        std::string empty_initial_json_inputs;
        test->json_initial_inputs_str(empty_initial_json_inputs);
        EXPECT_EQ("{}", empty_initial_json_inputs);
    }
}

TEST_F(TestTask, default_ctor_clone) {
    std::unique_ptr<Task> test(new TestTaskImpl);
    std::unique_ptr<Task>&& test_clone = test->clone();
    EXPECT_NE(nullptr, test_clone);
    EXPECT_EQ(test_clone->get_uuid().as_string(), test->get_uuid().as_string());
    {
        std::string empty_config;
        test->json_config_str(empty_config);
        EXPECT_EQ("{}", empty_config);
    }

    {
        std::string empty_initial_json_inputs;
        test->json_initial_inputs_str(empty_initial_json_inputs);
        EXPECT_EQ("{}", empty_initial_json_inputs);
    }
}

TEST_F(TestTask, label_ctor) {
    std::unique_ptr<Task> test(new TestTaskImpl("test_label"));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->get_uuid().as_string(), test->label());
    EXPECT_EQ("test_label", test->label());
    {
        std::string empty_config;
        test->json_config_str(empty_config);
        EXPECT_EQ("{}", empty_config);
    }

    {
        std::string empty_initial_json_inputs;
        test->json_initial_inputs_str(empty_initial_json_inputs);
        EXPECT_EQ("{}", empty_initial_json_inputs);
    }
}

TEST_F(TestTask, label_ctor_with_inputs) {
    rapidjson::Document initial_inputs;
    TestTask::get_generic_initial_inputs(initial_inputs);
    std::unique_ptr<Task> test(new TestTaskImpl("test_label", initial_inputs));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->get_uuid().as_string(), test->label());
    EXPECT_EQ("test_label", test->label());
    {
        std::string empty_config;
        test->json_config_str(empty_config);
        EXPECT_EQ("{}", empty_config);
    }

    {
        std::string json_inputs;
        test->json_initial_inputs_str(json_inputs);
        std::string expected_initial_json_inputs =
                TestTask::get_expected_generic_initial_inputs_str();
        EXPECT_EQ(expected_initial_json_inputs, json_inputs);
    }
}

TEST_F(TestTask, label_ctor_clone) {
    std::unique_ptr<Task> test(new TestTaskImpl("test_label"));
    std::unique_ptr<Task>&& test_clone = test->clone();
    EXPECT_NE(nullptr, test_clone);
    EXPECT_EQ(test_clone->get_uuid().as_string(), test->get_uuid().as_string());
    EXPECT_EQ("test_label", test_clone->label());
    std::string empty_config;
    test_clone->json_config_str(empty_config);
    EXPECT_EQ("{}", empty_config);
}

TEST_F(TestTask, call_back_ctor_sets_appropiate_callback) {
    std::function<void(bool)> complete_callback = [](bool status) {
        EXPECT_TRUE(status);
    };
    std::unique_ptr<Task> test(
            new TestTaskImpl("test_label", complete_callback));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->get_uuid().as_string(), test->label());
    EXPECT_EQ("test_label", test->label());
    test->complete(true);
    std::string empty_config;
    test->json_config_str(empty_config);
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
    EXPECT_EQ(test_clone->get_uuid().as_string(), test->get_uuid().as_string());
    EXPECT_EQ("test_label", test_clone->label());
    test_clone->complete(true);
    std::string empty_config;
    test_clone->json_config_str(empty_config);
    EXPECT_EQ("{}", empty_config);
}

TEST_F(TestTask, call_back_ctor_sets_appropiate_callback_plugin) {
    std::unique_ptr<TaskCallbackPlugin> complete_callback_plugin =
            std::make_unique<TestTaskCallbackPlugin>();
    std::unique_ptr<Task> test(new TestTaskImpl(
            "test_label", std::move(complete_callback_plugin)));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->get_uuid().as_string(), test->label());
    EXPECT_EQ("test_label", test->label());
    EXPECT_TRUE(test->callback_plugin_is_set());
    test->complete(true);
    std::string empty_config;
    test->json_config_str(empty_config);
    EXPECT_EQ("{}", empty_config);
}

TEST_F(TestTask, call_back_ctor_sets_appropiate_callback_plugin_clone) {
    std::unique_ptr<TaskCallbackPlugin> complete_callback_plugin =
            std::make_unique<TestTaskCallbackPlugin>();
    std::unique_ptr<Task> test(new TestTaskImpl(
            "test_label", std::move(complete_callback_plugin)));
    std::unique_ptr<Task>&& test_clone = test->clone();
    EXPECT_NE(nullptr, test_clone);
    EXPECT_EQ(test_clone->get_uuid().as_string(), test->get_uuid().as_string());
    EXPECT_EQ("test_label", test_clone->label());
    EXPECT_TRUE(test_clone->callback_plugin_is_set());
    test_clone->complete(true);
    std::string empty_config;
    test_clone->json_config_str(empty_config);
    EXPECT_EQ("{}", empty_config);
}

TEST_F(TestTask, label_ctor_with_initial_inputs) {
    rapidjson::Document json_initial_inputs;
    TestTask::get_generic_config(json_initial_inputs);
    std::unique_ptr<Task> test(
            new TestTaskImpl("test_label", json_initial_inputs));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->get_uuid().as_string(), test->label());
    EXPECT_EQ("test_label", test->label());
    std::string actual_initial_inputs;
    test->json_initial_inputs_str(actual_initial_inputs);
    std::string expected_initial_json_inputs =
            TestTask::get_expected_generic_initial_inputs_str();
    EXPECT_EQ(actual_initial_inputs, expected_initial_json_inputs);
}

TEST_F(TestTask, label_ctor_with_initial_inputs_clone) {
    rapidjson::Document json_initial_inputs;
    TestTask::get_generic_initial_inputs(json_initial_inputs);
    std::unique_ptr<Task> test(
            new TestTaskImpl("test_label", json_initial_inputs));
    std::unique_ptr<Task>&& test_clone = test->clone();
    EXPECT_NE(nullptr, test_clone);
    EXPECT_EQ(test_clone->get_uuid().as_string(), test->get_uuid().as_string());
    EXPECT_EQ("test_label", test_clone->label());
    std::string actual_initial_inputs;
    test_clone->json_initial_inputs_str(actual_initial_inputs);
    std::string expected_initial_json_inputs =
            TestTask::get_expected_generic_initial_inputs_str();
    EXPECT_EQ(actual_initial_inputs, expected_initial_json_inputs);
}

TEST_F(TestTask,
       call_back_ctor_sets_appropiate_callback_with_initial_inputs_empty_stages) {
    rapidjson::Document json_config;
    TestTask::get_generic_config(json_config);
    rapidjson::Document json_initial_inputs;
    TestTask::get_generic_initial_inputs(json_initial_inputs);
    std::function<void(bool)> complete_callback = [](bool status) {
        EXPECT_TRUE(status);
    };
    std::unique_ptr<Task> test(new TestTaskImpl(
            "test_label", json_initial_inputs, json_config, complete_callback));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->get_uuid().as_string(), test->label());
    EXPECT_EQ("test_label", test->label());
    test->complete(true);
    std::string actual_config;
    test->json_config_str(actual_config);
    std::string expected_config = TestTask::get_expected_config_str();
    EXPECT_EQ(actual_config, expected_config);
}

TEST_F(TestTask,
       call_back_ctor_sets_appropiate_callback_with_config_empty_stages_clone) {
    rapidjson::Document json_config;
    TestTask::get_generic_config(json_config);
    rapidjson::Document json_initial_inputs;
    TestTask::get_generic_initial_inputs(json_initial_inputs);
    std::function<void(bool)> complete_callback = [](bool status) {
        EXPECT_TRUE(status);
    };
    std::unique_ptr<Task> test(new TestTaskImpl(
            "test_label", json_initial_inputs, json_config, complete_callback));
    std::unique_ptr<Task>&& test_clone = test->clone();
    EXPECT_NE(nullptr, test_clone);
    EXPECT_EQ(test_clone->get_uuid().as_string(), test->get_uuid().as_string());
    EXPECT_EQ("test_label", test_clone->label());
    test_clone->complete(true);
    std::string actual_config;
    test_clone->json_config_str(actual_config);
    std::string expected_config = TestTask::get_expected_config_str();
    EXPECT_EQ(actual_config, expected_config);
}

TEST_F(TestTask,
       call_back_ctor_sets_appropiate_callback_plugin_with_config_empty_stages) {
    rapidjson::Document json_config;
    TestTask::get_generic_config(json_config);
    rapidjson::Document json_initial_inputs;
    TestTask::get_generic_initial_inputs(json_initial_inputs);
    std::unique_ptr<TaskCallbackPlugin> complete_callback_plugin =
            std::make_unique<TestTaskCallbackPlugin>();
    std::unique_ptr<Task> test(
            new TestTaskImpl("test_label",
                             json_initial_inputs,
                             json_config,
                             std::move(complete_callback_plugin)));
    EXPECT_NE(nullptr, test);
    EXPECT_NE(test->get_uuid().as_string(), test->label());
    EXPECT_EQ("test_label", test->label());
    EXPECT_TRUE(test->callback_plugin_is_set());
    test->complete(true);
    std::string actual_config;
    test->json_config_str(actual_config);
    std::string expected_config = TestTask::get_expected_config_str();
    EXPECT_EQ(actual_config, expected_config);
}

TEST_F(TestTask,
       call_back_ctor_sets_appropiate_callback_plugin_with_config_clone) {
    rapidjson::Document json_config;
    TestTask::get_generic_config(json_config);
    rapidjson::Document json_initial_inputs;
    TestTask::get_generic_initial_inputs(json_initial_inputs);
    std::unique_ptr<TaskCallbackPlugin> complete_callback_plugin =
            std::make_unique<TestTaskCallbackPlugin>();
    std::unique_ptr<Task> test(
            new TestTaskImpl("test_label",
                             json_initial_inputs,
                             json_config,
                             std::move(complete_callback_plugin)));
    std::unique_ptr<Task>&& test_clone = test->clone();
    EXPECT_NE(nullptr, test_clone);
    EXPECT_EQ(test_clone->get_uuid().as_string(), test->get_uuid().as_string());
    EXPECT_EQ("test_label", test_clone->label());
    EXPECT_TRUE(test_clone->callback_plugin_is_set());
    test_clone->complete(true);
    std::string actual_config;
    test_clone->json_config_str(actual_config);
    std::string expected_config = TestTask::get_expected_config_str();
    EXPECT_EQ(actual_config, expected_config);
}

TEST_F(TestTask, iterate_stages_succeeds_if_all_stages_ran_no_kill_task) {
    const auto expected = {"A", "B", "C"};
    TestTaskImpl tt;
    auto expected_it = expected.begin();
    ASSERT_EQ(tt.stages_.size(), expected.size());
    bool ran_all = tt.iterate_stages([&](TaskStage& next) {
        EXPECT_EQ(*(expected_it), next.label());
        bool ran = next.run();
        ++expected_it;
        return ran;
    });
    EXPECT_TRUE(ran_all);
}

TEST_F(TestTask, iterate_stages_fails_if_one_stage_failed) {
    TestTaskImpl tt;
    bool ran_all = tt.iterate_stages([&](TaskStage& next) {
        bool ran = next.run();
        return (next.label() == "B" ? false : ran);
    });
    EXPECT_FALSE(ran_all);
}

TEST_F(TestTask, iterate_stages_fails_if_task_killed) {
    TestTaskImpl tt;
    bool ran_all = tt.iterate_stages([&](TaskStage& next) {
        next.end();
        bool keep_running = (next.label() == "C") ? (not tt.kill()) : true;
        return keep_running;
    });
    EXPECT_FALSE(ran_all);
}

TEST_F(TestTask, user_cannot_move_task_while_iterating_stages) {
    // Test move copy ctor.
    ASSERT_DEATH(
            {
                TestTaskImpl tt;
                tt.iterate_stages([&](TaskStage& next) {
                    next.end();
                    TestTaskImpl destroy(std::move(tt));
                    return true;
                });
            },
            "");

    // Test move assignment operator.
    ASSERT_DEATH(
            {
                TestTaskImpl tt;
                tt.iterate_stages([&](TaskStage& next) {
                    next.end();
                    TestTaskImpl destroy;
                    destroy = std::move(tt);
                    return true;
                });
            },
            "");
}
}  // namespace dag_scheduler
}  // namespace com
