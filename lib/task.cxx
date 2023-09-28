#include "dag_scheduler/task.h"

#include "dag_scheduler/logging.h"
#include "dag_scheduler/task_callback_plugin.h"

#include <memory>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <boost/dll/alias.hpp>

#include <iostream>

void default_task_callback(bool) noexcept
{
  std::cout << "Default task callback called." << std::endl;
}

BOOST_DLL_ALIAS_SECTIONED(default_task_callback, task_callback, TaskCb)

namespace com
{
  namespace dag_scheduler
  {
    Task::Task() :
      LoggedClass<Task>(*this),
      iterating_(false),
      kill_(false)
    {
      label_ = uuid_.as_string();
      rapidjson::Document json_config;
      set_json_config(json_config);
    }

    Task::Task(std::vector<std::unique_ptr<TaskStage>> &stages) :
      Task(stages, "")
    {
      label_ = uuid_.as_string();
      rapidjson::Document json_config;
      set_json_config(json_config);
    }

    Task::Task(std::vector<std::unique_ptr<TaskStage>> &stages,
      const std::string &label) :
      LoggedClass<Task>(*this),
      iterating_(false),
      kill_(false),
      stages_(std::move(stages)),
      label_(label)
    {
      rapidjson::Document json_config;
      set_json_config(json_config);
    }

    Task::Task(std::vector<std::unique_ptr<TaskStage>> &stages,
      const std::string &label,
      std::function<void (bool)> complete_callback) :
      LoggedClass<Task>(*this),
      iterating_(false),
      kill_(false),
      stages_(std::move(stages)),
      label_(label),
      complete_callback_(complete_callback)
    {
      rapidjson::Document json_config;
      set_json_config(json_config);
    }

    Task::Task(std::vector<std::unique_ptr<TaskStage>> &stages,
      const std::string &label,
      std::unique_ptr<TaskCallbackPlugin> &&complete_callback_plugin) :
      LoggedClass<Task>(*this),
      iterating_(false),
      kill_(false),
      stages_(std::move(stages)),
      label_(label),
      complete_callback_plugin_(std::move(complete_callback_plugin))
    {
      rapidjson::Document json_config;
      set_json_config(json_config);
    }

    Task::Task(std::vector<std::unique_ptr<TaskStage>> &stages,
      const rapidjson::Document &json_config) :
      LoggedClass<Task>(*this),
      iterating_(false),
      kill_(false),
      stages_(std::move(stages))
    {
      set_json_config(json_config);
    }

    Task::Task(std::vector<std::unique_ptr<TaskStage>> &stages,
      const std::string &label,
      const rapidjson::Document &json_config) :
      LoggedClass<Task>(*this),
      iterating_(false),
      kill_(false),
      stages_(std::move(stages)),
      label_(label)
    {
      set_json_config(json_config);
    }

    Task::Task(std::vector<std::unique_ptr<TaskStage>> &stages,
      const std::string &label,
      std::function<void (bool)> complete_callback,
      const rapidjson::Document &json_config) :
      LoggedClass<Task>(*this),
      iterating_(false),
      kill_(false),
      stages_(std::move(stages)),
      label_(label),
      complete_callback_(complete_callback)
    {
      set_json_config(json_config);
    }

    Task::Task(std::vector<std::unique_ptr<TaskStage>> &stages,
      const std::string &label,
      std::unique_ptr<TaskCallbackPlugin> &&complete_callback_plugin,
      const rapidjson::Document &json_config) :
      LoggedClass<Task>(*this),
      iterating_(false),
      kill_(false),
      stages_(std::move(stages)),
      label_(label),
      complete_callback_plugin_(std::move(complete_callback_plugin))
    {
      set_json_config(json_config);
    }

    Task::~Task()
    {}

    Task::Task(Task &&other) :
      LoggedClass<Task>(*this),
      iterating_(false),
      kill_(false),
      stages_(std::move(other.stages_)),
      label_(std::move(other.label_)),
      uuid_(other.uuid_.clone()),
      json_config_(std::move(other.json_config_))
    {
      assert(not other.iterating_.load() && "You cannot move an itterating"
        "task");
    }

    Task &Task::operator=(Task &&other)
    {
      assert(not other.iterating_.load() && "You cannot move an itterating"
        "task");

      iterating_.store(false);
      kill_.store(false);
      stages_ = std::move(other.stages_);
      label_ = std::move(other.label_);
      uuid_ = other.uuid_.clone();
      json_config_ = std::move(other.json_config_);

      return (*this);
    }

    const std::string &Task::label() const
    {
      return label_;
    }

    const UUID &Task::get_uuid() const
    {
      return uuid_;
    }

    bool Task::iterate_stages(
      const std::function<bool(TaskStage &)> &next_stage)
    {
      bool ran_all = false;

      if (not iterating_.load()) {
        iterating_.store(true);
        {
          ran_all = std::all_of(stages_.begin(), stages_.end(),
            [&](class std::unique_ptr<TaskStage> &next) {
              bool ran = next_stage(*next);
              next->cleanup();
              bool ret = (ran && next->end() && (not kill_.load()));
              return ret;
            });
        }
        iterating_.store(false);
      }

      return ran_all;
    }

    bool Task::kill()
    {
      kill_.store(true);
      return kill_.load();
    }

    void Task::complete(bool status)
    {
      if (callback_is_set()) {
        Logging::info(LOG_TAG, "Going to call user specified function...");
        complete_callback_(status);
      }

      if (callback_plugin_is_set()) {
        Logging::info(LOG_TAG, "Going to call user specified callback",
          "plugin...");
        complete_callback_plugin_->completed(status, (*this));
      }
    }

    bool Task::callback_is_set() const
    {
      bool ret = false;
      if (complete_callback_) {
        ret = true;
      }
      return ret;
    }

    bool Task::callback_plugin_is_set() const
    {
      return complete_callback_plugin_ != nullptr;
    }

    const rapidjson::Document &Task::json_config() const
    {
      return (*json_config_);
    }
    
    void Task::json_config_str(std::string &out_str) const
    {
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

    std::unique_ptr<Task> Task::clone() const
    {
      std::vector<std::unique_ptr<TaskStage>> cloned_stages;
      std::for_each(stages_.begin(), stages_.end(),
        [&cloned_stages](const std::unique_ptr<TaskStage> &next_stage)
        {
          std::unique_ptr<TaskStage> &&task_stage_ptr = next_stage->clone();
          cloned_stages.push_back(std::move(task_stage_ptr));
        });

      rapidjson::Document json_config;
      if (json_config_) {
        rapidjson::Document &this_document = *(json_config_.get());
        json_config.CopyFrom(this_document, json_config.GetAllocator());
      }

      std::unique_ptr<Task> task_ptr;
      if (complete_callback_plugin_) {
        std::unique_ptr<TaskCallbackPlugin> &&callback_plugin =
          complete_callback_plugin_->clone();
        task_ptr = std::make_unique<Task>(cloned_stages, label_,
          std::move(callback_plugin), json_config);
      } else {
        task_ptr = std::make_unique<Task>(cloned_stages, label_,
          complete_callback_, json_config);
      }

      task_ptr->update_uuid(uuid_);

      return task_ptr;
    }

    bool operator==(const Task &lhs, const Task &rhs)
    {
      return lhs.uuid_ == rhs.uuid_;
    }

    bool operator!=(const Task &lhs, const Task &rhs)
    {
      return !(lhs == rhs);
    }

    std::ostream &operator<<(std::ostream &out, const Task &t)
    {
      out << "label = " << t.label_;
      if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
      }
      std::for_each(t.stages_.begin(), t.stages_.end(),
        [&](const std::unique_ptr<TaskStage> &s) {
          out << (*s) << " ";
        });
      std::string json_config_string;
      t.json_config_str(json_config_string);
      out << "configuration = " << json_config_string;

      return out;
    }

    std::stringstream &operator<<(std::stringstream &out, const Task &t)
    {
      out << "label = " << t.label_;
      if (t.label_ != t.uuid_.as_string()) {
        out << " uuid = " << t.uuid_;
      }
      std::for_each(t.stages_.begin(), t.stages_.end(),
        [&](const std::unique_ptr<TaskStage> &s) {
          out << (*s) << " ";
        });
      std::string json_config_string;
      t.json_config_str(json_config_string);
      out << "configuration:" << json_config_string;

      return out;
    }

    void Task::set_json_config(const rapidjson::Document &json_config)
    {
      if (json_config_) {
        json_config_.reset();
      }
      json_config_ = std::make_unique<rapidjson::Document>();
      json_config_->CopyFrom(json_config, json_config_->GetAllocator());
    }

    void Task::update_uuid(const UUID &uuid)
    {
      uuid_ = UUID(uuid.as_string());
    }
  }
}
