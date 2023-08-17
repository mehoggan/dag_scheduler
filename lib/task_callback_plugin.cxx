#include "dag_scheduler/task_callback_plugin.h"

namespace com
{
  namespace dag_scheduler
  {
    TaskCallbackPlugin::~TaskCallbackPlugin()
    {}

    void TaskCallbackPlugin::completed(bool, Task &)
    {
      LogTag CALLBACK_TAG_FOR_THIS = Logging::LogTag_for_this(*this);
      Logging::add_std_log_logger(Logging::LogTag_for_this(*this));
      Logging::info(CALLBACK_TAG_FOR_THIS, "Base callback plugin was called.");
    }

    TaskCallbackPlugin default_task_callback_plugin;
    BOOST_DLL_ALIAS_SECTIONED(default_task_callback_plugin,
      task_callback_plugin,
      TaskCb)
  }
}
