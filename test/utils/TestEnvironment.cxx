////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "TestEnvironment.h"

#include <exception>
#include <string>

#include "dag_scheduler/Logging.h"

namespace com::dag_scheduler::testing {

class NotImplementedException : public std::exception {
public:
    explicit NotImplementedException(const std::string& function_name)
            : message_(function_name + " not implemented!!!") {}

    const char* what() const noexcept override { return (message_).c_str(); }

private:
    std::string message_;
};

Pathing::Pathing() : Pathing("") {}

Pathing::Pathing(const std::string& executable_path)
        : executable_path_(executable_path) {}

std::filesystem::path Pathing::get_lib_dir_path() const {
    return get_lib_path().parent_path();
}

std::filesystem::path Pathing::get_lib_path() const {
#ifdef __linux__
    return executable_path().parent_path().parent_path().parent_path() / "lib" /
           ".libs" / "libdag_scheduler.so";
#elif __APPLE__ && __MACH__
    return executable_path().parent_path().parent_path().parent_path() / "lib" /
           ".libs" / "libdag_scheduler.dylib";
#else
  throw NotImplementedException(std::string(__FUNCTION__);
#endif
}

std::filesystem::path Pathing::get_stages_lib_dir_path() const {
    return get_stages_lib_path().parent_path();
}

std::filesystem::path Pathing::get_stages_lib_path() const {
#ifdef __linux__
    return executable_path().parent_path().parent_path().parent_path() /
           "stages_lib" / "lib" / ".libs" / "libstages_lib.so";
#elif __APPLE__ && __MACH__
    return executable_path().parent_path().parent_path().parent_path() /
           "stages_lib" / "lib" / ".libs" / "libstages_lib.dylib";
#else
    throw NotImplementedException(__FUNCTION__);
#endif
}

const std::filesystem::path& Pathing::executable_path() const {
    return executable_path_;
}

std::string TestEnvironment::EXE_PATH;
com::dag_scheduler::LogTag TestEnvironment::TEST_TAG("Testing");
Pathing TestEnvironment::PATHING;

void TestEnvironment::SetUp() {
    com::dag_scheduler::Logging::add_std_cout_logger(TEST_TAG);
    if (TestEnvironment::EXE_PATH.empty()) {
        throw std::runtime_error(
                std::string("Please make sure ") +
                std::string("com::dag_scheduler::testing::TestEnvironment::") +
                std::string("EXE_PATH is set from within main!"));
    } else {
        Logging::info(TEST_TAG, "Got", EXE_PATH, "for path to executable");
        PATHING = Pathing(EXE_PATH);
    }
}
}  // namespace com::dag_scheduler::testing
