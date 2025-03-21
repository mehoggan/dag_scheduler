#include <dag_scheduler/logging.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include <string>

namespace com {
namespace dag_scheduler {
namespace testing {
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

    virtual void SetUp();
};
}  // namespace testing
}  // namespace dag_scheduler
}  // namespace com
