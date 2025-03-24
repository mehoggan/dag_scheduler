////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include <dag_scheduler/DynamicLibraryRegistry.h>
#include <gtest/gtest.h>

#include <stdexcept>

#include "utils/TestEnvironment.h"

namespace com::dag_scheduler {
TEST(TestDynamicLibraryRegistry, ctor) {
    EXPECT_NO_THROW(DynamicLibraryRegistry());
}

TEST(TestDynamicLibraryRegistry, register_dynamic_library_dne) {
    ASSERT_THROW(
            DynamicLibraryRegistry::register_dynamic_library("pink_elephant"),
            std::runtime_error);
}

TEST(TestDynamicLibraryRegistry, register_dynamic_library_exists) {
    std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
    ASSERT_NO_THROW(DynamicLibraryRegistry::register_dynamic_library(lib_path));
}
}  // namespace com::dag_scheduler
