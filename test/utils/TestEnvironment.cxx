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

std::filesystem::path Pathing::getLibDirPath() const {
    return getLibPath().parent_path();
}

std::filesystem::path Pathing::getLibPath() const {
#ifdef __linux__
    return executablePath().parent_path().parent_path().parent_path() / "lib" /
           ".libs" / "libdag_scheduler.so";
#elif __APPLE__ && __MACH__
    return executablePath().parent_path().parent_path().parent_path() / "lib" /
           ".libs" / "libdag_scheduler.dylib";
#else
  throw NotImplementedException(std::string(__FUNCTION__);
#endif
}

std::filesystem::path Pathing::getStagesLibDirPath() const {
    return getStagesLibPath().parent_path();
}

std::filesystem::path Pathing::getStagesLibPath() const {
#ifdef __linux__
    std::cout << "Linux!!!" << std::endl;
    return executablePath().parent_path().parent_path().parent_path() /
           "stages_lib" / "lib" / ".libs" / "libstages_lib.so";
#elif __APPLE__ && __MACH__
    std::cout << "Mac!!!" << std::endl;
    return executable_path().parent_path().parent_path().parent_path() /
           "stages_lib" / "lib" / ".libs" / "libstages_lib.dylib";
#else
    throw NotImplementedException(__FUNCTION__);
#endif
}

const std::filesystem::path& Pathing::executablePath() const {
    return executable_path_;
}

std::string TestEnvironment::exe_path;  // NOLINT
com::dag_scheduler::LogTag TestEnvironment::test_tag("Testing");
Pathing TestEnvironment::pathing;

void TestEnvironment::SetUp() {
    com::dag_scheduler::Logging::addStdCoutLogger(test_tag);
    if (TestEnvironment::exe_path.empty()) {
        throw std::runtime_error(
                std::string("Please make sure ") +
                std::string("com::dag_scheduler::testing::TestEnvironment::") +
                std::string("EXE_PATH is set from within main!"));
    } else {
        Logging::info(test_tag, "Got", exe_path, "for path to executable");
        pathing = Pathing(exe_path);
    }
}
}  // namespace com::dag_scheduler::testing
