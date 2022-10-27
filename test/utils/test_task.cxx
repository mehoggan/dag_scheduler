#include "test_task.h"
#include "test_task_stage.h"

namespace com
{
  namespace dag_scheduler
  {
    TestTaskImpl::TestTaskImpl()
    {
      label_ = uuid_.as_string();
      Task::stages_.push_back(std::unique_ptr<TaskStage>(
        new TestTaskStageImpl("A")));
      Task::stages_.push_back(std::unique_ptr<TaskStage>(
        new TestTaskStageImpl("B")));
      Task::stages_.push_back(std::unique_ptr<TaskStage>(
        new TestTaskStageImpl("C")));
    }

    TestTaskImpl::TestTaskImpl(const std::string &label)
    {
      label_ = label;
      Task::stages_.push_back(std::unique_ptr<TaskStage>(
        new TestTaskStageImpl("A")));
      Task::stages_.push_back(std::unique_ptr<TaskStage>(
        new TestTaskStageImpl("B")));
      Task::stages_.push_back(std::unique_ptr<TaskStage>(
        new TestTaskStageImpl("C")));
    }


    TestTaskImpl::TestTaskImpl(const std::string &label,
      std::function<void (bool)> complete_callback)
    {
      label_ = label;
      Task::stages_.push_back(std::unique_ptr<TaskStage>(
        new TestTaskStageImpl("A")));
      Task::stages_.push_back(std::unique_ptr<TaskStage>(
        new TestTaskStageImpl("B")));
      Task::stages_.push_back(std::unique_ptr<TaskStage>(
        new TestTaskStageImpl("C")));
      complete_callback_ = complete_callback;
    }

    TestTaskImpl::~TestTaskImpl()
    {}

    TestTaskImpl::TestTaskImpl(TestTaskImpl &&other) :
      Task(std::move(other))
    {}
    
    TestTaskImpl &TestTaskImpl::operator=(TestTaskImpl &&other)
    {
      Task::operator=(std::move(other));

      return (*this);
    }
  }
}
