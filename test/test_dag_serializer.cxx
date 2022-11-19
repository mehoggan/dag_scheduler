#include <gtest/gtest.h>

#include "dag_scheduler/dag_serialization.h"

#include <atomic>
#include <map>
#include <thread>

namespace com
{
  namespace dag_scheduler
  {
    TEST(TestYAMLDagDeserializer, default_ctor)
    {
      EXPECT_NO_THROW(YAMLDagDeserializer());
    }

    TEST(TestYAMLDagDeserializer, make_dag_via_convert_wrong_root_element)
    {
      YAML::Node yaml_node;
      yaml_node["Dag"] = std::map<std::string, std::string>({
        {
          "Title", "Test YAML DAG"
        }
      });
      try {
        yaml_node.as<
          std::unique_ptr<com::dag_scheduler::DAG>>();
      } catch (const YAMLDagDeserializerError &ydde) {
        auto expected = std::string("Root element of \"DAG\" not found in\n") +
          std::string("Dag:\n  Title: Test YAML DAG");
        auto actual = std::string(ydde.what());
        EXPECT_EQ(expected, actual);
      }
    }

    TEST(TestYAMLDagDeserializer, make_dag_via_convert_empty_with_title)
    {
      YAML::Node yaml_node;
      yaml_node["DAG"] = std::map<std::string, std::string>({
        {
          "Title", "Test YAML DAG"
        }
      });
      auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
      ASSERT_NE(nullptr, test_dag);
      EXPECT_EQ("Test YAML DAG", test_dag->title());
      EXPECT_EQ(0ull, test_dag->vertex_count());
    }
  }
}
