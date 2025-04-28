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

    std::filesystem::path getLibDirPath() const;

    std::filesystem::path getLibPath() const;

    std::filesystem::path getStagesLibDirPath() const;

    std::filesystem::path getStagesLibPath() const;

    const std::filesystem::path& executablePath() const;

private:
    std::filesystem::path executable_path_;
};

class TestEnvironment : public ::testing::Environment {
public:
    static std::string exe_path;
    static com::dag_scheduler::LogTag test_tag;
    static Pathing pathing;

    void SetUp() override;
};
}  // namespace com::dag_scheduler::testing
#endif
