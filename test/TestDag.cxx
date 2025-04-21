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
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "dag_scheduler/Dag.h"
#include "dag_scheduler/LoggedClass.hpp"
#include "utils/TestTask.h"

namespace com::dag_scheduler {
class TestDag : public ::testing::Test, public LoggedClass<TestDag> {
public:
    TestDag() : LoggedClass<TestDag>(*this) {}

protected:
    void SetUp() override {}

    void TearDown() override {}

    DAG& getDAG() { return d_; }

    std::vector<UUID> fillDAGDefaultWithTasks() {
        std::vector<DAGVertex> vertices_to_add;
        auto v0_task = std::make_unique<TestTaskImpl>("V0 Task");
        DAGVertex vertex_v0("1", std::move(v0_task));
        vertices_to_add.push_back(std::move(vertex_v0));
        auto v1_task = std::make_unique<TestTaskImpl>("V1 Task");
        DAGVertex vertex_v1("1", std::move(v1_task));
        vertices_to_add.push_back(std::move(vertex_v1));
        auto v2_task = std::make_unique<TestTaskImpl>("V2 Task");
        DAGVertex vertex_v2("1a", std::move(v2_task));
        vertices_to_add.push_back(std::move(vertex_v2));
        auto v3_task = std::make_unique<TestTaskImpl>("V3 Task");
        DAGVertex vertex_v3("1b", std::move(v3_task));
        vertices_to_add.push_back(std::move(vertex_v3));
        auto v4_task = std::make_unique<TestTaskImpl>("V4 Task");
        DAGVertex vertex_v4("2", std::move(v4_task));
        vertices_to_add.push_back(std::move(vertex_v4));
        auto v5_task = std::make_unique<TestTaskImpl>("V5 Task");
        DAGVertex vertex_v5("2", std::move(v5_task));
        vertices_to_add.push_back(std::move(vertex_v5));
        auto v6_task = std::make_unique<TestTaskImpl>("V6 Task");
        DAGVertex vertex_v6("2", std::move(v6_task));
        vertices_to_add.push_back(std::move(vertex_v6));
        auto v7_task = std::make_unique<TestTaskImpl>("V7 Task");
        DAGVertex vertex_v7("3", std::move(v7_task));
        vertices_to_add.push_back(std::move(vertex_v7));
        auto v8_task = std::make_unique<TestTaskImpl>("V8 Task");
        DAGVertex vertex_v8("3", std::move(v8_task));
        vertices_to_add.push_back(std::move(vertex_v8));
        auto v9_task = std::make_unique<TestTaskImpl>("V9 Task");
        DAGVertex vertex_v9("4", std::move(v9_task));
        vertices_to_add.push_back(std::move(vertex_v9));
        auto v10_task = std::make_unique<TestTaskImpl>("V10 Task");
        DAGVertex vertex_v10("5", std::move(v10_task));
        vertices_to_add.push_back(std::move(vertex_v10));

        std::vector<UUID> vertices_uuids;
        vertices_uuids.reserve(vertices_to_add.size());
        std::for_each(vertices_to_add.begin(),
                      vertices_to_add.end(),
                      [&](DAGVertex& vertex) {
                          UUID uuid_cloned(vertex.getUUID().asString());
                          getDAG().addVertex(std::move(vertex));
                          rapidjson::Document initial_input;
                          get_generic_input(initial_input);
                          // TODO (mehoggan): Uncomment once
                          // override_initial_input_for_vertex_task is
                          // completed.
                          //
                          // getDAG().override_initial_input_for_vertex_task(
                          //   uuid_cloned, initial_input);
                          // std::weak_ptr<DAGVertex> v_weak =
                          //   getDAG().find_vertex_by_uuid(uuid_cloned);
                          // EXPECT_FALSE(v_weak.expired());
                          // ASSERT_TRUE(v_weak.lock()->get_task() !=
                          // nullptr);
                          vertices_uuids.push_back(std::move(uuid_cloned));
                      });
        getDAG().linearTraversal([&](std::shared_ptr<DAGVertex> vertex) {
            ASSERT_NE(nullptr, vertex->getTask());
        });
        EXPECT_EQ(vertices_to_add.size(), getDAG().vertexCount());
        vertices_to_add.clear();

        return vertices_uuids;
    }

    std::vector<DAGVertex> fillDAGDefault() {
        DAGVertex vertex_0("1"), vertex_1("1"), vertex_2("1a"), vertex_3("1b"),
                vertex_4("2"), vertex_5("2"), vertex_6("2"), vertex_7("3"),
                vertex_8("3"), vertex_9("4"), vertex_10("5");
        std::vector<DAGVertex> vertices_to_add;
        vertices_to_add.push_back(std::move(vertex_0));
        vertices_to_add.push_back(std::move(vertex_1));
        vertices_to_add.push_back(std::move(vertex_2));
        vertices_to_add.push_back(std::move(vertex_3));
        vertices_to_add.push_back(std::move(vertex_4));
        vertices_to_add.push_back(std::move(vertex_5));
        vertices_to_add.push_back(std::move(vertex_6));
        vertices_to_add.push_back(std::move(vertex_7));
        vertices_to_add.push_back(std::move(vertex_8));
        vertices_to_add.push_back(std::move(vertex_9));
        vertices_to_add.push_back(std::move(vertex_10));

        std::vector<DAGVertex> vertices_cloned;
        vertices_cloned.reserve(vertices_to_add.size());
        std::for_each(
                vertices_to_add.begin(),
                vertices_to_add.end(),
                [&](DAGVertex& vertex) {
                    DAGVertex v_clone_1 = vertex.clone();
                    DAGVertex v_clone_2 = vertex.clone();
                    DAGVertex v_clone_3 = vertex.clone();

                    vertices_cloned.push_back(std::move(v_clone_1));
                    getDAG().addVertex(std::move(v_clone_2));

                    std::weak_ptr<DAGVertex> v_weak =
                            getDAG().findVertexByUUID(v_clone_3.getUUID());
                    EXPECT_FALSE(v_weak.expired());
                });
        EXPECT_EQ(vertices_to_add.size(), getDAG().vertexCount());
        vertices_to_add.clear();

        return vertices_cloned;
    }

    static void getGenericConfig(rapidjson::Document& config_doc) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        writer.StartObject();
        writer.String("test_value_int");
        writer.Int(-1);
        writer.String("test_value_str");
        writer.String("test_string");
        writer.String("test_value_bool");
        writer.Bool(true);
        writer.String("test_value_double");
        writer.Double(-1.0);
        writer.EndObject();
        config_doc.Parse(buffer.GetString());
    }

    static std::string getExpectedConfigStr() {
        return std::string("{") + std::string("\"test_value_int\":-1,") +
               std::string("\"test_value_str\":\"test_string\",") +
               std::string("\"test_value_bool\":true,") +
               std::string("\"test_value_double\":-1.0") + std::string("}");
    }

    static void getGenericInput(rapidjson::Document& input_doc) {
        getGenericConfig(input_doc);
    }

    static std::string getExpectedInputStr() { return getExpectedConfigStr(); }

private:
    DAG d_;
};

TEST_F(TestDag, ctor) {
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
    std::string actual_config;
    getDAG().jsonConfigStr(actual_config);
    EXPECT_EQ("{}", actual_config);
}

TEST_F(TestDag, ctor_with_title) {
    DAG test_dag("test_dag");

    EXPECT_EQ(0ul, test_dag.edgeCount());
    EXPECT_EQ(0ul, test_dag.vertexCount());
    EXPECT_EQ(std::string("test_dag"), test_dag.title());
    std::string actual_config;
    getDAG().jsonConfigStr(actual_config);
    EXPECT_EQ("{}", actual_config);
}

TEST_F(TestDag, ctor_with_title_and_config) {
    rapidjson::Document json_config;
    TestDag::getGenericConfig(json_config);
    DAG test_dag("test_dag", json_config);

    EXPECT_EQ(0ul, test_dag.edgeCount());
    EXPECT_EQ(0ul, test_dag.vertexCount());
    EXPECT_EQ(std::string("test_dag"), test_dag.title());
    std::string actual_config;
    test_dag.jsonConfigStr(actual_config);
    EXPECT_EQ(TestDag::getExpectedConfigStr(), actual_config);
}

TEST_F(TestDag, move_ctor_no_edges_no_vertices) {
    DAG test_dag;

    DAG d_moved(std::move(test_dag));
    EXPECT_EQ(0ul, d_moved.edgeCount());
    EXPECT_EQ(0ul, d_moved.vertexCount());
    std::string actual_config;
    getDAG().jsonConfigStr(actual_config);
    EXPECT_EQ("{}", actual_config);
}

TEST_F(TestDag, move_ctor_no_edges_no_vertices_title_and_config) {
    rapidjson::Document json_config;
    TestDag::getGenericConfig(json_config);
    DAG test_dag("test_dag", json_config);

    DAG d_moved(std::move(test_dag));
    EXPECT_EQ(0ul, d_moved.edgeCount());
    EXPECT_EQ(0ul, d_moved.vertexCount());
    std::string actual_moved_config;
    d_moved.jsonConfigStr(actual_moved_config);
    EXPECT_EQ(TestDag::getExpectedConfigStr(), actual_moved_config);

    std::string actual_config;
    test_dag.jsonConfigStr(actual_config);  // NOLINT
    EXPECT_EQ("", actual_config) << "Empty because we moved it.";
}

TEST_F(TestDag, assignment_move_operator_no_edges_no_vertices) {
    DAG d;

    DAG d_moved;
    d_moved = std::move(d);
    EXPECT_EQ(0ul, d_moved.edgeCount());
    EXPECT_EQ(0ul, d_moved.vertexCount());
    std::string actual_config;
    getDAG().jsonConfigStr(actual_config);
    EXPECT_EQ("{}", actual_config);
}

TEST_F(TestDag,
       assignment_move_operator_no_edges_no_vertices_title_and_config) {
    rapidjson::Document json_config;
    TestDag::getGenericConfig(json_config);
    DAG d("test_dag", json_config);

    DAG d_moved;
    d_moved = std::move(d);
    EXPECT_EQ(0ul, d_moved.edgeCount());
    EXPECT_EQ(0ul, d_moved.vertexCount());
    std::string actual_moved_config;
    d_moved.jsonConfigStr(actual_moved_config);
    EXPECT_EQ(TestDag::getExpectedConfigStr(), actual_moved_config);

    std::string actual_config;
    d.jsonConfigStr(actual_config);
    EXPECT_EQ("", actual_config) << "Empty because we moved it.";
}

TEST_F(TestDag, clone_no_edges_no_vertices) {
    DAG d;

    DAG d_cloned = d.clone();
    EXPECT_EQ(0ul, d_cloned.edgeCount());
    EXPECT_EQ(0ul, d_cloned.vertexCount());
    std::string actual_config;
    d.jsonConfigStr(actual_config);
    EXPECT_EQ("{}", actual_config);
    d_cloned.jsonConfigStr(actual_config);
    EXPECT_EQ("{}", actual_config);
}

TEST_F(TestDag, clone_no_edges_no_vertices_title_and_config) {
    rapidjson::Document json_config;
    TestDag::getGenericConfig(json_config);
    DAG d("test_dag", json_config);

    DAG d_cloned = d.clone();
    EXPECT_EQ(0ul, d_cloned.edgeCount());
    EXPECT_EQ(0ul, d_cloned.vertexCount());

    std::string actual_config;
    d.jsonConfigStr(actual_config);
    EXPECT_EQ(TestDag::getExpectedConfigStr(), actual_config);

    std::string actual_cloned_config;
    d.jsonConfigStr(actual_cloned_config);
    EXPECT_EQ(TestDag::getExpectedConfigStr(), actual_cloned_config);
}

TEST_F(TestDag, add_vertex_and_reset) {
    DAGVertex v1("1");
    getDAG().add_vertex(std::move(v1));
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(1ul, getDAG().vertexCount());

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, add_single_vertex_and_find_vertex_by_vertex_and_uuid) {
    DAGVertex v1("1");
    DAGVertex v1_cloned = v1.clone();
    getDAG().add_vertex(std::move(v1));

    std::weak_ptr<DAGVertex> v1_weak = getDAG().find_vertex(v1_cloned);
    EXPECT_EQ(v1_cloned, *(v1_weak.lock()));

    std::weak_ptr<DAGVertex> v2_weak =
            getDAG().find_vertex_by_uuid(v1_cloned.get_uuid());
    EXPECT_EQ(v1_cloned, *(v2_weak.lock()));
    EXPECT_TRUE(getDAG().contains_vertex(v1_cloned));
    EXPECT_TRUE(getDAG().contains_vertex_by_uuid(v1_cloned.get_uuid()));
    EXPECT_TRUE(getDAG().contains_vertex_by_label(v1_cloned.label()));

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
    EXPECT_TRUE(v1_weak.expired());
    EXPECT_TRUE(v2_weak.expired());
}

TEST_F(TestDag, add_single_vertex_and_find_vertex_by_label) {
    DAGVertex v1("1");
    DAGVertex v1_cloned = v1.clone();
    getDAG().add_vertex(std::move(v1));

    std::vector<std::weak_ptr<DAGVertex>> vertices =
            getDAG().find_all_vertices_with_label(v1_cloned.label());
    EXPECT_EQ(1ul, vertices.size());
    EXPECT_EQ(v1_cloned, *(vertices[0].lock()));

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
    EXPECT_TRUE(vertices[0].expired());
}

TEST_F(TestDag, add_vertices_with_same_and_different_labels) {
    std::vector<DAGVertex> vertices_cloned = fill_dag_default();

    {
        std::weak_ptr<DAGVertex> v_weak =
                getDAG().find_vertex(vertices_cloned[0]);
        ASSERT_FALSE(v_weak.expired());
        EXPECT_EQ(vertices_cloned[0], *(v_weak.lock()));
        EXPECT_NE(vertices_cloned[1], *(v_weak.lock()));

        std::shared_ptr<DAGVertex> v = v_weak.lock();
        EXPECT_TRUE(getDAG().contains_vertex(*v));
        EXPECT_TRUE(getDAG().contains_vertex_by_uuid(v->get_uuid()));
        EXPECT_TRUE(getDAG().contains_vertex_by_label(v->label()));
    }

    {
        std::weak_ptr<DAGVertex> v_weak =
                getDAG().find_vertex_by_uuid(vertices_cloned[1].get_uuid());
        ASSERT_FALSE(v_weak.expired());
        EXPECT_NE(vertices_cloned[0], *(v_weak.lock()));
        EXPECT_EQ(vertices_cloned[1], *(v_weak.lock()));

        std::shared_ptr<DAGVertex> v = v_weak.lock();
        EXPECT_TRUE(getDAG().contains_vertex(*v));
        EXPECT_TRUE(getDAG().contains_vertex_by_uuid(v->get_uuid()));
        EXPECT_TRUE(getDAG().contains_vertex_by_label(v->label()));
    }

    {
        std::weak_ptr<DAGVertex> v_weak =
                getDAG().find_vertex_by_uuid(vertices_cloned[6].get_uuid());
        ASSERT_FALSE(v_weak.expired());
        EXPECT_NE(vertices_cloned[4], *(v_weak.lock()));
        EXPECT_NE(vertices_cloned[5], *(v_weak.lock()));
        EXPECT_EQ(vertices_cloned[6], *(v_weak.lock()));

        std::shared_ptr<DAGVertex> v = v_weak.lock();
        EXPECT_TRUE(getDAG().contains_vertex(*v));
        EXPECT_TRUE(getDAG().contains_vertex_by_uuid(v->get_uuid()));
        EXPECT_TRUE(getDAG().contains_vertex_by_label(v->label()));
    }

    {
        std::vector<std::weak_ptr<DAGVertex>> v_weaks =
                getDAG().find_all_vertices_with_label("2");
        EXPECT_EQ(3ul, v_weaks.size());
        EXPECT_EQ(vertices_cloned[4], *(v_weaks[0].lock()));
        EXPECT_EQ(vertices_cloned[5], *(v_weaks[1].lock()));
        EXPECT_EQ(vertices_cloned[6], *(v_weaks[2].lock()));

        for (std::weak_ptr<DAGVertex> v_weak : v_weaks) {
            std::shared_ptr<DAGVertex> v = v_weak.lock();
            EXPECT_TRUE(getDAG().contains_vertex(*v));
            EXPECT_TRUE(getDAG().contains_vertex_by_uuid(v->get_uuid()));
            EXPECT_TRUE(getDAG().contains_vertex_by_label(v->label()));
        }
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, clone_with_multiple_vertices_no_connections) {
    std::vector<DAGVertex> vertices_cloned = fill_dag_default();
    DAG dag_cloned = getDAG().clone();

    for (const DAGVertex& v : vertices_cloned) {
        EXPECT_TRUE(getDAG().contains_vertex(v));
        EXPECT_TRUE(dag_cloned.contains_vertex(v));
    }
    EXPECT_EQ(getDAG(), dag_cloned);

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, connect_and_acyclic_check) {
    std::vector<DAGVertex> vertices_cloned = fill_dag_default();

    /* cloned_vertices[0] -> cloned_vertices[1] */
    EXPECT_FALSE(getDAG().connection_would_make_cyclic(vertices_cloned[0],
                                                       vertices_cloned[1]));
    EXPECT_FALSE(getDAG().connection_would_make_cyclic_by_uuid(
            vertices_cloned[0].get_uuid(), vertices_cloned[1].get_uuid()));
    EXPECT_TRUE(getDAG().connect(vertices_cloned[0], vertices_cloned[1]));
    EXPECT_EQ(1ul,
              getDAG().find_vertex(vertices_cloned[0]).lock()->edge_count());
    EXPECT_EQ(0ul,
              getDAG().find_vertex(vertices_cloned[0])
                      .lock()
                      ->incoming_edge_count());
    EXPECT_EQ(0ul,
              getDAG().find_vertex(vertices_cloned[1]).lock()->edge_count());
    EXPECT_EQ(1ul,
              getDAG().find_vertex(vertices_cloned[1])
                      .lock()
                      ->incoming_edge_count());

    /* cloned_vertices[1] -> cloned_vertices[2] */
    EXPECT_FALSE(getDAG().connection_would_make_cyclic(vertices_cloned[1],
                                                       vertices_cloned[2]));
    EXPECT_FALSE(getDAG().connection_would_make_cyclic_by_uuid(
            vertices_cloned[1].get_uuid(), vertices_cloned[2].get_uuid()));
    EXPECT_TRUE(getDAG().connect(vertices_cloned[1], vertices_cloned[2]));

    /* cloned_vertices[0] -> cloned_vertices[2] */
    EXPECT_FALSE(getDAG().connection_would_make_cyclic(vertices_cloned[0],
                                                       vertices_cloned[2]));
    EXPECT_FALSE(getDAG().connection_would_make_cyclic_by_uuid(
            vertices_cloned[0].get_uuid(), vertices_cloned[2].get_uuid()));
    EXPECT_TRUE(getDAG().connect(vertices_cloned[0], vertices_cloned[2]));

    /* cloned_vertices[2] -> cloned_vertices[0] */
    EXPECT_TRUE(getDAG().connection_would_make_cyclic(vertices_cloned[2],
                                                      vertices_cloned[0]));
    EXPECT_TRUE(getDAG().connection_would_make_cyclic_by_uuid(
            vertices_cloned[2].get_uuid(), vertices_cloned[0].get_uuid()));
    EXPECT_THROW(getDAG().connect(vertices_cloned[2], vertices_cloned[0]),
                 DAG::DAGException);

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, connect_all_by_label_and_acyclic_check) {
    std::vector<DAGVertex> vertices_cloned = fill_dag_default();

    EXPECT_TRUE(getDAG().connect_all_by_label("1", "2"));
    EXPECT_THROW(getDAG().connect_all_by_label("2", "1"), DAG::DAGException);
    EXPECT_TRUE(getDAG().connect_all_by_label("1a", "5"));

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, add_and_connect_and_acyclic_check) {
    std::vector<DAGVertex> vertices_cloned = fill_dag_default();
    DAG test_dag("test_dag");

    {
        DAGVertex clone0 = vertices_cloned[0].clone();
        DAGVertex clone1 = vertices_cloned[1].clone();
        EXPECT_TRUE(test_dag.add_and_connect(std::move(clone0),
                                             std::move(clone1)));
    }

    {
        DAGVertex clone0 = vertices_cloned[1].clone();
        DAGVertex clone1 = vertices_cloned[0].clone();
        EXPECT_THROW(
                test_dag.add_and_connect(std::move(clone0), std::move(clone1)),
                DAG::DAGException);
    }

    {
        DAGVertex clone0 = vertices_cloned[2].clone();
        DAGVertex clone1 = vertices_cloned[3].clone();
        EXPECT_TRUE(test_dag.add_and_connect(std::move(clone0),
                                             std::move(clone1)));
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, connect_by_uuid) {
    std::vector<DAGVertex> vertices_cloned = fill_dag_default();
    DAG test_dag("test_dag");

    {
        DAGVertex clone0 = vertices_cloned[0].clone();
        DAGVertex clone1 = vertices_cloned[1].clone();
        test_dag.add_vertex(vertices_cloned[0].clone());
        test_dag.add_vertex(vertices_cloned[1].clone());
        EXPECT_TRUE(test_dag.connect_by_uuid(clone0.get_uuid(),
                                             clone1.get_uuid()));

        std::weak_ptr<DAGVertex> clone0_confirm = test_dag.find_vertex(clone0);
        EXPECT_NE(nullptr, clone0_confirm.lock());
        std::weak_ptr<DAGVertex> clone1_confirm = test_dag.find_vertex(clone1);
        EXPECT_NE(nullptr, clone1_confirm.lock());
        EXPECT_TRUE(clone0_confirm.lock()->contains_connection_to(
                *(clone1_confirm.lock().get())));
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, are_connected) {
    std::vector<DAGVertex> vertices_cloned = fill_dag_default();

    {
        DAGVertex clone0 = vertices_cloned[0].clone();
        DAGVertex clone1 = vertices_cloned[1].clone();
        DAGVertex clone2 = vertices_cloned[2].clone();
        EXPECT_TRUE(getDAG().connect(clone0, clone1));
        EXPECT_TRUE(getDAG().are_connected(clone0, clone1));
        EXPECT_FALSE(getDAG().are_connected(clone1, clone2));
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, are_connected_by_uuid) {
    std::vector<DAGVertex> vertices_cloned = fill_dag_default();

    {
        DAGVertex clone0 = vertices_cloned[0].clone();
        DAGVertex clone1 = vertices_cloned[1].clone();
        DAGVertex clone2 = vertices_cloned[2].clone();
        EXPECT_TRUE(getDAG().connect(clone0, clone1));
        EXPECT_TRUE(getDAG().are_connected_by_uuid(clone0.get_uuid(),
                                                   clone1.get_uuid()));
        EXPECT_FALSE(getDAG().are_connected_by_uuid(clone1.get_uuid(),
                                                    clone2.get_uuid()));
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, all_are_connected_by_label) {
    std::vector<DAGVertex> vertices_cloned = fill_dag_default();

    EXPECT_TRUE(getDAG().connect_all_by_label("1", "2"));
    EXPECT_TRUE(getDAG().connect_all_by_label("1a", "5"));

    EXPECT_TRUE(getDAG().all_are_connected_by_label("1", "2"));
    EXPECT_TRUE(getDAG().all_are_connected_by_label("1a", "5"));
    EXPECT_FALSE(getDAG().all_are_connected_by_label("3", "5"));
    EXPECT_FALSE(getDAG().all_are_connected_by_label("5", "3"));

    EXPECT_TRUE(getDAG().connect_all_by_label("3", "5"));
    EXPECT_TRUE(getDAG().all_are_connected_by_label("3", "5"));
    EXPECT_FALSE(getDAG().all_are_connected_by_label("5", "3"));

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, get_vertex_at) {
    std::vector<DAGVertex> vertices_cloned = fill_dag_default();
    ASSERT_EQ(vertices_cloned.size(), getDAG().vertexCount());
    for (std::size_t i = 0; i < vertices_cloned.size(); ++i) {
        EXPECT_EQ(vertices_cloned[i], *(getDAG().get_vertex_at(i)));
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, clone_connections) {
    std::vector<DAGVertex> vertices_cloned = fill_dag_default();
    for (std::size_t i = 1; i < vertices_cloned.size(); ++i) {
        getDAG().connect(vertices_cloned[0], vertices_cloned[i]);
    }
    DAGVertex v = vertices_cloned[0].clone();
    getDAG().clone_connections(*(getDAG().get_vertex_at(0)), v);
    EXPECT_EQ(v, *(getDAG().get_vertex_at(0)));

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, copy_ctor) {
    {
        DAG d_copy(getDAG());
        EXPECT_EQ(d_copy, getDAG());
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());

    {
        std::vector<DAGVertex> vertices_cloned = fill_dag_default();
        DAG d_copy(getDAG());
        EXPECT_EQ(d_copy, getDAG());
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());

    {
        std::vector<DAGVertex> vertices_cloned = fill_dag_default();
        getDAG().connect_all_by_label("1", "2");
        getDAG().connect_all_by_label("2", "3");
        getDAG().connect_all_by_label("3", "5");
        DAG d_copy(getDAG());
        EXPECT_EQ(d_copy, getDAG());
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, assignment_operator) {
    {
        DAG d_copy;
        d_copy = getDAG();
        EXPECT_EQ(d_copy, getDAG());
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());

    {
        std::vector<DAGVertex> vertices_cloned = fill_dag_default();
        DAG d_copy;
        d_copy = getDAG();
        EXPECT_EQ(d_copy, getDAG());
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());

    {
        std::vector<DAGVertex> vertices_cloned = fill_dag_default();
        getDAG().connect_all_by_label("1", "2");
        getDAG().connect_all_by_label("2", "3");
        getDAG().connect_all_by_label("3", "5");
        DAG d_copy;
        d_copy = getDAG();
        EXPECT_EQ(d_copy, getDAG());
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, remove_vertex) {
    fill_dag_default();
    DAG g_clone = getDAG().clone();

    auto find_v_1a = g_clone.find_all_vertices_with_label("1a");
    ASSERT_EQ(1u, find_v_1a.size());
    EXPECT_EQ("1a", find_v_1a[0].lock()->label());
    g_clone.connect_all_by_label("1a", "1");
    g_clone.connect_all_by_label("1a", "1b");
    g_clone.connect_all_by_label("1a", "2");
    g_clone.connect_all_by_label("1a", "3");
    g_clone.connect_all_by_label("1a", "4");
    g_clone.connect_all_by_label("1a", "5");
    g_clone.linear_traversal([&](std::shared_ptr<DAGVertex> v) {
        ASSERT_NE(nullptr, v.get());
        if (v->label() != "1a") {
            EXPECT_EQ(1u, v->incoming_edge_count());
        } else {
            EXPECT_EQ(0u, v->incoming_edge_count());
        }
    });

    ASSERT_TRUE(g_clone.remove_vertex(*(find_v_1a[0].lock())));

    EXPECT_EQ(0u, g_clone.find_all_vertices_with_label("1a").size());
    g_clone.linear_traversal([&](std::shared_ptr<DAGVertex> v) {
        ASSERT_NE(nullptr, v.get());
        EXPECT_EQ(0u, v->incoming_edge_count());
    });

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, remove_vertex_by_uuid) {
    fill_dag_default();
    DAG g_clone = getDAG().clone();

    auto find_v_1a = g_clone.find_all_vertices_with_label("1a");
    g_clone.connect_all_by_label("1a", "1");
    g_clone.connect_all_by_label("1a", "1b");
    g_clone.connect_all_by_label("1a", "2");
    g_clone.connect_all_by_label("1a", "3");
    g_clone.connect_all_by_label("1a", "4");
    g_clone.connect_all_by_label("1a", "5");
    ASSERT_EQ(11u, g_clone.vertexCount());
    ASSERT_EQ(1u, find_v_1a.size());
    EXPECT_EQ("1a", find_v_1a[0].lock()->label());

    ASSERT_TRUE(
            g_clone.remove_vertex_by_uuid(find_v_1a[0].lock()->get_uuid()));

    EXPECT_EQ(0u, g_clone.find_all_vertices_with_label("1a").size());
    g_clone.linear_traversal([&](std::shared_ptr<DAGVertex> v) {
        ASSERT_NE(nullptr, v.get());
        EXPECT_EQ(0u, v->incoming_edge_count());
    });

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDag, remove_vertices_with_label) {
    fillDAGDefaultWithTasks();
    DAG g_clone = getDAG().clone();

    auto find_v_1a = g_clone.find_all_vertices_with_label("1a");
    g_clone.connect_all_by_label("1a", "1");
    g_clone.connect_all_by_label("1a", "1b");
    g_clone.connect_all_by_label("1a", "2");
    g_clone.connect_all_by_label("1a", "3");
    g_clone.connect_all_by_label("1a", "4");
    g_clone.connect_all_by_label("1a", "5");
    g_clone.connect_all_by_label("1", "2");
    EXPECT_EQ(11u, g_clone.vertexCount());
    ASSERT_TRUE(g_clone.remove_all_vertex_with_label("1"));
    ASSERT_TRUE(g_clone.remove_all_vertex_with_label("1a"));
    EXPECT_EQ(8u, g_clone.vertexCount());

    g_clone.linear_traversal([&](std::shared_ptr<DAGVertex> v) {
        ASSERT_NE(nullptr, v.get());
        EXPECT_EQ(0u, v->incoming_edge_count());
    });
}
}  // namespace com::dag_scheduler
