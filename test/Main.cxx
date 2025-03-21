#include <dag_scheduler/logging.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "utils/test_environment.h"

int main(int argc, char* argv[]) {
    const std::string exe_path = argv[0];
    com::dag_scheduler::testing::TestEnvironment::EXE_PATH = exe_path;

    com::dag_scheduler::LogTag main_tag("main");
    com::dag_scheduler::Logging::add_std_cout_logger(main_tag);

    ::testing::AddGlobalTestEnvironment(
            new com::dag_scheduler::testing::TestEnvironment);
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
