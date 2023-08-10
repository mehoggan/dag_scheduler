#include <gtest/gtest.h>

#include "utils/test_environment.h"

#include "dag_scheduler/dag_serialization.h"

#include <atomic>
#include <exception>
#include <map>
#include <stdexcept>
#include <thread>

namespace com
{
  namespace dag_scheduler
  {
    const std::string TEST_UUID_1 = "fa6e25e4-0992-4c0e-9429-a3c66175ab91";
    const std::string TEST_UUID_2 = "fa6e25e4-0992-4c0e-9429-a3c66175ab92";

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
        std::string("        Callback: <optional>\n") +
        std::string("            LibraryName: <string>\n") +
        std::string("            SymbolName: <string>\n") +
        std::string("            Type: <enum {Plugin, Function}>\n") +
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
        std::string("        Callback: <optional>\n") +
        std::string("            LibraryName: <string>\n") +
        std::string("            SymbolName: <string>\n") +
        std::string("            Type: <enum {Plugin, Function}>\n") +
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
        std::string("        Callback: <optional>\n") +
        std::string("            LibraryName: <string>\n") +
        std::string("            SymbolName: <string>\n") +
        std::string("            Type: <enum {Plugin, Function}>\n") +
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
        std::string("        Callback: <optional>\n") +
        std::string("            LibraryName: <string>\n") +
        std::string("            SymbolName: <string>\n") +
        std::string("            Type: <enum {Plugin, Function}>\n") +
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
          YAMLDagDeserializer::TITLE_KEY, "Test YAML DAG"
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
          YAMLDagDeserializer::TITLE_KEY, "Test YAML DAG"
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

    TEST(TestYAMLDagDeserializer, make_dag_vertices_not_list_of_obj)
    {
      YAML::Node yaml_node;
      yaml_node[YAMLDagDeserializer::DAG_KEY] =
        std::map<std::string, YAML::Node>({
          {YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
      yaml_node[YAMLDagDeserializer::DAG_KEY]
        [YAMLDagDeserializer::VERTICES_KEY] = "This is bad input";
      try {
        yaml_node.as<
          std::unique_ptr<com::dag_scheduler::DAG>>();
      } catch (const YAMLDagDeserializerError &ydde) {
        auto expected = std::string("\"Vertices\" must be a ") +
          std::string("YAML Sequence. As in:\n") +
          YAMLDagDeserializer::sample_dag_output(
            YAMLDagDeserializer::UpTo::VERTEX);
        auto actual = std::string(ydde.what());
        EXPECT_EQ(expected, actual);
      }
    }

    TEST(TestYAMLDagDeserializer, make_dag_via_convert_vertices_empty)
    {
      YAML::Node yaml_node;
      yaml_node[YAMLDagDeserializer::DAG_KEY] =
        std::map<std::string, YAML::Node>({
          {YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
      yaml_node[YAMLDagDeserializer::DAG_KEY]
        [YAMLDagDeserializer::VERTICES_KEY] = std::vector<YAML::Node> {};
      auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
      ASSERT_NE(nullptr, test_dag);
      EXPECT_EQ("Test YAML DAG", test_dag->title());
      EXPECT_EQ(0ull, test_dag->vertex_count());
    }

    TEST(TestYAMLDagDeserializer, make_dag_via_convert_vertices_non_empty_good)
    {
      YAML::Node yaml_node;
      yaml_node[YAMLDagDeserializer::DAG_KEY] =
        std::map<std::string, YAML::Node>({
          {YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
      YAML::Node first_vertex;
      first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
      YAML::Node second_vertex;
      second_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_2;
      std::vector<YAML::Node> vertices = {first_vertex, second_vertex};
      yaml_node[YAMLDagDeserializer::DAG_KEY]
        [YAMLDagDeserializer::VERTICES_KEY] = vertices;
      auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
      ASSERT_NE(nullptr, test_dag);
      EXPECT_EQ("Test YAML DAG", test_dag->title());
      EXPECT_EQ(2ull, test_dag->vertex_count());
      UUID uuid_1(TEST_UUID_1);
      EXPECT_TRUE(test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock() !=
        nullptr);
      UUID uuid_2(TEST_UUID_1);
      EXPECT_TRUE(test_dag->find_vertex_by_uuid(std::move(uuid_2)).lock() !=
        nullptr);
    }

    TEST(TestYAMLDagDeserializer, make_dag_via_convert_vertices_no_uuid)
    {
      YAML::Node yaml_node;
      yaml_node[YAMLDagDeserializer::DAG_KEY] =
        std::map<std::string, YAML::Node>({
          {YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
      YAML::Node first_vertex;
      first_vertex[YAMLDagDeserializer::NAME_KEY] = "A Vertex without a uuid.";
      std::vector<YAML::Node> vertices = {first_vertex};
      yaml_node[YAMLDagDeserializer::DAG_KEY]
        [YAMLDagDeserializer::VERTICES_KEY] = vertices;
      try {
        yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
        ASSERT_TRUE(false);
      } catch (const YAML::InvalidNode &excep) {
        std::string expected_what = std::string("invalid node; first ") +
         std::string("invalid key: \"UUID\"");
        EXPECT_EQ(expected_what, std::string(excep.what()));
      }
    }

    TEST(TestYAMLDagDeserializer, make_dag_via_vertices_with_named_task)
    {
      YAML::Node yaml_node;
      yaml_node[YAMLDagDeserializer::DAG_KEY] =
        std::map<std::string, YAML::Node>({
          {YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
      YAML::Node first_vertex;
      first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
      first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
      const std::string uuid1_task_label = "A named task";
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::NAME_KEY] = YAML::Node(uuid1_task_label);
      std::vector<YAML::Node> vertices = {first_vertex};
      yaml_node[YAMLDagDeserializer::DAG_KEY]
        [YAMLDagDeserializer::VERTICES_KEY] = vertices;
      auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
      ASSERT_NE(nullptr, test_dag);
      EXPECT_EQ("Test YAML DAG", test_dag->title());
      EXPECT_EQ(1ull, test_dag->vertex_count());
      UUID uuid_1(TEST_UUID_1);
      std::shared_ptr<DAGVertex> uuid1_vertex = test_dag->find_vertex_by_uuid(
        std::move(uuid_1)).lock();
      EXPECT_TRUE(uuid1_vertex != nullptr);
      EXPECT_EQ(uuid1_task_label, uuid1_vertex->get_task()->label());
    }

    TEST(TestYAMLDagDeserializer, make_dag_via_vertices_with_non_named_task)
    {
      YAML::Node yaml_node;
      yaml_node[YAMLDagDeserializer::DAG_KEY] =
        std::map<std::string, YAML::Node>({
          {YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
      YAML::Node first_vertex;
      first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
      first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
      std::vector<YAML::Node> vertices = {first_vertex};
      yaml_node[YAMLDagDeserializer::DAG_KEY]
        [YAMLDagDeserializer::VERTICES_KEY] = vertices;
      auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
      ASSERT_NE(nullptr, test_dag);
      EXPECT_EQ("Test YAML DAG", test_dag->title());
      EXPECT_EQ(1ull, test_dag->vertex_count());
      UUID uuid_1(TEST_UUID_1);
      std::shared_ptr<DAGVertex> uuid1_vertex = test_dag->find_vertex_by_uuid(
        std::move(uuid_1)).lock();
      EXPECT_TRUE(uuid1_vertex != nullptr);
      EXPECT_EQ("", uuid1_vertex->get_task()->label());
    }

    TEST(TestYAMLDagDeserializer,
      make_dag_via_vertices_with_ill_formed_callback)
    {
      YAML::Node yaml_node;
      yaml_node[YAMLDagDeserializer::DAG_KEY] =
        std::map<std::string, YAML::Node>({
          {YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
      YAML::Node first_vertex;
      first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
      first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
      const std::string uuid1_task_label = "A named task";
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::NAME_KEY] = YAML::Node(uuid1_task_label);
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
      std::vector<YAML::Node> vertices = {first_vertex};
      yaml_node[YAMLDagDeserializer::DAG_KEY]
        [YAMLDagDeserializer::VERTICES_KEY] = vertices;
      try {
        yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
        ASSERT_FALSE(true);
      } catch (const YAML::InvalidNode &excep) {
        EXPECT_EQ(
          std::string("invalid node; first invalid key: \"LibraryName\""),
          std::string(excep.what()));
      }
    }

    TEST(TestYAMLDagDeserializer,
      make_dag_via_vertices_with_ill_formed_callback_lib_not_found)
    {
      YAML::Node yaml_node;
      yaml_node[YAMLDagDeserializer::DAG_KEY] =
        std::map<std::string, YAML::Node>({
          {YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
      YAML::Node first_vertex;
      first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
      first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
      const std::string uuid1_task_label = "A named task";
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::NAME_KEY] = YAML::Node(uuid1_task_label);
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::CALLBACK_KEY]
        [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node("Library DNE");
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::CALLBACK_KEY]
        [YAMLDagDeserializer::SYMBOL_NAME_KEY] = YAML::Node("Method DNE");
      std::vector<YAML::Node> vertices = {first_vertex};
      yaml_node[YAMLDagDeserializer::DAG_KEY]
        [YAMLDagDeserializer::VERTICES_KEY] = vertices;
      try {
        yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
        ASSERT_FALSE(true);
      } catch (const YAMLDagDeserializerError &yaml_deserialization_error) {
        auto expected_error_substr =
          std::string("Could not load library from ");
        auto actual_error_str = std::string(yaml_deserialization_error.what());
        EXPECT_TRUE(actual_error_str.find(expected_error_substr) !=
          std::string::npos);
      }
    }

    TEST(TestYAMLDagDeserializer,
      make_dag_via_vertices_with_ill_formed_callback_lib_found_no_method)
    {
      YAML::Node yaml_node;
      yaml_node[YAMLDagDeserializer::DAG_KEY] =
        std::map<std::string, YAML::Node>({
          {YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
      YAML::Node first_vertex;
      first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
      first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
      const std::string uuid1_task_label = "A named task";
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::NAME_KEY] = YAML::Node(uuid1_task_label);
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
      std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::CALLBACK_KEY]
        [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::CALLBACK_KEY]
        [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Function");
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::CALLBACK_KEY]
        [YAMLDagDeserializer::SYMBOL_NAME_KEY] = YAML::Node("Method DNE");
      std::vector<YAML::Node> vertices = {first_vertex};
      yaml_node[YAMLDagDeserializer::DAG_KEY]
        [YAMLDagDeserializer::VERTICES_KEY] = vertices;
      try {
        yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
        ASSERT_FALSE(true);
      } catch (const YAMLDagDeserializerError &yaml_deserialization_error) {
        auto expected_error_substr =
          std::string("Failed to load Method DNE from");
        auto actual_error_str = std::string(yaml_deserialization_error.what());
        EXPECT_TRUE(actual_error_str.find(expected_error_substr) !=
          std::string::npos);
      }
    }

    TEST(TestYAMLDagDeserializer, make_dag_via_vertices_with_task_callback)
    {
      YAML::Node yaml_node;
      yaml_node[YAMLDagDeserializer::DAG_KEY] =
        std::map<std::string, YAML::Node>({
          {YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
      YAML::Node first_vertex;
      first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
      first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
      const std::string uuid1_task_label = "A named task";
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::NAME_KEY] = YAML::Node(uuid1_task_label);
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
      std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::CALLBACK_KEY]
        [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::CALLBACK_KEY]
        [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Function");
      first_vertex[YAMLDagDeserializer::TASK_KEY]
        [YAMLDagDeserializer::CALLBACK_KEY]
        [YAMLDagDeserializer::SYMBOL_NAME_KEY] = YAML::Node("task_callback");
      std::vector<YAML::Node> vertices = {first_vertex};
      yaml_node[YAMLDagDeserializer::DAG_KEY]
        [YAMLDagDeserializer::VERTICES_KEY] = vertices;
      auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
      ASSERT_TRUE(test_dag != nullptr);
      std::shared_ptr<DAGVertex> test_vertex =
        test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
      EXPECT_TRUE(test_vertex != nullptr);
      EXPECT_TRUE(test_vertex->get_task() != nullptr);
      EXPECT_TRUE(test_vertex->get_task()->callback_is_set());
    }
  }
}
