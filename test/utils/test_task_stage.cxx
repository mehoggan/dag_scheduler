#include "test_task_stage.h"

#include <assert.h>

namespace com
{
  namespace dag_scheduler
  {
    TestTaskStageImpl::TestTaskStageImpl() :
      TestTaskStageImpl("")
    {
      label_ = get_uuid().as_string();
    }

    TestTaskStageImpl::TestTaskStageImpl(const std::string &label,
      const std::chrono::nanoseconds &run_sleep_time) :
      TaskStage(label),
      running_(false),
      nasty_user_defined_pointer_(new int(10)),
      run_sleep_time_(run_sleep_time)
    {}

    TestTaskStageImpl::TestTaskStageImpl(const std::string &label,
      const UUID &uuid,
      const std::chrono::nanoseconds &run_sleep_time) :
      TaskStage(label, uuid),
      nasty_user_defined_pointer_(new int(10)),
      run_sleep_time_(run_sleep_time)
    {}

    TestTaskStageImpl::~TestTaskStageImpl()
    {
      cleanup();
    }

    TestTaskStageImpl::TestTaskStageImpl(TestTaskStageImpl &&other) :
      TaskStage(std::move(other)),
      running_(false),
      nasty_user_defined_pointer_(
        std::move(other.nasty_user_defined_pointer_)),
      run_sleep_time_(std::move(other.run_sleep_time_))
    {
      assert(not other.is_running() &&
        "You cannot move a running TaskStage");

      other.nasty_user_defined_pointer_ = nullptr;
    }

    TestTaskStageImpl &TestTaskStageImpl::operator=(TestTaskStageImpl &&other)
    {
      TaskStage::operator=(std::move(other));
      assert(not other.is_running() &&
        "You cannot move a running TaskStage");
      running_.store(false);
      nasty_user_defined_pointer_ = std::move(
        other.nasty_user_defined_pointer_);
      run_sleep_time_ = std::move(other.run_sleep_time_);

      other.nasty_user_defined_pointer_ = nullptr;

      return (*this);
    }

    bool TestTaskStageImpl::run()
    {
      running_.store(true);
      /*
      * Do nothing.
      */
      std::this_thread::sleep_for(run_sleep_time_);
      running_.store(false);
      return true;
    }

    bool TestTaskStageImpl::is_running() const
    {
      return running_.load();
    }

    bool TestTaskStageImpl::end()
    {
      return true;
    }

    void TestTaskStageImpl::cleanup()
    {
      if (nasty_user_defined_pointer_ != nullptr) {
        delete nasty_user_defined_pointer_;
      }
      nasty_user_defined_pointer_ = nullptr;
    }

    std::unique_ptr<TaskStage> TestTaskStageImpl::clone() const
    {
      std::unique_ptr<TaskStage> test_task_stage_impl_ptr =
        std::make_unique<TestTaskStageImpl>(label_, uuid_);
      return test_task_stage_impl_ptr;
    }
  }
}
