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

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <fstream>
#include <sstream>
#include <string>

namespace com::dag_scheduler {
class TestLogging : public ::testing::Test {
protected:
    void SetUp() override { Logging::init(); }

    void TearDown() override {}
};

TEST_F(TestLogging, addLoggersNoDuplicates) {
    LogTag tag1("tag1");

    Logging::clearAll();
    EXPECT_TRUE(Logging::addStdCoutLogger(tag1));
    EXPECT_FALSE(Logging::addStdCoutLogger(tag1));
    Logging::clearAll();
    EXPECT_TRUE(Logging::addStdCerrLogger(tag1));
    EXPECT_FALSE(Logging::addStdCerrLogger(tag1));
    Logging::clearAll();
    EXPECT_TRUE(Logging::addStdLogLogger(tag1));
    EXPECT_FALSE(Logging::addStdLogLogger(tag1));
    Logging::clearAll();
    EXPECT_TRUE(Logging::addFileLogger(tag1, ""));
    EXPECT_FALSE(Logging::addFileLogger(tag1, ""));
    Logging::clearAll();
}

TEST_F(TestLogging, doesTagSinkFilteringWork) {
    Logging::addStdLogLogger(LogTag("TAGA"), DAG_SCHEDULER_INFO);
    Logging::debug(LogTag("TAGA"), "Hello World");
    Logging::addStdCerrLogger(LogTag("TAGB"));
    Logging::error(LogTag("TAGB"), "Goodbye World!");
    Logging::addStdCerrLogger(LogTag("TAGC"));
    Logging::error(LogTag("TAGC"), "Goodbye World!!");
    Logging::addStdCerrLogger(LogTag("TAGD"));
    Logging::error(LogTag("TAGD"), "Goodbye World!!!");
}

TEST_F(TestLogging, mktmpdirMakesTmpDirAndCorrectLogsWritten) {
    LogTag log_tag("mktmpdir_makes_tmp_dir");
    Logging::addStdLogLogger(log_tag, DAG_SCHEDULER_DEBUG);
    boost::filesystem::path tmpdir = Logging::mktmpdir();
    boost::filesystem::path log_path = tmpdir / "logs.lg";
    Logging::addFileLogger(
            LogTag("FILE_LOGGER"), log_path, DAG_SCHEDULER_INFO);
    for (std::size_t i = 0; i < 20; ++i) {
        Logging::info(LogTag("FILE_LOGGER"), "Hello World", i);
    }

    EXPECT_TRUE(boost::filesystem::exists(tmpdir));
    EXPECT_TRUE(boost::filesystem::is_directory(tmpdir));
    EXPECT_TRUE(boost::filesystem::exists(log_path));

    sleep(1);

    std::ifstream input_fs(log_path.string());
    std::string line;
    std::size_t index = 0;
    while (std::getline(input_fs, line)) {
        std::stringstream string_stream;
        string_stream << "Hello World " << index;
        EXPECT_TRUE(line.find(string_stream.str(), 0) != std::string::npos)
                << string_stream.str() << " not in " << line;
        ++index;
    }

    boost::filesystem::remove_all(tmpdir);
}

TEST_F(TestLogging, testClearAll) {
    ASSERT_TRUE(Logging::clearAll());
    LogTag log_tag("test_clearAll");
    Logging::addStdLogLogger(log_tag, DAG_SCHEDULER_DEBUG);
    EXPECT_EQ(1u, Logging::s_loggers.size());
    ASSERT_TRUE(Logging::clearAll());
    EXPECT_EQ(0u, Logging::s_loggers.size());
    Logging::info(log_tag, "Check to see if log write to stdout.");
}
}  // namespace com::dag_scheduler
