#include "dag_scheduler/task_callback_plugin.h"

namespace com
{
  namespace dag_scheduler
  {
    TaskCallbackPlugin::~TaskCallbackPlugin()
    {}

    void TaskCallbackPlugin::completed(bool, Task &)
    {}
  }
}
