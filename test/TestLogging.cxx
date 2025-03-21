#include <dag_scheduler/logging.h>
#include <gtest/gtest.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

namespace com {
namespace dag_scheduler {
class TestLogging : public ::testing::Test {
protected:
    virtual void SetUp() { Logging::init(); }

    virtual void TearDown() {}
};

TEST_F(TestLogging, add_loggers_no_duplicates) {
    LogTag tag1("tag1");

    Logging::clear_all();
    EXPECT_TRUE(Logging::add_std_cout_logger(tag1));
    EXPECT_FALSE(Logging::add_std_cout_logger(tag1));
    Logging::clear_all();
    EXPECT_TRUE(Logging::add_std_cerr_logger(tag1));
    EXPECT_FALSE(Logging::add_std_cerr_logger(tag1));
    Logging::clear_all();
    EXPECT_TRUE(Logging::add_std_log_logger(tag1));
    EXPECT_FALSE(Logging::add_std_log_logger(tag1));
    Logging::clear_all();
    EXPECT_TRUE(Logging::add_file_logger(tag1, ""));
    EXPECT_FALSE(Logging::add_file_logger(tag1, ""));
    Logging::clear_all();
}

TEST_F(TestLogging, does_tag_sink_filtering_work) {
    Logging::add_std_log_logger(LogTag("TAGA"), DAG_SCHEDULER_INFO);
    Logging::debug(LogTag("TAGA"), "Hello World");
    Logging::add_std_cerr_logger(LogTag("TAGB"));
    Logging::error(LogTag("TAGB"), "Goodbye World!");
    Logging::add_std_cerr_logger(LogTag("TAGC"));
    Logging::error(LogTag("TAGC"), "Goodbye World!!");
    Logging::add_std_cerr_logger(LogTag("TAGD"));
    Logging::error(LogTag("TAGD"), "Goodbye World!!!");
}

TEST_F(TestLogging, mktmpdir_makes_tmp_dir_and_correct_logs_written) {
    LogTag tag("mktmpdir_makes_tmp_dir");
    Logging::add_std_log_logger(tag, DAG_SCHEDULER_DEBUG);
    boost::filesystem::path tmpdir = Logging::mktmpdir();
    boost::filesystem::path log_path = tmpdir / "logs.lg";
    Logging::add_file_logger(
            LogTag("FILE_LOGGER"), log_path, DAG_SCHEDULER_INFO);
    for (std::size_t i = 0; i < 20; ++i) {
        Logging::info(LogTag("FILE_LOGGER"), "Hello World", i);
    }

    EXPECT_TRUE(boost::filesystem::exists(tmpdir));
    EXPECT_TRUE(boost::filesystem::is_directory(tmpdir));
    EXPECT_TRUE(boost::filesystem::exists(log_path));

    sleep(1);

    std::ifstream ifs(log_path.string());
    std::string line;
    std::size_t i = 0;
    while (std::getline(ifs, line)) {
        std::stringstream ss;
        ss << "Hello World " << i;
        EXPECT_TRUE(line.find(ss.str(), 0) != std::string::npos)
                << ss.str() << " not in " << line;
        ++i;
    }

    boost::filesystem::remove_all(tmpdir);
}

TEST_F(TestLogging, test_clear_all) {
    ASSERT_TRUE(Logging::clear_all());
    LogTag tag("test_clear_all");
    Logging::add_std_log_logger(tag, DAG_SCHEDULER_DEBUG);
    EXPECT_EQ(1u, Logging::loggers_.size());
    ASSERT_TRUE(Logging::clear_all());
    EXPECT_EQ(0u, Logging::loggers_.size());
    Logging::info(tag, "Check to see if log write to stdout.");
}
}  // namespace dag_scheduler
}  // namespace com
