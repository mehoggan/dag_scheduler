////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include <algorithm>
#include <boost/exception/exception.hpp>
#include <cctype>
#include <string>

#include "dag_scheduler/DagSerialization.h"
#include "utils/TestEnvironment.h"

namespace com::dag_scheduler {
const char test_uuid_1[] = "fa6e25e4-0992-4c0e-9429-a3c66175ab91";
const char test_uuid_2[] = "fa6e25e4-0992-4c0e-9429-a3c66175ab92";
const char dummy_config_str_key_1[] = "StringKey1";
const char dummy_config_str_key_2[] = "StringKey2";
const char dummy_config_str_key_3[] = "StringKey3";
const char dummy_config_str_key_4[] = "StringKey4";
const char dummy_config_int_key[] = "IntKey";
const char dummy_config_flt_key[] = "FloatKey";
const char dummy_config_bol_key[] = "BooleanKey";

void getBaseTaskInitialInputs(YAML::Node& out_initial_inputs_node) {
    out_initial_inputs_node = YAML::Node();
    out_initial_inputs_node[dummy_config_str_key_1] =
            std::map<std::string, YAML::Node>(
                    {{dummy_config_int_key, YAML::Node(1)},
                     {dummy_config_flt_key, YAML::Node(2.0)},
                     {dummy_config_bol_key, YAML::Node(false)}});
}

std::string getBaseTaskInitialInputsAsJsonStr() {
    YAML::Node initial_inputs_node;
    getBaseTaskInitialInputs(initial_inputs_node);
    YAML::Emitter json_emitter;
    json_emitter << YAML::DoubleQuoted << YAML::Flow << YAML::BeginSeq
                 << initial_inputs_node;
    std::string json_str(json_emitter.c_str() + 1);
    json_str.erase(std::remove_if(json_str.begin(),
                                  json_str.end(),
                                  [](unsigned char rune) {
                                      return std::isspace(rune);
                                  }),
                   json_str.end());
    return json_str;
}

void getBaseTaskConfiguration(YAML::Node& out_config_node) {
    out_config_node = YAML::Node();
    out_config_node[dummy_config_str_key_2] = 10;
    out_config_node[dummy_config_str_key_3] = 20.0;
    out_config_node[dummy_config_str_key_4] = true;
}

std::string getBaseTaskConfigurationAsJsonStr() {
    YAML::Node config_node;
    getBaseTaskConfiguration(config_node);
    YAML::Emitter json_emitter;
    json_emitter << YAML::DoubleQuoted << YAML::Flow << YAML::BeginSeq
                 << config_node;
    std::string json_str(json_emitter.c_str() + 1);
    json_str.erase(std::remove_if(json_str.begin(),
                                  json_str.end(),
                                  [](unsigned char rune) {
                                      return std::isspace(rune);
                                  }),
                   json_str.end());
    return json_str;
}

void getBaseDAGConfiguration(YAML::Node& out_config_node) {
    out_config_node = YAML::Node();
    out_config_node[dummy_config_str_key_1] =
            std::map<std::string, YAML::Node>(
                    {{dummy_config_int_key, YAML::Node(1)},
                     {dummy_config_flt_key, YAML::Node(2.0)},
                     {dummy_config_bol_key, YAML::Node(false)}});
    out_config_node[dummy_config_str_key_2] = 10;
    out_config_node[dummy_config_str_key_3] = 20.0;
    out_config_node[dummy_config_str_key_4] = true;
}

std::string getBaseDAGConfigurationAsJsonStr() {
    YAML::Node config_node;
    getBaseDAGConfiguration(config_node);
    YAML::Emitter json_emitter;
    json_emitter << YAML::DoubleQuoted << YAML::Flow << YAML::BeginSeq
                 << config_node;
    std::string json_str(json_emitter.c_str() + 1);
    json_str.erase(std::remove_if(json_str.begin(),
                                  json_str.end(),
                                  [](unsigned char rune) {
                                      return std::isspace(rune);
                                  }),
                   json_str.end());
    return json_str;
}

TEST(TestYAMLDAGDeserializer, outputUptoEmpty) {
    std::string expected_empty_upto = "\n";
    std::string actual = YAMLDAGDeserializer::sampleDAGOutput(
            YAMLDAGDeserializer::UpTo::EMPTY);
    EXPECT_EQ(expected_empty_upto, actual);
}

TEST(TestYAMLDAGDeserializer, outputUptoDAG) {
    std::string expected_dag_upto =
            std::string("DAG:\n") +
            std::string("  Title: <optional string>\n") +
            std::string("  Configuration: <optional YAML>\n") +
            std::string("    <valid YAML>\n") + std::string("  ...\n");
    std::string actual = YAMLDAGDeserializer::sampleDAGOutput(
            YAMLDAGDeserializer::UpTo::DAG);
    EXPECT_EQ(expected_dag_upto, actual);
}

TEST(TestYAMLDAGDeserializer, outputUptoVertices) {
    std::string expected_vertices_upto =
            std::string("DAG:\n") +
            std::string("  Title: <optional string>\n") +
            std::string("  Configuration: <optional YAML>\n") +
            std::string("    <valid YAML>\n") + std::string("  Vertices:\n") +
            std::string("    ...\n");
    std::string actual = YAMLDAGDeserializer::sampleDAGOutput(
            YAMLDAGDeserializer::UpTo::VERTICES);
    EXPECT_EQ(expected_vertices_upto, actual);
}

TEST(TestYAMLDAGDeserializer, outputUptoVertex) {
    std::string expected_vertex_upto =
            std::string("DAG:\n") +
            std::string("  Title: <optional string>\n") +
            std::string("  Configuration: <optional YAML>\n") +
            std::string("    <valid YAML>\n") + std::string("  Vertices:\n") +
            std::string("    - Vertex:\n") +
            std::string("      Name: <optional string>\n") +
            std::string("      UUID: <valid uuid4 string>\n") +
            std::string("        ...\n");
    std::string actual = YAMLDAGDeserializer::sampleDAGOutput(
            YAMLDAGDeserializer::UpTo::VERTEX);
    EXPECT_EQ(expected_vertex_upto, actual);
}

TEST(TestYAMLDAGDeserializer, outputUptoTask) {
    std::string expected_task_upto =
            std::string("DAG:\n") +
            std::string("  Title: <optional string>\n") +
            std::string("  Configuration: <optional YAML>\n") +
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
    std::string actual = YAMLDAGDeserializer::sampleDAGOutput(
            YAMLDAGDeserializer::UpTo::TASK);
    EXPECT_EQ(expected_task_upto, actual);
}

TEST(TestYAMLDAGDeserializer, outputUptoStages) {
    std::string expected_stages_upto =
            std::string("DAG:\n") +
            std::string("  Title: <optional string>\n") +
            std::string("  Configuration: <optional YAML>\n") +
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
    std::string actual = YAMLDAGDeserializer::sampleDAGOutput(
            YAMLDAGDeserializer::UpTo::STAGES);
    EXPECT_EQ(expected_stages_upto, actual);
}

TEST(TestYAMLDAGDeserializer, outputUptoStage) {
    std::string expected_stage_upto =
            std::string("DAG:\n") +
            std::string("  Title: <optional string>\n") +
            std::string("  Configuration: <optional YAML>\n") +
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
    std::string actual = YAMLDAGDeserializer::sampleDAGOutput(
            YAMLDAGDeserializer::UpTo::STAGE);
    EXPECT_EQ(expected_stage_upto, actual);
}

TEST(TestYAMLDAGDeserializer, outputUptoJustConnections) {
    std::string expected_connections_upto =
            std::string("  Connections:\n") +
            std::string("    - Connection:\n") +
            std::string("      From: <valid uuid4 string from vertex>\n") +
            std::string("      To: <valid uuid4 string from vertex>\n") +
            std::string("    ...\n");
    std::string actual = YAMLDAGDeserializer::sampleDAGOutput(
            YAMLDAGDeserializer::UpTo::CONNECTIONS);
    EXPECT_EQ(expected_connections_upto, actual);
}

TEST(TestYAMLDAGDeserializer, outputFull) {
    std::string expected_full =
            std::string("DAG:\n") +
            std::string("  Title: <optional string>\n") +
            std::string("  Configuration: <optional YAML>\n") +
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
            std::string("  Connections:\n") +
            std::string("    - Connection:\n") +
            std::string("      From: <valid uuid4 string from vertex>\n") +
            std::string("      To: <valid uuid4 string from vertex>\n") +
            std::string("    ...\n");
    std::string actual = YAMLDAGDeserializer::fullSampleOutput();
    EXPECT_EQ(expected_full, actual);
}

TEST(TestYAMLDAGDeserializer, defaultCtor) {
    EXPECT_NO_THROW(YAMLDAGDeserializer());
}

TEST(TestYAMLDAGDeserializer, makeDAGWrongRootElement) {
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, std::string>(
            {{YAMLDAGDeserializer::title_key, "Test YAML DAG"}});
    try {
        yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    } catch (const YAMLDAGDeserializerError& yaml_error) {
        auto expected = std::string("Root element of \"DAG\" not found in\n") +
                        std::string("DAG:\n  Title: Test YAML DAG\n") +
                        std::string("For example, as in\n") +
                        YAMLDAGDeserializer::sampleDAGOutput(
                                YAMLDAGDeserializer::UpTo::DAG);
        auto actual = std::string(yaml_error.what());
        EXPECT_EQ(expected, actual);
    }
}

TEST(TestYAMLDAGDeserializer, makeDAGTitle) {
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, std::string>(
            {{YAMLDAGDeserializer::title_key, "Test YAML DAG"}});
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(0ull, test_dag->vertexCount());
}

TEST(TestYAMLDAGDeserializer, makeDAGTitleDAGConfig) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(0ull, test_dag->vertexCount());
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());
}

TEST(TestYAMLDAGDeserializer, makeDAG) {
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, std::string>();
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("", test_dag->title());
    EXPECT_EQ(0ull, test_dag->vertexCount());
}

TEST(TestYAMLDAGDeserializer, makeDAGWithDAGConfig) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("", test_dag->title());
    EXPECT_EQ(0ull, test_dag->vertexCount());
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());
}

TEST(TestYAMLDAGDeserializer, makeDAGBadVerticesNotListOfObj) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>(
                    {{std::string(YAMLDAGDeserializer::title_key),
                      YAML::Node("Test YAML DAG")}});
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] =
                     "This is bad input";
    try {
        yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    } catch (const YAMLDAGDeserializerError& yaml_error) {
        auto expected = std::string("\"Vertices\" must be a ") +
                        std::string("YAML Sequence. As in:\n") +
                        YAMLDAGDeserializer::sampleDAGOutput(
                                YAMLDAGDeserializer::UpTo::VERTEX);
        auto actual = std::string(yaml_error.what());
        EXPECT_EQ(expected, actual);
    }
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelConfig) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] =
                     std::vector<YAML::Node>{};
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(0ull, test_dag->vertexCount());
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());
}

TEST(TestYAMLDAGDeserializer, makeDAGVerticesTitle) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    YAML::Node second_vertex;
    second_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_2;
    std::vector<YAML::Node> vertices = {first_vertex, second_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(2ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    EXPECT_TRUE(test_dag->findVertexByUUID(std::move(uuid_1)).lock() !=
                nullptr);
    UUID uuid_2(test_uuid_1);
    EXPECT_TRUE(test_dag->findVertexByUUID(std::move(uuid_2)).lock() !=
                nullptr);
}

TEST(TestYAMLDAGDeserializer, makeDAGTitleDAGConfigVertices) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    YAML::Node second_vertex;
    second_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_2;
    std::vector<YAML::Node> vertices = {first_vertex, second_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(2ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    EXPECT_TRUE(test_dag->findVertexByUUID(std::move(uuid_1)).lock() !=
                nullptr);
    UUID uuid_2(test_uuid_1);
    EXPECT_TRUE(test_dag->findVertexByUUID(std::move(uuid_2)).lock() !=
                nullptr);
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());
}

TEST(TestYAMLDAGDeserializer, makeDAGBadVerticesNoVertexUUID) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::name_key)] =
            "A Vertex without a uuid.";
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    try {
        yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
        ASSERT_TRUE(false);
    } catch (const std::runtime_error& exception) {
        std::string expected_what = std::string("invalid node; first ") +
                                    std::string("invalid key: \"UUID\"");
        EXPECT_EQ(expected_what, std::string(exception.what()));
    }
}

TEST(TestYAMLDAGDeserializer, makeDAGVerticesNamedTask) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ(uuid1_task_label, uuid1_vertex->getTask()->label());
}

TEST(TestYAMLDAGDeserializer, makeDAGVerticesNamedTaskTaskConfig) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ(uuid1_task_label, uuid1_vertex->getTask()->label());

    std::string uuid1_task_config_str;
    uuid1_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDAGDeserializer, makeDAGVerticesNamedTaskInitialInputs) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ(uuid1_task_label, uuid1_vertex->getTask()->label());

    std::string uuid1_task_initial_inputs_str;
    uuid1_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer,
     makeDAGVerticesNamedTaskInitialInputsTaskConfig) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ(uuid1_task_label, uuid1_vertex->getTask()->label());

    std::string uuid1_task_initial_inputs_str;
    uuid1_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);

    std::string uuid1_task_config_str;
    uuid1_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelDAGConfigVerticesNamedTask) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ(uuid1_task_label, uuid1_vertex->getTask()->label());
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelDAGConfigVerticesNamedTaskTaskConfig) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ(uuid1_task_label, uuid1_vertex->getTask()->label());
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_config_str;
    uuid1_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelDAGConfigVerticesNamedTaskInitialInputs) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ(uuid1_task_label, uuid1_vertex->getTask()->label());
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_initial_inputs_str;
    uuid1_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelDAGConfigVerticesNamedTaskInitialInputsTaskConfig) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ(uuid1_task_label, uuid1_vertex->getTask()->label());
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_config_str;
    uuid1_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

    std::string uuid1_task_initial_inputs_str;
    uuid1_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelVerticesTask) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ("", uuid1_vertex->getTask()->label());
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelVerticesTaskTaskConfig) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ("", uuid1_vertex->getTask()->label());

    std::string uuid1_task_config_str;
    uuid1_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelVerticesTaskInitialInputs) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ("", uuid1_vertex->getTask()->label());

    std::string uuid1_task_initial_inputs_str;
    uuid1_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelVerticesTaskInitialInputsTaskConfig) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ("", uuid1_vertex->getTask()->label());

    std::string uuid1_task_config_str;
    uuid1_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

    std::string uuid1_task_initial_inputs_str;
    uuid1_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelDAGConfigVerticesTask) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ("", uuid1_vertex->getTask()->label());
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelDAGConfigVerticesTaskTaskConfig) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ("", uuid1_vertex->getTask()->label());
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_config_str;
    uuid1_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelDAGConfigVerticesTaskInitialInputs) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ("", uuid1_vertex->getTask()->label());
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_initial_inputs_str;
    uuid1_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelDAGConfigVerticesTaskInitialInputsTaskConfig) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_NE(nullptr, test_dag);
    EXPECT_EQ("Test YAML DAG", test_dag->title());
    EXPECT_EQ(1ull, test_dag->vertexCount());
    UUID uuid_1(test_uuid_1);
    std::shared_ptr<DAGVertex> uuid1_vertex =
            test_dag->findVertexByUUID(std::move(uuid_1)).lock();
    EXPECT_TRUE(uuid1_vertex != nullptr);
    EXPECT_EQ("", uuid1_vertex->getTask()->label());
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_config_str;
    uuid1_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

    std::string uuid1_task_initial_inputs_str;
    uuid1_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer, makeDAGTitleVerticesBadTaskCallback) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    try {
        yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
        ASSERT_FALSE(true);
    } catch (const YAML::InvalidNode& exception) {
        EXPECT_EQ(std::string(
                          "invalid node; first invalid key: \"LibraryName\""),
                  std::string(exception.what()));
    }
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelVerticesBadTaskCallbackLibNotFound) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node("Library DNE");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("Method DNE");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    try {
        yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
        ASSERT_FALSE(true);
    } catch (const YAMLDAGDeserializerError& yaml_deserialization_error) {
        auto expected_error_substr =
                std::string("Could not load library from ");
        auto actual_error_str = std::string(yaml_deserialization_error.what());
        EXPECT_TRUE(actual_error_str.find(expected_error_substr) !=
                    std::string::npos);
    }
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelVerticesBadTaskCallbackLibFoundNoMethod) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Function");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("Method DNE");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    try {
        yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
        ASSERT_FALSE(true);
    } catch (const YAMLDAGDeserializerError& yaml_deserialization_error) {
        auto expected_error_substr =
                std::string("Failed to load Method DNE from");
        auto actual_error_str = std::string(yaml_deserialization_error.what());
        EXPECT_TRUE(actual_error_str.find(expected_error_substr) !=
                    std::string::npos);
    }
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelVerticesTaskCallback) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Function");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackIsSet());
    test_vertex->getTask()->complete(true);
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelVerticesTaskCallbackTaskConfig) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Function");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackIsSet());
    test_vertex->getTask()->complete(true);

    std::string uuid1_task_config_str;
    test_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelVerticesTaskCallbackInitialInputs) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Function");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackIsSet());
    test_vertex->getTask()->complete(true);

    std::string uuid1_task_initial_inputs_str;
    test_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelVerticesTaskCallbackInitialInputsTaskConfig) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Function");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackIsSet());
    test_vertex->getTask()->complete(true);

    std::string uuid1_task_config_str;
    test_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

    std::string uuid1_task_initial_inputs_str;
    test_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelDAGConfigVerticesTaskCallback) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Function");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackIsSet());
    test_vertex->getTask()->complete(true);
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelDAGConfigVerticesTaskCallbackTaskConfig) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Function");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackIsSet());
    test_vertex->getTask()->complete(true);
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_config_str;
    test_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelDAGConfigVerticesTaskCallbackInitialInputs) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Function");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackIsSet());
    test_vertex->getTask()->complete(true);
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_initial_inputs_str;
    test_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer,
     labelDAGConfigVerticesTaskCallbackInitialInputsTaskConfig) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Function");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackIsSet());
    test_vertex->getTask()->complete(true);
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_config_str;
    test_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

    std::string uuid1_task_initial_inputs_str;
    test_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelVerticesTaskCallbackPlugin) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackPluginIsSet());
    test_vertex->getTask()->complete(true);
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelVerticesTaskCallbackPluginTaskConfig) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackPluginIsSet());
    test_vertex->getTask()->complete(true);

    std::string uuid1_task_config_str;
    test_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelVerticesTaskCallbackPluginInitialInputs) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackPluginIsSet());
    test_vertex->getTask()->complete(true);

    std::string uuid1_task_initial_inputs_str;
    test_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelVerticesTaskCallbackPluginInitialInputsTaskConfig) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackPluginIsSet());
    test_vertex->getTask()->complete(true);

    std::string uuid1_task_config_str;
    test_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

    std::string uuid1_task_initial_inputs_str;
    test_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelDAGConfigVerticesTaskCallbackPlugin) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackPluginIsSet());
    test_vertex->getTask()->complete(true);
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelDAGConfigVerticesTaskCallbackPluginTaskConfig) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackPluginIsSet());
    test_vertex->getTask()->complete(true);
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_config_str;
    test_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelDAGConfigVerticesTaskCallbackPluginInitialInputs) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackPluginIsSet());
    test_vertex->getTask()->complete(true);
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_initial_inputs_str;
    test_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer,
     daGConfigVerticesTaskCallbackPluginInitialInputsTaskConfig) {
    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    EXPECT_TRUE(test_vertex->getTask()->callbackPluginIsSet());
    test_vertex->getTask()->complete(true);
    std::string expected_config_json_str;
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_config_str;
    test_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

    std::string uuid1_task_initial_inputs_str;
    test_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer,
     make_DAG_label_vertices_bad_task_callback_plugin_symbol_dne) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin_dne");
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    try {
        yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
        ASSERT_TRUE(false);
    } catch (const YAMLDAGDeserializerError& yaml_deserialization_error) {
        auto expected_error_substr =
                std::string("Failed to load task_callback_plugin_dne from");
        auto actual_error_str = std::string(yaml_deserialization_error.what());
        EXPECT_TRUE(actual_error_str.find(expected_error_substr) !=
                    std::string::npos);
    }
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelVerticesBadTaskStagesNodeWrongType) {
    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::stages_key)] = YAML::Node();
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;

    try {
        yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
        ASSERT_TRUE(false);
    } catch (const std::runtime_error& error) {
        auto expected_error_str = std::string("bad conversion");
        auto actual_error_str = std::string(error.what());
        std::cout << error.what() << std::endl;
        EXPECT_EQ(expected_error_str, actual_error_str);
    }
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelVerticesTaskStages) {
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    std::string stages_lib_path =
            testing::TestEnvironment::pathing.getStagesLibPath();

    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    YAML::Node stage_node;
    stage_node[std::string(YAMLDAGDeserializer::name_key)] =
            YAML::Node("PrintStage");
    stage_node[std::string(YAMLDAGDeserializer::library_name_key)] =
            YAML::Node(stages_lib_path);
    stage_node[std::string(YAMLDAGDeserializer::symbol_name_key)] =
            "print_stage";
    std::vector<YAML::Node> stage_nodes = {stage_node};
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::stages_key)] = stage_nodes;
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    std::size_t stage_count = 0;
    test_vertex->getTask()->iterateStages([&](TaskStage& stage) -> bool {
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

TEST(TestYAMLDAGDeserializer, makeDAGLabelVerticesTaskStagesTaskConfig) {
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    std::string stages_lib_path =
            testing::TestEnvironment::pathing.getStagesLibPath();

    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    YAML::Node stage_node;
    stage_node[std::string(YAMLDAGDeserializer::name_key)] =
            YAML::Node("PrintStage");
    stage_node[std::string(YAMLDAGDeserializer::library_name_key)] =
            YAML::Node(stages_lib_path);
    stage_node[std::string(YAMLDAGDeserializer::symbol_name_key)] =
            "print_stage";
    std::vector<YAML::Node> stage_nodes = {stage_node};
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::stages_key)] = stage_nodes;
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    std::size_t stage_count = 0;
    test_vertex->getTask()->iterateStages([&](TaskStage& stage) -> bool {
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
    test_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelVerticesTaskStagesInitialInputs) {
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    std::string stages_lib_path =
            testing::TestEnvironment::pathing.getStagesLibPath();

    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    YAML::Node stage_node;
    stage_node[std::string(YAMLDAGDeserializer::name_key)] =
            YAML::Node("PrintStage");
    stage_node[std::string(YAMLDAGDeserializer::library_name_key)] =
            YAML::Node(stages_lib_path);
    stage_node[std::string(YAMLDAGDeserializer::symbol_name_key)] =
            "print_stage";
    std::vector<YAML::Node> stage_nodes = {stage_node};
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::stages_key)] = stage_nodes;
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    std::size_t stage_count = 0;
    test_vertex->getTask()->iterateStages([&](TaskStage& stage) -> bool {
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
    test_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelVerticesTaskStagesInitialInputsTaskConfig) {
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    std::string stages_lib_path =
            testing::TestEnvironment::pathing.getStagesLibPath();

    YAML::Node yaml_node;
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)] =
            std::map<std::string, YAML::Node>({{YAMLDAGDeserializer::title_key,
                                                YAML::Node("Test YAML DAG")}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    YAML::Node stage_node;
    stage_node[std::string(YAMLDAGDeserializer::name_key)] =
            YAML::Node("PrintStage");
    stage_node[std::string(YAMLDAGDeserializer::library_name_key)] =
            YAML::Node(stages_lib_path);
    stage_node[std::string(YAMLDAGDeserializer::symbol_name_key)] =
            "print_stage";
    std::vector<YAML::Node> stage_nodes = {stage_node};
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::stages_key)] = stage_nodes;
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    std::size_t stage_count = 0;
    test_vertex->getTask()->iterateStages([&](TaskStage& stage) -> bool {
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
    test_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

    std::string uuid1_task_initial_inputs_str;
    test_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer, makeDAGLabelDAGConfigVerticesTaskStages) {
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    std::string stages_lib_path =
            testing::TestEnvironment::pathing.getStagesLibPath();

    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    YAML::Node stage_node;
    stage_node[std::string(YAMLDAGDeserializer::name_key)] =
            YAML::Node("PrintStage");
    stage_node[std::string(YAMLDAGDeserializer::library_name_key)] =
            YAML::Node(stages_lib_path);
    stage_node[std::string(YAMLDAGDeserializer::symbol_name_key)] =
            "print_stage";
    std::vector<YAML::Node> stage_nodes = {stage_node};
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::stages_key)] = stage_nodes;
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    std::size_t stage_count = 0;
    test_vertex->getTask()->iterateStages([&](TaskStage& stage) -> bool {
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
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelDAGConfigVerticesTaskStagesTaskConfig) {
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    std::string stages_lib_path =
            testing::TestEnvironment::pathing.getStagesLibPath();

    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    YAML::Node stage_node;
    stage_node[std::string(YAMLDAGDeserializer::name_key)] =
            YAML::Node("PrintStage");
    stage_node[std::string(YAMLDAGDeserializer::library_name_key)] =
            YAML::Node(stages_lib_path);
    stage_node[std::string(YAMLDAGDeserializer::symbol_name_key)] =
            "print_stage";
    std::vector<YAML::Node> stage_nodes = {stage_node};
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::stages_key)] = stage_nodes;
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    std::size_t stage_count = 0;
    test_vertex->getTask()->iterateStages([&](TaskStage& stage) -> bool {
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
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_config_str;
    test_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);
}

TEST(TestYAMLDAGDeserializer,
     makeDAGLabelDAGConfigVerticesTaskStagesInitialInputs) {
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    std::string stages_lib_path =
            testing::TestEnvironment::pathing.getStagesLibPath();

    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    YAML::Node stage_node;
    stage_node[std::string(YAMLDAGDeserializer::name_key)] =
            YAML::Node("PrintStage");
    stage_node[std::string(YAMLDAGDeserializer::library_name_key)] =
            YAML::Node(stages_lib_path);
    stage_node[std::string(YAMLDAGDeserializer::symbol_name_key)] =
            "print_stage";
    std::vector<YAML::Node> stage_nodes = {stage_node};
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::stages_key)] = stage_nodes;
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    std::size_t stage_count = 0;
    test_vertex->getTask()->iterateStages([&](TaskStage& stage) -> bool {
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
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_initial_inputs_str;
    test_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);
}

TEST(TestYAMLDAGDeserializer,
     labelDAGConfigVerticesTaskStagesInitialInputsTaskConfig) {
    std::string lib_path = testing::TestEnvironment::pathing.getLibPath();
    std::string stages_lib_path =
            testing::TestEnvironment::pathing.getStagesLibPath();

    YAML::Node dag_configuration_node;
    getBaseDAGConfiguration(dag_configuration_node);
    YAML::Node yaml_node;
    yaml_node["DAG"] = std::map<std::string, YAML::Node>(
            {{YAMLDAGDeserializer::title_key, YAML::Node("Test YAML DAG")},
             {YAMLDAGDeserializer::configuration_key,
              dag_configuration_node}});
    YAML::Node first_vertex;
    first_vertex[std::string(YAMLDAGDeserializer::uuid_key)] = test_uuid_1;
    first_vertex[std::string(YAMLDAGDeserializer::task_key)] = YAML::Node();
    const std::string uuid1_task_label = "A named task";
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::name_key)] =
                        YAML::Node(uuid1_task_label);

    YAML::Node task_config;
    getBaseTaskConfiguration(task_config);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::configuration_key)] =
                        task_config;

    YAML::Node task_initial_inputs;
    getBaseTaskInitialInputs(task_initial_inputs);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::initial_inputs_key)] =
                        task_initial_inputs;

    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)] =
                        YAML::Node();
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::library_name_key)] =
                        YAML::Node(lib_path);
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::callback_type_key)] =
                        YAML::Node("Plugin");
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::callback_key)]
                [std::string(YAMLDAGDeserializer::symbol_name_key)] =
                        YAML::Node("task_callback_plugin");
    YAML::Node stage_node;
    stage_node[std::string(YAMLDAGDeserializer::name_key)] =
            YAML::Node("PrintStage");
    stage_node[std::string(YAMLDAGDeserializer::library_name_key)] =
            YAML::Node(stages_lib_path);
    stage_node[std::string(YAMLDAGDeserializer::symbol_name_key)] =
            "print_stage";
    std::vector<YAML::Node> stage_nodes = {stage_node};
    first_vertex[std::string(YAMLDAGDeserializer::task_key)]
                [std::string(YAMLDAGDeserializer::stages_key)] = stage_nodes;
    std::vector<YAML::Node> vertices = {first_vertex};
    yaml_node[std::string(YAMLDAGDeserializer::dag_key)]
             [std::string(YAMLDAGDeserializer::vertices_key)] = vertices;
    auto test_dag = yaml_node.as<std::unique_ptr<com::dag_scheduler::DAG>>();
    ASSERT_TRUE(test_dag != nullptr);
    std::shared_ptr<DAGVertex> test_vertex =
            test_dag->findVertexByUUID(UUID(test_uuid_1)).lock();
    EXPECT_TRUE(test_vertex != nullptr);
    EXPECT_TRUE(test_vertex->getTask() != nullptr);
    std::size_t stage_count = 0;
    test_vertex->getTask()->iterateStages([&](TaskStage& stage) -> bool {
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
    test_dag->jsonConfigStr(expected_config_json_str);
    EXPECT_EQ(expected_config_json_str, getBaseDAGConfigurationAsJsonStr());

    std::string uuid1_task_config_str;
    test_vertex->getTask()->jsonConfigStr(uuid1_task_config_str);
    std::string expected_task_config_str = getBaseTaskConfigurationAsJsonStr();
    EXPECT_EQ(expected_task_config_str, uuid1_task_config_str);

    std::string uuid1_task_initial_inputs_str;
    test_vertex->getTask()->jsonInitialInputsStr(
            uuid1_task_initial_inputs_str);
    std::string expected_task_initial_inputs_str =
            getBaseTaskInitialInputsAsJsonStr();
    EXPECT_EQ(expected_task_initial_inputs_str, uuid1_task_initial_inputs_str);

    std::cout << yaml_node << "Dude" << std::endl;
}
}  // namespace com::dag_scheduler
