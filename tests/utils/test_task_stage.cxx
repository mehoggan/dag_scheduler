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
      task_stage(label),
      running_(false),
      nasty_user_defined_pointer_(new int(10)),
      run_sleep_time_(run_sleep_time)
    {}

    TestTaskStageImpl::~TestTaskStageImpl()
    {
      cleanup();
    }

    TestTaskStageImpl::TestTaskStageImpl(TestTaskStageImpl &&other) :
      task_stage(std::move(other)),
      running_(false),
      nasty_user_defined_pointer_(
        std::move(other.nasty_user_defined_pointer_)),
      run_sleep_time_(std::move(other.run_sleep_time_))
    {
      assert(not other.is_running() &&
        "You cannot move a running task_stage");

      other.nasty_user_defined_pointer_ = nullptr;
    }

    TestTaskStageImpl &TestTaskStageImpl::operator=(TestTaskStageImpl &&other)
    {
      task_stage::operator=(std::move(other));
      assert(not other.is_running() &&
        "You cannot move a running task_stage");
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
  }
}
