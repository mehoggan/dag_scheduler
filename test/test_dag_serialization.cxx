#include <gtest/gtest.h>

#include <algorithm>
#include <boost/exception/exception.hpp>
#include <cctype>
#include <map>
#include <string>

#include "dag_scheduler/dag_serialization.h"
#include "utils/test_environment.h"

namespace com {
namespace dag_scheduler {
const std::string TEST_UUID_1 = "fa6e25e4-0992-4c0e-9429-a3c66175ab91";
const std::string TEST_UUID_2 = "fa6e25e4-0992-4c0e-9429-a3c66175ab92";
const std::string DUMMY_CONFIG_STR_KEY_1 = "StringKey1";
const std::string DUMMY_CONFIG_STR_KEY_2 = "StringKey2";
const std::string DUMMY_CONFIG_STR_KEY_3 = "StringKey3";
const std::string DUMMY_CONFIG_STR_KEY_4 = "StringKey4";
const std::string DUMMY_CONFIG_INT_KEY = "IntKey";
const std::string DUMMY_CONFIG_FLT_KEY = "FloatKey";
const std::string DUMMY_CONFIG_BOL_KEY = "BooleanKey";

void get_base_task_initial_inputs(YAML::Node &out_initial_inputs_node) {
  out_initial_inputs_node = YAML::Node();
  out_initial_inputs_node[DUMMY_CONFIG_STR_KEY_1] =
      std::map<std::string, YAML::Node>(
          {{DUMMY_CONFIG_INT_KEY, YAML::Node(1)},
           {DUMMY_CONFIG_FLT_KEY, YAML::Node(2.0)},
           {DUMMY_CONFIG_BOL_KEY, YAML::Node(false)}});
}

std::string get_base_task_initial_inputs_as_json_str() {
  YAML::Node initial_inputs_node;
  get_base_task_initial_inputs(initial_inputs_node);
  YAML::Emitter json_emitter;
  json_emitter << YAML::DoubleQuoted << YAML::Flow << YAML::BeginSeq
               << initial_inputs_node;
  std::string json_str(json_emitter.c_str() + 1);
  json_str.erase(
      std::remove_if(json_str.begin(), json_str.end(),
                     [](unsigned char c) { return std::isspace(c); }),
      json_str.end());
  return json_str;
}

void get_base_task_configuration(YAML::Node &out_config_node) {
  out_config_node = YAML::Node();
  out_config_node[DUMMY_CONFIG_STR_KEY_2] = 10;
  out_config_node[DUMMY_CONFIG_STR_KEY_3] = 20.0;
  out_config_node[DUMMY_CONFIG_STR_KEY_4] = true;
}

std::string get_base_task_configuration_as_json_str() {
  YAML::Node config_node;
  get_base_task_configuration(config_node);
  YAML::Emitter json_emitter;
  json_emitter << YAML::DoubleQuoted << YAML::Flow << YAML::BeginSeq
               << config_node;
  std::string json_str(json_emitter.c_str() + 1);
  json_str.erase(
      std::remove_if(json_str.begin(), json_str.end(),
                     [](unsigned char c) { return std::isspace(c); }),
      json_str.end());
  return json_str;
}

void get_base_dag_configuration(YAML::Node &out_config_node) {
  out_config_node = YAML::Node();
  out_config_node[DUMMY_CONFIG_STR_KEY_1] = std::map<std::string, YAML::Node>(
      {{DUMMY_CONFIG_INT_KEY, YAML::Node(1)},
       {DUMMY_CONFIG_FLT_KEY, YAML::Node(2.0)},
       {DUMMY_CONFIG_BOL_KEY, YAML::Node(false)}});
  out_config_node[DUMMY_CONFIG_STR_KEY_2] = 10;
  out_config_node[DUMMY_CONFIG_STR_KEY_3] = 20.0;
  out_config_node[DUMMY_CONFIG_STR_KEY_4] = true;
}

std::string get_base_dag_configuration_as_json_str() {
  YAML::Node config_node;
  get_base_dag_configuration(config_node);
  YAML::Emitter json_emitter;
  json_emitter << YAML::DoubleQuoted << YAML::Flow << YAML::BeginSeq
               << config_node;
  std::string json_str(json_emitter.c_str() + 1);
  json_str.erase(
      std::remove_if(json_str.begin(), json_str.end(),
                     [](unsigned char c) { return std::isspace(c); }),
      json_str.end());
  return json_str;
}

TEST(TestYAMLDagDeserializer, output_upto_empty) {
  std::string expected_empty_upto = "\n";
  std::string actual =
      YAMLDagDeserializer::sample_dag_output(YAMLDagDeserializer::UpTo::EMPTY);
  EXPECT_EQ(expected_empty_upto, actual);
}

TEST(TestYAMLDagDeserializer, output_upto_dag) {
  std::string expected_dag_upto =
      std::string("DAG:\n") + std::string("  Title: <optional string>\n") +
      std::string("  Configuation: <optional YAML>\n") +
      std::string("    <valid YAML>\n") + std::string("  ...\n");
  std::string actual =
      YAMLDagDeserializer::sample_dag_output(YAMLDagDeserializer::UpTo::DAG);
  EXPECT_EQ(expected_dag_upto, actual);
}

TEST(TestYAMLDagDeserializer, output_upto_vertices) {
  std::string expected_vertices_upto =
      std::string("DAG:\n") + std::string("  Title: <optional string>\n") +
      std::string("  Configuation: <optional YAML>\n") +
      std::string("    <valid YAML>\n") + std::string("  Vertices:\n") +
      std::string("    ...\n");
  std::string actual = YAMLDagDeserializer::sample_dag_output(
      YAMLDagDeserializer::UpTo::VERTICES);
  EXPECT_EQ(expected_vertices_upto, actual);
}

TEST(TestYAMLDagDeserializer, output_upto_vertex) {
  std::string expected_vertex_upto =
      std::string("DAG:\n") + std::string("  Title: <optional string>\n") +
      std::string("  Configuation: <optional YAML>\n") +
      std::string("    <valid YAML>\n") + std::string("  Vertices:\n") +
      std::string("    - Vertex:\n") +
      std::string("      Name: <optional string>\n") +
      std::string("      UUID: <valid uuid4 string>\n") +
      std::string("        ...\n");
  std::string actual =
      YAMLDagDeserializer::sample_dag_output(YAMLDagDeserializer::UpTo::VERTEX);
  EXPECT_EQ(expected_vertex_upto, actual);
}

TEST(TestYAMLDagDeserializer, output_upto_task) {
  std::string expected_task_upto =
      std::string("DAG:\n") + std::string("  Title: <optional string>\n") +
      std::string("  Configuation: <optional YAML>\n") +
      std::string("    <valid YAML>\n") + std::string("  Vertices:\n") +
      std::string("    - Vertex:\n") +
      std::string("      Name: <optional string>\n") +
      std::string("      UUID: <valid uuid4 string>\n") +
      std::string("      Task:\n") +
      std::string("        Name: <optional string>\n") +
      std::string("        InitialInputs: <optional YAML>\n") +
      std::string("          <valid YAML>\n") +
      std::string("        Configuration: <optional YAML>\n") +
      std::string("          <valid YAML>\n") +
      std::string("        Callback: <optional>\n") +
      std::string("            LibraryName: <string>\n") +
      std::string("            SymbolName: <string>\n") +
      std::string("            Type: <enum {Plugin, Function}>\n") +
      std::string("        ...\n");
  std::string actual =
      YAMLDagDeserializer::sample_dag_output(YAMLDagDeserializer::UpTo::TASK);
  EXPECT_EQ(expected_task_upto, actual);
}

TEST(TestYAMLDagDeserializer, output_upto_stages) {
  std::string expected_stages_upto =
      std::string("DAG:\n") + std::string("  Title: <optional string>\n") +
      std::string("  Configuation: <optional YAML>\n") +
      std::string("    <valid YAML>\n") + std::string("  Vertices:\n") +
      std::string("    - Vertex:\n") +
      std::string("      Name: <optional string>\n") +
      std::string("      UUID: <valid uuid4 string>\n") +
      std::string("      Task:\n") +
      std::string("        Name: <optional string>\n") +
      std::string("        InitialInputs: <optional YAML>\n") +
      std::string("          <valid YAML>\n") +
      std::string("        Configuration: <optional YAML>\n") +
      std::string("          <valid YAML>\n") +
      std::string("        Callback: <optional>\n") +
      std::string("            LibraryName: <string>\n") +
      std::string("            SymbolName: <string>\n") +
      std::string("            Type: <enum {Plugin, Function}>\n") +
      std::string("        Stages:\n") + std::string("          ...\n");
  std::string actual =
      YAMLDagDeserializer::sample_dag_output(YAMLDagDeserializer::UpTo::STAGES);
  EXPECT_EQ(expected_stages_upto, actual);
}

TEST(TestYAMLDagDeserializer, output_upto_stage) {
  std::string expected_stage_upto =
      std::string("DAG:\n") + std::string("  Title: <optional string>\n") +
      std::string("  Configuation: <optional YAML>\n") +
      std::string("    <valid YAML>\n") + std::string("  Vertices:\n") +
      std::string("    - Vertex:\n") +
      std::string("      Name: <optional string>\n") +
      std::string("      UUID: <valid uuid4 string>\n") +
      std::string("      Task:\n") +
      std::string("        Name: <optional string>\n") +
      std::string("        InitialInputs: <optional YAML>\n") +
      std::string("          <valid YAML>\n") +
      std::string("        Configuration: <optional YAML>\n") +
      std::string("          <valid YAML>\n") +
      std::string("        Callback: <optional>\n") +
      std::string("            LibraryName: <string>\n") +
      std::string("            SymbolName: <string>\n") +
      std::string("            Type: <enum {Plugin, Function}>\n") +
      std::string("        Stages:\n") +
      std::string("            - Name: <optional string>\n") +
      std::string("              LibraryName: <string>\n") +
      std::string("              SymbolName: <string>\n") +
      std::string("          ...\n") + std::string("    ...\n");
  std::string actual =
      YAMLDagDeserializer::sample_dag_output(YAMLDagDeserializer::UpTo::STAGE);
  EXPECT_EQ(expected_stage_upto, actual);
}

TEST(TestYAMLDagDeserializer, output_upto_just_connections) {
  std::string expected_connections_upto =
      std::string("  Connections:\n") + std::string("    - Connection:\n") +
      std::string("      From: <valid uuid4 string from vertex>\n") +
      std::string("      To: <valid uuid4 string from vertex>\n") +
      std::string("    ...\n");
  std::string actual = YAMLDagDeserializer::sample_dag_output(
      YAMLDagDeserializer::UpTo::CONNECTIONS);
  EXPECT_EQ(expected_connections_upto, actual);
}

TEST(TestYAMLDagDeserializer, output_full) {
  std::string expected_full =
      std::string("DAG:\n") + std::string("  Title: <optional string>\n") +
      std::string("  Configuation: <optional YAML>\n") +
      std::string("    <valid YAML>\n") + std::string("  Vertices:\n") +
      std::string("    - Vertex:\n") +
      std::string("      Name: <optional string>\n") +
      std::string("      UUID: <valid uuid4 string>\n") +
      std::string("      Task:\n") +
      std::string("        Name: <optional string>\n") +
      std::string("        InitialInputs: <optional YAML>\n") +
      std::string("          <valid YAML>\n") +
      std::string("        Configuration: <optional YAML>\n") +
      std::string("          <valid YAML>\n") +
      std::string("        Callback: <optional>\n") +
      std::string("            LibraryName: <string>\n") +
      std::string("            SymbolName: <string>\n") +
      std::string("            Type: <enum {Plugin, Function}>\n") +
      std::string("        Stages:\n") +
      std::string("            - Name: <optional string>\n") +
      std::string("              LibraryName: <string>\n") +
      std::string("              SymbolName: <string>\n") +
      std::string("          ...\n") + std::string("    ...\n") +
      std::string("  Connections:\n") + std::string("    - Connection:\n") +
      std::string("      From: <valid uuid4 string from vertex>\n") +
      std::string("      To: <valid uuid4 string from vertex>\n") +
      std::string("    ...\n");
  std::string actual = YAMLDagDeserializer::full_sample_output();
  EXPECT_EQ(expected_full, actual);
}

TEST(TestYAMLDagDeserializer, default_ctor) {
  EXPECT_NO_THROW(YAMLDagDeserializer());
}

TEST(TestYAMLDagDeserializer, make_dag_wrong_root_element) {
  YAML::Node yaml_node;
  yaml_node["Dag"] = std::map<std::string, std::string>(
      {{YAMLDagDeserializer::TITLE_KEY, "Test YAML DAG"}});
  try {
    yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  } catch (const YAMLDagDeserializerError &ydde) {
    auto expected =
        std::string("Root element of \"DAG\" not found in\n") +
        std::string("Dag:\n  Title: Test YAML DAG\n") +
        std::string("For example, as in\n") +
        YAMLDagDeserializer::sample_dag_output(YAMLDagDeserializer::UpTo::DAG);
    auto actual = std::string(ydde.what());
    EXPECT_EQ(expected, actual);
  }
}

TEST(TestYAMLDagDeserializer, make_dag_title) {
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, std::string>(
      {{YAMLDagDeserializer::TITLE_KEY, "Test YAML DAG"}});
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(0ull, test_dag->vertex_count());
}

TEST(TestYAMLDagDeserializer, make_dag_title_dag_config) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(0ull, test_dag->vertex_count());
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());
}

TEST(TestYAMLDagDeserializer, make_dag) {
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, std::string>();
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("", test_dag->title());
  EXPECT_EQ(0ull, test_dag->vertex_count());
}

TEST(TestYAMLDagDeserializer, make_dag_dag_config) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("", test_dag->title());
  EXPECT_EQ(0ull, test_dag->vertex_count());
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());
}

TEST(TestYAMLDagDeserializer, make_dag_bad_vertices_not_list_of_obj) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      "This is bad input";
  try {
    yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  } catch (const YAMLDagDeserializerError &ydde) {
    auto expected = std::string("\"Vertices\" must be a ") +
                    std::string("YAML Sequence. As in:\n") +
                    YAMLDagDeserializer::sample_dag_output(
                        YAMLDagDeserializer::UpTo::VERTEX);
    auto actual = std::string(ydde.what());
    EXPECT_EQ(expected, actual);
  }
}

TEST(TestYAMLDagDeserializer, make_dag_label_config) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      std::vector<YAML::Node>{};
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(0ull, test_dag->vertex_count());
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());
}

TEST(TestYAMLDagDeserializer, make_dag_vertices_title) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  YAML::Node second_vertex;
  second_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_2;
  std::vector<YAML::Node> vertices = {first_vertex, second_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
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

TEST(TestYAMLDagDeserializer, make_dag_title_dag_config_vertices) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  YAML::Node second_vertex;
  second_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_2;
  std::vector<YAML::Node> vertices = {first_vertex, second_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
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
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());
}

TEST(TestYAMLDagDeserializer, make_dag_bad_vertices_no_vertex_uuid) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::NAME_KEY] = "A Vertex without a uuid.";
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  try {
    yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(false);
  } catch (const std::runtime_error &excep) {
    std::string expected_what = std::string("invalid node; first ") +
                                std::string("invalid key: \"UUID\"");
    EXPECT_EQ(expected_what, std::string(excep.what()));
  }
}

TEST(TestYAMLDagDeserializer, make_dag_vertices_named_task) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ(uuid1_task_label, uuid1_vertex->get_task()->label());
}

TEST(TestYAMLDagDeserializer, make_dag_vertices_named_task_task_config) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ(uuid1_task_label, uuid1_vertex->get_task()->label());

  std::string uuid1_task_config_str;
  uuid1_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDagDeserializer, make_dag_vertices_named_task_initial_inputs) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ(uuid1_task_label, uuid1_vertex->get_task()->label());

  std::string uuid1_task_initial_inputs_str;
  uuid1_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_vertices_named_task_initial_inputs_task_config) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ(uuid1_task_label, uuid1_vertex->get_task()->label());

  std::string uuid1_task_initial_inputs_str;
  uuid1_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);

  std::string uuid1_task_config_str;
  uuid1_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDagDeserializer, make_dag_label_dag_config_vertices_named_task) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ(uuid1_task_label, uuid1_vertex->get_task()->label());
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_dag_config_vertices_named_task_task_config) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ(uuid1_task_label, uuid1_vertex->get_task()->label());
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_config_str;
  uuid1_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_dag_config_vertices_named_task_initial_inputs) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ(uuid1_task_label, uuid1_vertex->get_task()->label());
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_initial_inputs_str;
  uuid1_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_dag_config_vertices_named_task_initial_inputs_task_config) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ(uuid1_task_label, uuid1_vertex->get_task()->label());
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_config_str;
  uuid1_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

  std::string uuid1_task_initial_inputs_str;
  uuid1_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer, make_dag_label_vertices_task) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ("", uuid1_vertex->get_task()->label());
}

TEST(TestYAMLDagDeserializer, make_dag_label_vertices_task_task_config) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ("", uuid1_vertex->get_task()->label());

  std::string uuid1_task_config_str;
  uuid1_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDagDeserializer, make_dag_label_vertices_task_initial_inputs) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ("", uuid1_vertex->get_task()->label());

  std::string uuid1_task_initial_inputs_str;
  uuid1_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_vertices_task_initial_inputs_task_config) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ("", uuid1_vertex->get_task()->label());

  std::string uuid1_task_config_str;
  uuid1_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

  std::string uuid1_task_initial_inputs_str;
  uuid1_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer, make_dag_label_dag_config_vertices_task) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ("", uuid1_vertex->get_task()->label());
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_dag_config_vertices_task_task_config) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ("", uuid1_vertex->get_task()->label());
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_config_str;
  uuid1_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_dag_config_vertices_task_initial_inputs) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ("", uuid1_vertex->get_task()->label());
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_initial_inputs_str;
  uuid1_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_dag_config_vertices_task_initial_inputs_task_config) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_NE(nullptr, test_dag);
  EXPECT_EQ("Test YAML DAG", test_dag->title());
  EXPECT_EQ(1ull, test_dag->vertex_count());
  UUID uuid_1(TEST_UUID_1);
  std::shared_ptr<DAGVertex> uuid1_vertex =
      test_dag->find_vertex_by_uuid(std::move(uuid_1)).lock();
  EXPECT_TRUE(uuid1_vertex != nullptr);
  EXPECT_EQ("", uuid1_vertex->get_task()->label());
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_config_str;
  uuid1_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

  std::string uuid1_task_initial_inputs_str;
  uuid1_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer, make_dag_title_vertices_bad_task_callback) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  try {
    yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_FALSE(true);
  } catch (const YAML::InvalidNode &excep) {
    EXPECT_EQ(std::string("invalid node; first invalid key: \"LibraryName\""),
              std::string(excep.what()));
  }
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_vertices_bad_task_callback_lib_not_found) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] =
                  YAML::Node("Library DNE");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] = YAML::Node("Method DNE");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  try {
    yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_FALSE(true);
  } catch (const YAMLDagDeserializerError &yaml_deserialization_error) {
    auto expected_error_substr = std::string("Could not load library from ");
    auto actual_error_str = std::string(yaml_deserialization_error.what());
    EXPECT_TRUE(actual_error_str.find(expected_error_substr) !=
                std::string::npos);
  }
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_vertices_bad_task_callback_lib_found_no_method) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Function");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] = YAML::Node("Method DNE");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  try {
    yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_FALSE(true);
  } catch (const YAMLDagDeserializerError &yaml_deserialization_error) {
    auto expected_error_substr = std::string("Failed to load Method DNE from");
    auto actual_error_str = std::string(yaml_deserialization_error.what());
    EXPECT_TRUE(actual_error_str.find(expected_error_substr) !=
                std::string::npos);
  }
}

TEST(TestYAMLDagDeserializer, make_dag_label_vertices_task_callback) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Function");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_is_set());
  test_vertex->get_task()->complete(true);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_vertices_task_callback_task_config) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Function");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_is_set());
  test_vertex->get_task()->complete(true);

  std::string uuid1_task_config_str;
  test_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_vertices_task_callback_initial_inputs) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Function");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_is_set());
  test_vertex->get_task()->complete(true);

  std::string uuid1_task_initial_inputs_str;
  test_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_vertices_task_callback_initial_inputs_task_config) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Function");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_is_set());
  test_vertex->get_task()->complete(true);

  std::string uuid1_task_config_str;
  test_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

  std::string uuid1_task_initial_inputs_str;
  test_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_dag_config_vertices_task_callback) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Function");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_is_set());
  test_vertex->get_task()->complete(true);
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_dag_config_vertices_task_callback_task_config) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Function");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_is_set());
  test_vertex->get_task()->complete(true);
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_config_str;
  test_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_dag_config_vertices_task_callback_initial_inputs) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Function");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_is_set());
  test_vertex->get_task()->complete(true);
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_initial_inputs_str;
  test_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer,
     label_dag_config_vertices_task_callback_initial_inputs_task_config) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Function");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_is_set());
  test_vertex->get_task()->complete(true);
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_config_str;
  test_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

  std::string uuid1_task_initial_inputs_str;
  test_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer, make_dag_label_vertices_task_callback_plugin) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_plugin_is_set());
  test_vertex->get_task()->complete(true);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_vertices_task_callback_plugin_task_config) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_plugin_is_set());
  test_vertex->get_task()->complete(true);

  std::string uuid1_task_config_str;
  test_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_vertices_task_callback_plugin_initial_inputs) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_plugin_is_set());
  test_vertex->get_task()->complete(true);

  std::string uuid1_task_initial_inputs_str;
  test_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_vertices_task_callback_plugin_initial_inputs_task_config) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_plugin_is_set());
  test_vertex->get_task()->complete(true);

  std::string uuid1_task_config_str;
  test_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

  std::string uuid1_task_initial_inputs_str;
  test_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_dag_config_vertices_task_callback_plugin) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_plugin_is_set());
  test_vertex->get_task()->complete(true);
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_dag_config_vertices_task_callback_plugin_task_config) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_plugin_is_set());
  test_vertex->get_task()->complete(true);
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_config_str;
  test_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_dag_config_vertices_task_callback_plugin_initial_inputs) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_plugin_is_set());
  test_vertex->get_task()->complete(true);
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_initial_inputs_str;
  test_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer,
     dag_config_vertices_task_callback_plugin_initial_inputs_task_config) {
  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  EXPECT_TRUE(test_vertex->get_task()->callback_plugin_is_set());
  test_vertex->get_task()->complete(true);
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_config_str;
  test_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

  std::string uuid1_task_initial_inputs_str;
  test_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_vertices_bad_task_callback_plugin_symbol_dne) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin_dne");
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  try {
    yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(false);
  } catch (const YAMLDagDeserializerError &yaml_deserialization_error) {
    auto expected_error_substr =
        std::string("Failed to load task_callback_plugin_dne from");
    auto actual_error_str = std::string(yaml_deserialization_error.what());
    EXPECT_TRUE(actual_error_str.find(expected_error_substr) !=
                std::string::npos);
  }
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_vertices_bad_task_stages_node_wrong_type) {
  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::STAGES_KEY] =
      YAML::Node();
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;

  try {
    yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(false);
  } catch (const std::runtime_error &error) {
    auto expected_error_str = std::string("bad conversion");
    auto actual_error_str = std::string(error.what());
    std::cout << error.what() << std::endl;
    EXPECT_EQ(expected_error_str, actual_error_str);
  }
}

TEST(TestYAMLDagDeserializer, make_dag_label_vertices_task_stages) {
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  std::string stages_lib_path =
      testing::TestEnvironment::PATHING.get_stages_lib_path();

  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  YAML::Node stage_node;
  stage_node[YAMLDagDeserializer::NAME_KEY] = YAML::Node("PrintStage");
  stage_node[YAMLDagDeserializer::LIBRARY_NAME_KEY] =
      YAML::Node(stages_lib_path);
  stage_node[YAMLDagDeserializer::SYMBOL_NAME_KEY] = "print_stage";
  std::vector<YAML::Node> stage_nodes = {stage_node};
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::STAGES_KEY] =
      stage_nodes;
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  std::size_t stage_count = 0;
  test_vertex->get_task()->iterate_stages([&](TaskStage &stage) -> bool {
    std::string actual_type_str = typeid(stage).name();
    auto expected_type_substr = "PrintStage";
    EXPECT_TRUE(actual_type_str.find(expected_type_substr) !=
                std::string::npos);
    ++stage_count;
    bool status = stage.run();
    return status;
  });
  EXPECT_EQ(1, stage_count);
}

TEST(TestYAMLDagDeserializer, make_dag_label_vertices_task_stages_task_config) {
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  std::string stages_lib_path =
      testing::TestEnvironment::PATHING.get_stages_lib_path();

  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  YAML::Node stage_node;
  stage_node[YAMLDagDeserializer::NAME_KEY] = YAML::Node("PrintStage");
  stage_node[YAMLDagDeserializer::LIBRARY_NAME_KEY] =
      YAML::Node(stages_lib_path);
  stage_node[YAMLDagDeserializer::SYMBOL_NAME_KEY] = "print_stage";
  std::vector<YAML::Node> stage_nodes = {stage_node};
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::STAGES_KEY] =
      stage_nodes;
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  std::size_t stage_count = 0;
  test_vertex->get_task()->iterate_stages([&](TaskStage &stage) -> bool {
    std::string actual_type_str = typeid(stage).name();
    auto expected_type_substr = "PrintStage";
    EXPECT_TRUE(actual_type_str.find(expected_type_substr) !=
                std::string::npos);
    ++stage_count;
    bool status = stage.run();
    return status;
  });
  EXPECT_EQ(1, stage_count);

  std::string uuid1_task_config_str;
  test_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_vertices_task_stages_initial_inputs) {
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  std::string stages_lib_path =
      testing::TestEnvironment::PATHING.get_stages_lib_path();

  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  YAML::Node stage_node;
  stage_node[YAMLDagDeserializer::NAME_KEY] = YAML::Node("PrintStage");
  stage_node[YAMLDagDeserializer::LIBRARY_NAME_KEY] =
      YAML::Node(stages_lib_path);
  stage_node[YAMLDagDeserializer::SYMBOL_NAME_KEY] = "print_stage";
  std::vector<YAML::Node> stage_nodes = {stage_node};
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::STAGES_KEY] =
      stage_nodes;
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  std::size_t stage_count = 0;
  test_vertex->get_task()->iterate_stages([&](TaskStage &stage) -> bool {
    std::string actual_type_str = typeid(stage).name();
    auto expected_type_substr = "PrintStage";
    EXPECT_TRUE(actual_type_str.find(expected_type_substr) !=
                std::string::npos);
    ++stage_count;
    bool status = stage.run();
    return status;
  });
  EXPECT_EQ(1, stage_count);

  std::string uuid1_task_initial_inputs_str;
  test_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_vertices_task_stages_initial_inputs_task_config) {
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  std::string stages_lib_path =
      testing::TestEnvironment::PATHING.get_stages_lib_path();

  YAML::Node yaml_node;
  yaml_node[YAMLDagDeserializer::DAG_KEY] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  YAML::Node stage_node;
  stage_node[YAMLDagDeserializer::NAME_KEY] = YAML::Node("PrintStage");
  stage_node[YAMLDagDeserializer::LIBRARY_NAME_KEY] =
      YAML::Node(stages_lib_path);
  stage_node[YAMLDagDeserializer::SYMBOL_NAME_KEY] = "print_stage";
  std::vector<YAML::Node> stage_nodes = {stage_node};
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::STAGES_KEY] =
      stage_nodes;
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  std::size_t stage_count = 0;
  test_vertex->get_task()->iterate_stages([&](TaskStage &stage) -> bool {
    std::string actual_type_str = typeid(stage).name();
    auto expected_type_substr = "PrintStage";
    EXPECT_TRUE(actual_type_str.find(expected_type_substr) !=
                std::string::npos);
    ++stage_count;
    bool status = stage.run();
    return status;
  });
  EXPECT_EQ(1, stage_count);

  std::string uuid1_task_config_str;
  test_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

  std::string uuid1_task_initial_inputs_str;
  test_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer, make_dag_label_dag_config_vertices_task_stages) {
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  std::string stages_lib_path =
      testing::TestEnvironment::PATHING.get_stages_lib_path();

  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  YAML::Node stage_node;
  stage_node[YAMLDagDeserializer::NAME_KEY] = YAML::Node("PrintStage");
  stage_node[YAMLDagDeserializer::LIBRARY_NAME_KEY] =
      YAML::Node(stages_lib_path);
  stage_node[YAMLDagDeserializer::SYMBOL_NAME_KEY] = "print_stage";
  std::vector<YAML::Node> stage_nodes = {stage_node};
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::STAGES_KEY] =
      stage_nodes;
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  std::size_t stage_count = 0;
  test_vertex->get_task()->iterate_stages([&](TaskStage &stage) -> bool {
    std::string actual_type_str = typeid(stage).name();
    auto expected_type_substr = "PrintStage";
    EXPECT_TRUE(actual_type_str.find(expected_type_substr) !=
                std::string::npos);
    ++stage_count;
    bool status = stage.run();
    return status;
  });
  EXPECT_EQ(1, stage_count);
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_dag_config_vertices_task_stages_task_config) {
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  std::string stages_lib_path =
      testing::TestEnvironment::PATHING.get_stages_lib_path();

  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  YAML::Node stage_node;
  stage_node[YAMLDagDeserializer::NAME_KEY] = YAML::Node("PrintStage");
  stage_node[YAMLDagDeserializer::LIBRARY_NAME_KEY] =
      YAML::Node(stages_lib_path);
  stage_node[YAMLDagDeserializer::SYMBOL_NAME_KEY] = "print_stage";
  std::vector<YAML::Node> stage_nodes = {stage_node};
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::STAGES_KEY] =
      stage_nodes;
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  std::size_t stage_count = 0;
  test_vertex->get_task()->iterate_stages([&](TaskStage &stage) -> bool {
    std::string actual_type_str = typeid(stage).name();
    auto expected_type_substr = "PrintStage";
    EXPECT_TRUE(actual_type_str.find(expected_type_substr) !=
                std::string::npos);
    ++stage_count;
    bool status = stage.run();
    return status;
  });
  EXPECT_EQ(1, stage_count);
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_config_str;
  test_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDagDeserializer,
     make_dag_label_dag_config_vertices_task_stages_initial_inputs) {
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  std::string stages_lib_path =
      testing::TestEnvironment::PATHING.get_stages_lib_path();

  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  YAML::Node stage_node;
  stage_node[YAMLDagDeserializer::NAME_KEY] = YAML::Node("PrintStage");
  stage_node[YAMLDagDeserializer::LIBRARY_NAME_KEY] =
      YAML::Node(stages_lib_path);
  stage_node[YAMLDagDeserializer::SYMBOL_NAME_KEY] = "print_stage";
  std::vector<YAML::Node> stage_nodes = {stage_node};
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::STAGES_KEY] =
      stage_nodes;
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  std::size_t stage_count = 0;
  test_vertex->get_task()->iterate_stages([&](TaskStage &stage) -> bool {
    std::string actual_type_str = typeid(stage).name();
    auto expected_type_substr = "PrintStage";
    EXPECT_TRUE(actual_type_str.find(expected_type_substr) !=
                std::string::npos);
    ++stage_count;
    bool status = stage.run();
    return status;
  });
  EXPECT_EQ(1, stage_count);
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_initial_inputs_str;
  test_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDagDeserializer,
     label_dag_config_vertices_task_stages_initial_inputs_task_config) {
  std::string lib_path = testing::TestEnvironment::PATHING.get_lib_path();
  std::string stages_lib_path =
      testing::TestEnvironment::PATHING.get_stages_lib_path();

  YAML::Node dag_configuration_node;
  get_base_dag_configuration(dag_configuration_node);
  YAML::Node yaml_node;
  yaml_node["DAG"] = std::map<std::string, YAML::Node>(
      {{YAMLDagDeserializer::TITLE_KEY, YAML::Node("Test YAML DAG")},
       {YAMLDagDeserializer::CONFIGURATION_KEY, dag_configuration_node}});
  YAML::Node first_vertex;
  first_vertex[YAMLDagDeserializer::UUID_KEY] = TEST_UUID_1;
  first_vertex[YAMLDagDeserializer::TASK_KEY] = YAML::Node();
  const std::string uuid1_task_label = "A named task";
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::NAME_KEY] =
      YAML::Node(uuid1_task_label);

  YAML::Node task_config;
  get_base_task_configuration(task_config);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CONFIGURATION_KEY] = task_config;

  YAML::Node task_initial_inputs;
  get_base_task_initial_inputs(task_initial_inputs);
  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::INITIAL_INPUTS_KEY] = task_initial_inputs;

  first_vertex[YAMLDagDeserializer::TASK_KEY]
              [YAMLDagDeserializer::CALLBACK_KEY] = YAML::Node();
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::LIBRARY_NAME_KEY] = YAML::Node(lib_path);
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::CALLBACK_TYPE_KEY] = YAML::Node("Plugin");
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::CALLBACK_KEY]
              [YAMLDagDeserializer::SYMBOL_NAME_KEY] =
                  YAML::Node("task_callback_plugin");
  YAML::Node stage_node;
  stage_node[YAMLDagDeserializer::NAME_KEY] = YAML::Node("PrintStage");
  stage_node[YAMLDagDeserializer::LIBRARY_NAME_KEY] =
      YAML::Node(stages_lib_path);
  stage_node[YAMLDagDeserializer::SYMBOL_NAME_KEY] = "print_stage";
  std::vector<YAML::Node> stage_nodes = {stage_node};
  first_vertex[YAMLDagDeserializer::TASK_KEY][YAMLDagDeserializer::STAGES_KEY] =
      stage_nodes;
  std::vector<YAML::Node> vertices = {first_vertex};
  yaml_node[YAMLDagDeserializer::DAG_KEY][YAMLDagDeserializer::VERTICES_KEY] =
      vertices;
  auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
  ASSERT_TRUE(test_dag != nullptr);
  std::shared_ptr<DAGVertex> test_vertex =
      test_dag->find_vertex_by_uuid(UUID(TEST_UUID_1)).lock();
  EXPECT_TRUE(test_vertex != nullptr);
  EXPECT_TRUE(test_vertex->get_task() != nullptr);
  std::size_t stage_count = 0;
  test_vertex->get_task()->iterate_stages([&](TaskStage &stage) -> bool {
    std::string actual_type_str = typeid(stage).name();
    auto expected_type_substr = "PrintStage";
    EXPECT_TRUE(actual_type_str.find(expected_type_substr) !=
                std::string::npos);
    ++stage_count;
    bool status = stage.run();
    return status;
  });
  EXPECT_EQ(1, stage_count);
  std::string expected_config_json_str;
  test_dag->json_config_str(expected_config_json_str);
  EXPECT_EQ(expected_config_json_str, get_base_dag_configuration_as_json_str());

  std::string uuid1_task_config_str;
  test_vertex->get_task()->json_config_str(uuid1_task_config_str);
  std::string expected_task_config_str =
      get_base_task_configuration_as_json_str();
  EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

  std::string uuid1_task_initial_inputs_str;
  test_vertex->get_task()->json_initial_inputs_str(
      uuid1_task_initial_inputs_str);
  std::string expected_task_initial_inputs_str =
      get_base_task_initial_inputs_as_json_str();
  EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);

  std::cout << yaml_node << "Dude" << std::endl;
}
}  // namespace dag_scheduler
}  // namespace com
