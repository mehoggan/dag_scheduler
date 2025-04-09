////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef TEST_UTILS_TESTENVIRONMENT_H
#define TEST_UTILS_TESTENVIRONMENT_H

#include <dag_scheduler/Logging.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <string>

namespace com::dag_scheduler::testing {
class Pathing {
public:
    Pathing();

    explicit Pathing(const std::string& executable_path);

    std::filesystem::path get_lib_dir_path() const;

    std::filesystem::path get_lib_path() const;

    std::filesystem::path get_stages_lib_dir_path() const;

    std::filesystem::path get_stages_lib_path() const;

    const std::filesystem::path& executable_path() const;

private:
    std::filesystem::path executable_path_;
};

class TestEnvironment : public ::testing::Environment {
public:
    static std::string EXE_PATH;
    static com::dag_scheduler::LogTag TEST_TAG;
    static Pathing PATHING;

    void SetUp() override;
};
}  // namespace com::dag_scheduler::testing
#endif
