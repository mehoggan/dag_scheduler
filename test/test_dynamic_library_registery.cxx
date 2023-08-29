#include <gtest/gtest.h>

#include "utils/test_environment.h"

#include <dag_scheduler/dynamic_library_registry.h>

#include <stdexcept>

namespace com
{
  namespace dag_scheduler
  {
    TEST(TestDynamicLibraryRegistery, ctor)
    {
      EXPECT_NO_THROW(DynamicLibraryRegistry());
    }

    TEST(TestDynamicLibraryRegistery, register_dynamic_library_dne)
    {
      ASSERT_THROW(
        DynamicLibraryRegistry::register_dynamic_library("pink_elephant"),
        std::runtime_error);
    }

    TEST(TestDynamicLibraryRegistery, register_dynamic_library_exists)
    {
      std::string lib_path =
        testing::TestEnvironment::PATHING.get_lib_path();
      ASSERT_NO_THROW(
        DynamicLibraryRegistry::register_dynamic_library(lib_path));
    }
  }
}
