#include "dag_scheduler/task.h"

#include "dag_scheduler/logging.h"
#include "dag_scheduler/task_callback_plugin.h"

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
    }

    Task::Task(std::vector<std::unique_ptr<TaskStage>> &stages) :
      Task(stages, "")
    {
      label_ = uuid_.as_string();
    }

    Task::Task(std::vector<std::unique_ptr<TaskStage>> &stages,
      const std::string &label) :
      LoggedClass<Task>(*this),
      iterating_(false),
      kill_(false),
      stages_(std::move(stages)),
      label_(label)
    {}

    Task::Task(std::vector<std::unique_ptr<TaskStage>> &stages,
      const std::string &label,
      std::function<void (bool)> complete_callback) :
      LoggedClass<Task>(*this),
      iterating_(false),
      kill_(false),
      stages_(std::move(stages)),
      label_(label),
      complete_callback_(complete_callback)
    {}

    Task::Task(std::vector<std::unique_ptr<TaskStage>> &stages,
      const std::string &label,
      std::unique_ptr<TaskCallbackPlugin> &&complete_callback_plugin) :
      LoggedClass<Task>(*this),
      iterating_(false),
      kill_(false),
      stages_(std::move(stages)),
      label_(label),
      complete_callback_plugin_(std::move(complete_callback_plugin))
    {}

    Task::~Task()
    {}

    Task::Task(Task &&other) :
      LoggedClass<Task>(*this),
      iterating_(false),
      kill_(false),
      stages_(std::move(other.stages_)),
      label_(std::move(other.label_)),
      uuid_(other.uuid_.clone())
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

      return out;
    }
  }
}
