#include "test_task.h"
#include "test_task_stage.h"

namespace com {
  namespace dag_scheduler {
    TestTaskImpl::TestTaskImpl() {
      Logging::info(LOG_TAG, "This was called. 1A");
      label_ = uuid_.as_string();
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("A"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("B"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("C"))
      );
    }

    TestTaskImpl::TestTaskImpl(const std::string &label) {
      Logging::info(LOG_TAG, "This was called. 1B");
      label_ = label;
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("A"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("B"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("C"))
      );
    }

    TestTaskImpl::TestTaskImpl(
      const std::string &label, const rapidjson::Document &json_initial_inputs
    ) {
      Logging::info(LOG_TAG, "This was called. 1C");
      label_ = label;
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("A"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("B"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("C"))
      );
      Task::set_json_initial_inputs(json_initial_inputs);
    }

    TestTaskImpl::TestTaskImpl(
      const std::string &label, std::function<void(bool)> complete_callback
    ) {
      Logging::info(LOG_TAG, "This was called. 1D");
      label_ = label;
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("A"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("B"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("C"))
      );
      complete_callback_ = complete_callback;
    }

    TestTaskImpl::TestTaskImpl(
      const std::string &label,
      const rapidjson::Document &json_initial_inputs,
      std::function<void(bool)> complete_callback
    ) {
      Logging::info(LOG_TAG, "This was called. 1E");
      label_ = label;
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("A"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("B"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("C"))
      );
      Task::set_json_initial_inputs(json_initial_inputs);
      complete_callback_ = complete_callback;
    }

    TestTaskImpl::TestTaskImpl(
      const std::string &label,
      std::unique_ptr<TaskCallbackPlugin> &&complete_callback_plugin
    ) {
      Logging::info(LOG_TAG, "This was called. 1F");
      label_ = label;
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("A"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("B"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("C"))
      );
      complete_callback_plugin_ = std::move(complete_callback_plugin);
    }

    TestTaskImpl::TestTaskImpl(
      const std::string &label,
      const rapidjson::Document &json_initial_inputs,
      std::unique_ptr<TaskCallbackPlugin> &&complete_callback_plugin
    ) {
      Logging::info(LOG_TAG, "This was called. 1G");
      label_ = label;
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("A"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("B"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("C"))
      );
      Task::set_json_initial_inputs(json_initial_inputs);
      complete_callback_plugin_ = std::move(complete_callback_plugin);
    }

    TestTaskImpl::TestTaskImpl(
      const std::string &label, const rapidjson::Document &json_config,
      const rapidjson::Document &json_initial_inputs
    ) {
      Logging::info(LOG_TAG, "This was called. 1H");
      label_ = label;
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("A"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("B"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("C"))
      );
      Task::set_json_config(json_config);
      Task::set_json_initial_inputs(json_initial_inputs);
    }

    TestTaskImpl::TestTaskImpl(
      const std::string &label, const rapidjson::Document &json_config,
      const rapidjson::Document &json_initial_inputs,
      std::function<void(bool)> complete_callback
    ) {
      Logging::info(LOG_TAG, "This was called. 1I");
      label_ = label;
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("A"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("B"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("C"))
      );
      Task::set_json_config(json_config);
      Task::set_json_initial_inputs(json_initial_inputs);
      complete_callback_ = complete_callback;
    }

    TestTaskImpl::TestTaskImpl(
      const std::string &label, const rapidjson::Document &json_config,
      const rapidjson::Document &json_initial_inputs,
      std::unique_ptr<TaskCallbackPlugin> &&complete_callback_plugin
    ) {
      Logging::info(LOG_TAG, "This was called. 1J");
      label_ = label;
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("A"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("B"))
      );
      Task::stages_.push_back(
        std::unique_ptr<TaskStage>(new TestTaskStageImpl("C"))
      );
      Task::set_json_config(json_config);
      Task::set_json_initial_inputs(json_initial_inputs);
      complete_callback_plugin_ = std::move(complete_callback_plugin);
    }

    TestTaskImpl::~TestTaskImpl() {}

    TestTaskImpl::TestTaskImpl(TestTaskImpl &&other) :
      Task(std::move(other)) {}

    TestTaskImpl &TestTaskImpl::operator=(TestTaskImpl &&other) {
      Task::operator=(std::move(other));

      return (*this);
    }
  } // namespace dag_scheduler
} // namespace com
