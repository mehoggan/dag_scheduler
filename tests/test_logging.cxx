#include <gtest/gtest.h>

#include <dag_scheduler/logging.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <streambuf>

namespace com
{
  namespace dag_scheduler
  {
    class TestLogging : public ::testing::Test
    {
    protected:
      virtual void SetUp()
      {
        logging::init();
      }

      virtual void TearDown()
      {}
    };

    TEST_F(TestLogging, does_tag_sink_filtering_work)
    {
      logging::add_std_log_logger(log_tag("TAGA"), DAG_SCHEDULER_INFO);
      logging::debug(log_tag("TAGA"), "Hello World");
      logging::add_std_cerr_logger(log_tag("TAGB"));
      logging::error(log_tag("TAGB"), "Goodbye World!");
      logging::add_std_cerr_logger(log_tag("TAGC"));
      logging::error(log_tag("TAGC"), "Goodbye World!!");
      logging::add_std_cerr_logger(log_tag("TAGD"));
      logging::error(log_tag("TAGD"), "Goodbye World!!!");
    }

    TEST_F(TestLogging, mktmpdir_makes_tmp_dir_and_correct_logs_written)
    {
      log_tag tag("mktmpdir_makes_tmp_dir");
      logging::add_std_log_logger(tag, DAG_SCHEDULER_DEBUG);
      boost::filesystem::path tmpdir = logging::mktmpdir();
      boost::filesystem::path log_path = tmpdir / "logs.lg";
      logging::add_file_logger(log_tag("FILE_LOGGER"), log_path,
        DAG_SCHEDULER_INFO);
      for (std::size_t i = 0; i < 20; ++i) {
        logging::info(log_tag("FILE_LOGGER"), "Hello World", i);
      }

      EXPECT_TRUE(boost::filesystem::exists(tmpdir));
      EXPECT_TRUE(boost::filesystem::is_directory(tmpdir));
      EXPECT_TRUE(boost::filesystem::exists(log_path));

      sleep(1);

      std::ifstream ifs(log_path.string());
      std::string line;
      std::size_t i = 0;
      while(std::getline(ifs, line)) {
        std::stringstream ss;
        ss << "Hello World " << i;
        EXPECT_TRUE(line.find(ss.str(), 0) != std::string::npos) <<
          ss.str() << " not in " << line;
        ++i;
      }


      boost::filesystem::remove_all(tmpdir);
    }
  }
}
