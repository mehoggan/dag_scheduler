#ifndef TEST_TASK_H_INCLUDED
#define TEST_TASK_H_INCLUDED

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task.h"
#include "dag_scheduler/task_callback_plugin.h"

#include <gtest/gtest_prod.h>

#include <rapidjson/document.h>

#include <atomic>
#include <functional>

namespace com {
  namespace dag_scheduler {
    class TestTaskImpl : public Task {
    private:
      FRIEND_TEST(
        TestTask, iterate_stages_succeeds_if_all_stages_ran_no_kill_task
      );

    public:
      TestTaskImpl();

      TestTaskImpl(const std::string &label);

      TestTaskImpl(
        const std::string &label,
        const rapidjson::Document &json_initial_inputs
      );

      TestTaskImpl(
        const std::string &label, std::function<void(bool)> complete_callback
      );

      TestTaskImpl(
        const std::string &label,
        const rapidjson::Document &json_initial_inputs,
        std::function<void(bool)> complete_callback
      );

      TestTaskImpl(
        const std::string &label,
        std::unique_ptr<TaskCallbackPlugin> &&complete_callback_plugin
      );

      TestTaskImpl(
        const std::string &label,
        const rapidjson::Document &json_initial_inputs,
        std::unique_ptr<TaskCallbackPlugin> &&complete_callback_plugin
      );

      TestTaskImpl(
        const std::string &label, const rapidjson::Document &json_config,
        const rapidjson::Document &json_initial_inputs
      );

      TestTaskImpl(
        const std::string &label, const rapidjson::Document &json_config,
        const rapidjson::Document &json_initial_inputs,
        std::function<void(bool)> complete_callback
      );

      TestTaskImpl(
        const std::string &label, const rapidjson::Document &json_config,
        const rapidjson::Document &json_initial_inputs,
        std::unique_ptr<TaskCallbackPlugin> &&complete_callback_plugin
      );

      virtual ~TestTaskImpl();

      TestTaskImpl(TestTaskImpl &&other);

      TestTaskImpl &operator=(TestTaskImpl &&other);
    };
  } // namespace dag_scheduler
} // namespace com
#endif
