#ifndef TEST_TASK_STAGE_H_INCLUDED
#define TEST_TASK_STAGE_H_INCLUDED

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task_stage.h"

#include <atomic>
#include <chrono>

namespace com
{
  namespace dag_scheduler
  {
    class TestTaskStageImpl : public TaskStage
    {
    public:
      TestTaskStageImpl();

      explicit TestTaskStageImpl(const std::string &label,
        const std::chrono::nanoseconds &run_sleep_time =
          std::chrono::nanoseconds(100000000));

      TestTaskStageImpl(const std::string &label,
        const UUID &uuid,
        const std::chrono::nanoseconds &run_sleep_time =
          std::chrono::nanoseconds(100000000));

      virtual ~TestTaskStageImpl();

      TestTaskStageImpl(const TestTaskStageImpl &) = delete;

      TestTaskStageImpl &operator=(const TestTaskStageImpl &) = delete;

      TestTaskStageImpl(TestTaskStageImpl &&other);

      TestTaskStageImpl &operator=(TestTaskStageImpl &&other);

      virtual bool run() override;

      virtual bool is_running() const override;

      virtual bool end() override;

      virtual void cleanup() override;

      virtual std::unique_ptr<TaskStage> clone() const override;

    private:
      std::atomic_bool running_;
      int *nasty_user_defined_pointer_;
      std::chrono::nanoseconds run_sleep_time_;
    };
  }
}
#endif
