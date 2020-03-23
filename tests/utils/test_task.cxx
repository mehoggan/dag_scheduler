#include "test_task.h"
#include "test_task_stage.h"

namespace com
{
  namespace dag_scheduler
  {
    TestTaskImpl::TestTaskImpl()
    {
      label_ = uuid_.as_string();
      task::stages_.push_back(std::unique_ptr<task_stage>(
        new TestTaskStageImpl("A")));
      task::stages_.push_back(std::unique_ptr<task_stage>(
        new TestTaskStageImpl("B")));
      task::stages_.push_back(std::unique_ptr<task_stage>(
        new TestTaskStageImpl("C")));
    }

    TestTaskImpl::TestTaskImpl(const std::string &label)
    {
      label_ = label;
      task::stages_.push_back(std::unique_ptr<task_stage>(
        new TestTaskStageImpl("A")));
      task::stages_.push_back(std::unique_ptr<task_stage>(
        new TestTaskStageImpl("B")));
      task::stages_.push_back(std::unique_ptr<task_stage>(
        new TestTaskStageImpl("C")));
    }


    TestTaskImpl::TestTaskImpl(const std::string &label,
      std::function<void (bool)> complete_callback)
    {
      label_ = label;
      task::stages_.push_back(std::unique_ptr<task_stage>(
        new TestTaskStageImpl("A")));
      task::stages_.push_back(std::unique_ptr<task_stage>(
        new TestTaskStageImpl("B")));
      task::stages_.push_back(std::unique_ptr<task_stage>(
        new TestTaskStageImpl("C")));
      complete_callback_ = complete_callback;
    }

    TestTaskImpl::~TestTaskImpl()
    {}

    TestTaskImpl::TestTaskImpl(TestTaskImpl &&other) :
      task(std::move(other))
    {}
    
    TestTaskImpl &TestTaskImpl::operator=(TestTaskImpl &&other)
    {
      task::operator=(std::move(other));

      return (*this);
    }
  }
}
