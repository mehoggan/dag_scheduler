#include "dag_scheduler/task_callback_plugin.h"

#include <boost/dll/alias.hpp>

namespace com {
namespace dag_scheduler {
TaskCallbackPlugin::~TaskCallbackPlugin() {}

void TaskCallbackPlugin::completed(bool, Task &) {
  LogTag CALLBACK_TAG_FOR_THIS = Logging::LogTag_for_this(*this);
  Logging::add_std_log_logger(Logging::LogTag_for_this(*this));
  Logging::info(CALLBACK_TAG_FOR_THIS, "Base callback plugin was called.");
}

std::unique_ptr<TaskCallbackPlugin> TaskCallbackPlugin::clone() const {
  auto cloned_callback = std::make_unique<TaskCallbackPlugin>();
  return cloned_callback;
}

TaskCallbackPlugin default_task_callback_plugin;
BOOST_DLL_ALIAS_SECTIONED(default_task_callback_plugin, task_callback_plugin,
                          TaskCb)
}  // namespace dag_scheduler
}  // namespace com
