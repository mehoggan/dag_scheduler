#include <gtest/gtest.h>

#include "dag_scheduler/dag_serialization.h"

#include <atomic>
#include <map>
#include <thread>

namespace com
{
  namespace dag_scheduler
  {
    TEST(TestYAMLDagDeserializer, sample_dag_upto_output_empty)
    {
      std::string expected_empty_upto = "\n";
      std::string actual = YAMLDagDeserializer::sample_dag_output(
        YAMLDagDeserializer::UpTo::EMPTY);
      EXPECT_EQ(expected_empty_upto, actual);
    }

    TEST(TestYAMLDagDeserializer, sample_dag_upto_output_dag)
    {
      std::string expected_dag_upto =
        std::string("DAG:\n") +
        std::string("  Title: <optional string>\n") +
        std::string("  ...\n");
      std::string actual = YAMLDagDeserializer::sample_dag_output(
        YAMLDagDeserializer::UpTo::DAG);
      EXPECT_EQ(expected_dag_upto, actual);
    }

    TEST(TestYAMLDagDeserializer, sample_dag_upto_output_vertices)
    {
      std::string expected_vertices_upto =
        std::string("DAG:\n") +
        std::string("  Title: <optional string>\n") +
        std::string("  Vertices:\n") +
        std::string("    ...\n");
      std::string actual = YAMLDagDeserializer::sample_dag_output(
        YAMLDagDeserializer::UpTo::VERTICES);
      EXPECT_EQ(expected_vertices_upto, actual);
    }

    TEST(TestYAMLDagDeserializer, sample_dag_upto_output_vertex)
    {
      std::string expected_vertex_upto =
        std::string("DAG:\n") +
        std::string("  Title: <optional string>\n") +
        std::string("  Vertices:\n") +
        std::string("    - Vertex:\n") +
        std::string("      Name: <optional string>\n") +
        std::string("      UUID: <valid uuid4 string>\n") +
        std::string("        ...\n");
      std::string actual = YAMLDagDeserializer::sample_dag_output(
        YAMLDagDeserializer::UpTo::VERTEX);
      EXPECT_EQ(expected_vertex_upto, actual);
    }

    TEST(TestYAMLDagDeserializer, sample_dag_upto_output_task)
    {
      std::string expected_task_upto =
        std::string("DAG:\n") +
        std::string("  Title: <optional string>\n") +
        std::string("  Vertices:\n") +
        std::string("    - Vertex:\n") +
        std::string("      Name: <optional string>\n") +
        std::string("      UUID: <valid uuid4 string>\n") +
        std::string("      Task:\n") +
        std::string("        Name: <optional string>\n") +
        std::string("        ...\n");
      std::string actual = YAMLDagDeserializer::sample_dag_output(
        YAMLDagDeserializer::UpTo::TASK);
      EXPECT_EQ(expected_task_upto, actual);
    }

    TEST(TestYAMLDagDeserializer, sample_dag_upto_output_stages)
    {
      std::string expected_stages_upto =
        std::string("DAG:\n") +
        std::string("  Title: <optional string>\n") +
        std::string("  Vertices:\n") +
        std::string("    - Vertex:\n") +
        std::string("      Name: <optional string>\n") +
        std::string("      UUID: <valid uuid4 string>\n") +
        std::string("      Task:\n") +
        std::string("        Name: <optional string>\n") +
        std::string("        Stages:\n") +
        std::string("          ...\n");
      std::string actual = YAMLDagDeserializer::sample_dag_output(
        YAMLDagDeserializer::UpTo::STAGES);
      EXPECT_EQ(expected_stages_upto, actual);
    }

    TEST(TestYAMLDagDeserializer, sample_dag_upto_output_stage)
    {
      std::string expected_stage_upto =
        std::string("DAG:\n") +
        std::string("  Title: <optional string>\n") +
        std::string("  Vertices:\n") +
        std::string("    - Vertex:\n") +
        std::string("      Name: <optional string>\n") +
        std::string("      UUID: <valid uuid4 string>\n") +
        std::string("      Task:\n") +
        std::string("        Name: <optional string>\n") +
        std::string("        Stages:\n") +
        std::string("          - Stage:\n") +
        std::string("            Name: <optional string>\n") +
        std::string("            Type: <namespaced C++ class>\n") +
        std::string("          ...\n") +
        std::string("    ...\n");
      std::string actual = YAMLDagDeserializer::sample_dag_output(
        YAMLDagDeserializer::UpTo::STAGE);
      EXPECT_EQ(expected_stage_upto, actual);
    }

    TEST(TestYAMLDagDeserializer, sample_dag_upto_output_just_connections)
    {
      std::string expected_connections_upto =
        std::string("  Connections:\n") +
        std::string("    - Connection:\n") +
        std::string("      From: <valid uuid4 string from vertex>\n") +
        std::string("      To: <valid uuid4 string from vertex>\n") +
        std::string("    ...\n");
      std::string actual = YAMLDagDeserializer::sample_dag_output(
        YAMLDagDeserializer::UpTo::CONNECTIONS);
      EXPECT_EQ(expected_connections_upto, actual);
    }

    TEST(TestYAMLDagDeserializer, sample_dag_upto_output_full)
    {
      std::string expected_full =
        std::string("DAG:\n") +
        std::string("  Title: <optional string>\n") +
        std::string("  Vertices:\n") +
        std::string("    - Vertex:\n") +
        std::string("      Name: <optional string>\n") +
        std::string("      UUID: <valid uuid4 string>\n") +
        std::string("      Task:\n") +
        std::string("        Name: <optional string>\n") +
        std::string("        Stages:\n") +
        std::string("          - Stage:\n") +
        std::string("            Name: <optional string>\n") +
        std::string("            Type: <namespaced C++ class>\n") +
        std::string("          ...\n") +
        std::string("    ...\n") +
        std::string("  Connections:\n") +
        std::string("    - Connection:\n") +
        std::string("      From: <valid uuid4 string from vertex>\n") +
        std::string("      To: <valid uuid4 string from vertex>\n") +
        std::string("    ...\n");
      std::string actual = YAMLDagDeserializer::full_sample_output();
      EXPECT_EQ(expected_full, actual);
    }

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
          std::string("Dag:\n  Title: Test YAML DAG\n") +
          std::string("For example, as in\n") +
          YAMLDagDeserializer::sample_dag_output(
            YAMLDagDeserializer::UpTo::DAG);
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

    TEST(TestYAMLDagDeserializer, make_dag_via_convert_empty_with_no_title)
    {
      YAML::Node yaml_node;
      yaml_node["DAG"] = std::map<std::string, std::string>();
      auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
      ASSERT_NE(nullptr, test_dag);
      EXPECT_EQ("", test_dag->title());
      EXPECT_EQ(0ull, test_dag->vertex_count());
    }
  }
}
