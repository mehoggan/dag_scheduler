#include "test_task.h"

namespace com
{
  namespace dag_scheduler
  {
    TestTaskImpl::TestTaskImpl() :
      logged_class<TestTaskImpl>(*this),
      task(),
      running_(false),
      nasty_user_defined_pointer_(new int(10))
    {}

    TestTaskImpl::TestTaskImpl(const std::string &label) :
      logged_class<TestTaskImpl>(*this),
      task(label),
      running_(false),
      nasty_user_defined_pointer_(new int(10))
    {}

    TestTaskImpl::~TestTaskImpl()
    {
      cleanup();
    }

    bool TestTaskImpl::run()
    {
      running_.store(true);
      return true;
    }

    bool TestTaskImpl::is_running() const
    {
      return running_.load();
    }

    bool TestTaskImpl::kill()
    {
      bool ret = false;
      if (running_.load()) {
        running_.store(false);
        ret = true;
      }
      return ret;
    }

    void TestTaskImpl::cleanup()
    {
      delete nasty_user_defined_pointer_;
    }

    std::unique_ptr<task> TestTaskImpl::clone()
    {
      return std::unique_ptr<task>(new TestTaskImpl(*this));
    }

    TestTaskImpl::TestTaskImpl(const TestTaskImpl &other) :
      logged_class<TestTaskImpl>(*this),
      task()
    {
      uuid_ = const_cast<uuid &>(other.get_uuid()).clone();
      label_ = other.label();
      nasty_user_defined_pointer_ =
        new int(*(other.nasty_user_defined_pointer_));
    }
  }
}
