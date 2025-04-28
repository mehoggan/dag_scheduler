////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include <dag_scheduler/Logging.h>
#include <gtest/gtest.h>

#include "utils/TestEnvironment.h"

int main(int argc, char* argv[]) {
    const std::string exe_path = argv[0];
    com::dag_scheduler::testing::TestEnvironment::exe_path = exe_path;

    com::dag_scheduler::LogTag main_tag("main");
    com::dag_scheduler::Logging::addStdCoutLogger(main_tag);

    ::testing::AddGlobalTestEnvironment(
            new com::dag_scheduler::testing::TestEnvironment);
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
