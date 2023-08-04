#include "test_environment.h"
#include "dag_scheduler/logging.h"
#include <stdexcept>

namespace com
{
  namespace dag_scheduler
  {
    namespace testing
    {
      Pathing::Pathing() :
        Pathing("")
      {}

      Pathing::Pathing(const std::string &executable_path) :
        executable_path_(executable_path)
      {}

      std::filesystem::path Pathing::get_lib_dir_path() const
      {
        return get_lib_path().parent_path();
      }

      std::filesystem::path Pathing::get_lib_path() const
      {
        return executable_path_
          .parent_path()
          .parent_path()
          .parent_path() / "lib" / ".libs" / "libdag_scheduler.dylib";
      }

      const std::filesystem::path &Pathing::executable_path() const
      {
        return executable_path_;
      }


      std::string TestEnvironment::EXE_PATH;
      com::dag_scheduler::LogTag TestEnvironment::TEST_TAG("Testing");
      Pathing TestEnvironment::PATHING;

      void TestEnvironment::SetUp()
      {
        com::dag_scheduler::Logging::add_std_cout_logger(TEST_TAG);
        if (TestEnvironment::EXE_PATH.empty()) {
          throw std::runtime_error(std::string("Please make sure ") +
              std::string("com::dag_scheduler::testing::TestEnvironment::") +
              std::string("EXE_PATH is set from within main!"));
        } else {
          Logging::info(TEST_TAG, "Got", EXE_PATH, "for path to executable");
          PATHING = Pathing(EXE_PATH);
        }
      }
    }
  }
}
