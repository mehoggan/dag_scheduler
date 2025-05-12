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
class TestDAG : public ::testing::Test, public LoggedClass<TestDAG> {
public:
    TestDAG() : LoggedClass<TestDAG>(*this) {}

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
                          getGenericInput(initial_input);
                          // TODO (mehoggan): Uncomment once
                          // override_initial_input_for_vertex_task is
                          // completed.
                          //
                          // getDAG().override_initial_input_for_vertex_task(
                          //   uuid_cloned, initial_input);
                          // std::weak_ptr<DAGVertex> v_weak =
                          //   getDAG().findVertex_by_uuid(uuid_cloned);
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

TEST_F(TestDAG, ctor) {
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
    std::string actual_config;
    getDAG().jsonConfigStr(actual_config);
    EXPECT_EQ("{}", actual_config);
}

TEST_F(TestDAG, ctorWithTitle) {
    DAG test_dag("test_dag");

    EXPECT_EQ(0ul, test_dag.edgeCount());
    EXPECT_EQ(0ul, test_dag.vertexCount());
    EXPECT_EQ(std::string("test_dag"), test_dag.title());
    std::string actual_config;
    getDAG().jsonConfigStr(actual_config);
    EXPECT_EQ("{}", actual_config);
}

TEST_F(TestDAG, ctorWithTitleAndConfig) {
    rapidjson::Document json_config;
    TestDAG::getGenericConfig(json_config);
    DAG test_dag("test_dag", json_config);

    EXPECT_EQ(0ul, test_dag.edgeCount());
    EXPECT_EQ(0ul, test_dag.vertexCount());
    EXPECT_EQ(std::string("test_dag"), test_dag.title());
    std::string actual_config;
    test_dag.jsonConfigStr(actual_config);
    EXPECT_EQ(TestDAG::getExpectedConfigStr(), actual_config);
}

TEST_F(TestDAG, moveCtorNoEdgesNoVertices) {
    DAG test_dag;

    DAG d_moved(std::move(test_dag));
    EXPECT_EQ(0ul, d_moved.edgeCount());
    EXPECT_EQ(0ul, d_moved.vertexCount());
    std::string actual_config;
    getDAG().jsonConfigStr(actual_config);
    EXPECT_EQ("{}", actual_config);
}

TEST_F(TestDAG, moveCtorNoEdgesNoVerticesTitleAndConfig) {
    rapidjson::Document json_config;
    TestDAG::getGenericConfig(json_config);
    DAG test_dag("test_dag", json_config);

    DAG d_moved(std::move(test_dag));
    EXPECT_EQ(0ul, d_moved.edgeCount());
    EXPECT_EQ(0ul, d_moved.vertexCount());
    std::string actual_moved_config;
    d_moved.jsonConfigStr(actual_moved_config);
    EXPECT_EQ(TestDAG::getExpectedConfigStr(), actual_moved_config);

    std::string actual_config;
    test_dag.jsonConfigStr(actual_config);  // NOLINT
    EXPECT_EQ("", actual_config) << "Empty because we moved it.";
}

TEST_F(TestDAG, assignmentMoveOperatorNoEdgesNoVertices) {
    DAG test_dag;

    DAG d_moved;
    d_moved = std::move(test_dag);
    EXPECT_EQ(0ul, d_moved.edgeCount());
    EXPECT_EQ(0ul, d_moved.vertexCount());
    std::string actual_config;
    getDAG().jsonConfigStr(actual_config);
    EXPECT_EQ("{}", actual_config);
}

TEST_F(TestDAG, assignmentMoveOperatorNoEdgesNoVerticesTitleAndConfig) {
    rapidjson::Document json_config;
    TestDAG::getGenericConfig(json_config);
    DAG test_dag("test_dag", json_config);

    DAG d_moved;
    d_moved = std::move(test_dag);
    EXPECT_EQ(0ul, d_moved.edgeCount());
    EXPECT_EQ(0ul, d_moved.vertexCount());
    std::string actual_moved_config;
    d_moved.jsonConfigStr(actual_moved_config);
    EXPECT_EQ(TestDAG::getExpectedConfigStr(), actual_moved_config);

    std::string actual_config;
    test_dag.jsonConfigStr(actual_config);  // NOLINT
    EXPECT_EQ("", actual_config) << "Empty because we moved it.";
}

TEST_F(TestDAG, cloneNoEdgesNoVertices) {
    DAG test_dag;

    DAG d_cloned = test_dag.clone();
    EXPECT_EQ(0ul, d_cloned.edgeCount());
    EXPECT_EQ(0ul, d_cloned.vertexCount());
    std::string actual_config;
    test_dag.jsonConfigStr(actual_config);
    EXPECT_EQ("{}", actual_config);
    d_cloned.jsonConfigStr(actual_config);
    EXPECT_EQ("{}", actual_config);
}

TEST_F(TestDAG, cloneNoEdgesNoVerticesTitleAndConfig) {
    rapidjson::Document json_config;
    TestDAG::getGenericConfig(json_config);
    DAG test_dag("test_dag", json_config);

    DAG d_cloned = test_dag.clone();
    EXPECT_EQ(0ul, d_cloned.edgeCount());
    EXPECT_EQ(0ul, d_cloned.vertexCount());

    std::string actual_config;
    test_dag.jsonConfigStr(actual_config);
    EXPECT_EQ(TestDAG::getExpectedConfigStr(), actual_config);

    std::string actual_cloned_config;
    test_dag.jsonConfigStr(actual_cloned_config);
    EXPECT_EQ(TestDAG::getExpectedConfigStr(), actual_cloned_config);
}

TEST_F(TestDAG, addVertexAndReset) {
    DAGVertex vertex_1("1");
    getDAG().addVertex(std::move(vertex_1));
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(1ul, getDAG().vertexCount());

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, addSingleVertexAndFindVertexByVertexAndUUID) {
    DAGVertex vertex_1("1");
    DAGVertex v1_cloned = vertex_1.clone();
    getDAG().addVertex(std::move(vertex_1));

    std::weak_ptr<DAGVertex> v1_weak = getDAG().findVertex(v1_cloned);
    EXPECT_EQ(v1_cloned, *(v1_weak.lock()));

    std::weak_ptr<DAGVertex> v2_weak =
            getDAG().findVertexByUUID(v1_cloned.getUUID());
    EXPECT_EQ(v1_cloned, *(v2_weak.lock()));
    EXPECT_TRUE(getDAG().containsVertex(v1_cloned));
    EXPECT_TRUE(getDAG().containsVertexByUUID(v1_cloned.getUUID()));
    EXPECT_TRUE(getDAG().containsVertexByLabel(v1_cloned.label()));

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
    EXPECT_TRUE(v1_weak.expired());
    EXPECT_TRUE(v2_weak.expired());
}

TEST_F(TestDAG, addSingleVertexAndFindVertexByLabel) {
    DAGVertex vertex_1("1");
    DAGVertex v1_cloned = vertex_1.clone();
    getDAG().addVertex(std::move(vertex_1));

    std::vector<std::weak_ptr<DAGVertex>> vertices =
            getDAG().findAllVerticesWithLabel(v1_cloned.label());
    EXPECT_EQ(1ul, vertices.size());
    EXPECT_EQ(v1_cloned, *(vertices[0].lock()));

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
    EXPECT_TRUE(vertices[0].expired());
}

TEST_F(TestDAG, addVerticesWithSameAndDifferentLabels) {
    std::vector<DAGVertex> vertices_cloned = fillDAGDefault();

    {
        std::weak_ptr<DAGVertex> v_weak =
                getDAG().findVertex(vertices_cloned[0]);
        ASSERT_FALSE(v_weak.expired());
        EXPECT_EQ(vertices_cloned[0], *(v_weak.lock()));
        EXPECT_NE(vertices_cloned[1], *(v_weak.lock()));

        std::shared_ptr<DAGVertex> vertex_ptr = v_weak.lock();
        EXPECT_TRUE(getDAG().containsVertex(*vertex_ptr));
        EXPECT_TRUE(getDAG().containsVertexByUUID(vertex_ptr->getUUID()));
        EXPECT_TRUE(getDAG().containsVertexByLabel(vertex_ptr->label()));
    }

    {
        std::weak_ptr<DAGVertex> v_weak =
                getDAG().findVertexByUUID(vertices_cloned[1].getUUID());
        ASSERT_FALSE(v_weak.expired());
        EXPECT_NE(vertices_cloned[0], *(v_weak.lock()));
        EXPECT_EQ(vertices_cloned[1], *(v_weak.lock()));

        std::shared_ptr<DAGVertex> vertex_ptr = v_weak.lock();
        EXPECT_TRUE(getDAG().containsVertex(*vertex_ptr));
        EXPECT_TRUE(getDAG().containsVertexByUUID(vertex_ptr->getUUID()));
        EXPECT_TRUE(getDAG().containsVertexByLabel(vertex_ptr->label()));
    }

    {
        std::weak_ptr<DAGVertex> v_weak =
                getDAG().findVertexByUUID(vertices_cloned[6].getUUID());
        ASSERT_FALSE(v_weak.expired());
        EXPECT_NE(vertices_cloned[4], *(v_weak.lock()));
        EXPECT_NE(vertices_cloned[5], *(v_weak.lock()));
        EXPECT_EQ(vertices_cloned[6], *(v_weak.lock()));

        std::shared_ptr<DAGVertex> vertex_ptr = v_weak.lock();
        EXPECT_TRUE(getDAG().containsVertex(*vertex_ptr));
        EXPECT_TRUE(getDAG().containsVertexByUUID(vertex_ptr->getUUID()));
        EXPECT_TRUE(getDAG().containsVertexByLabel(vertex_ptr->label()));
    }

    {
        std::vector<std::weak_ptr<DAGVertex>> v_weaks =
                getDAG().findAllVerticesWithLabel("2");
        EXPECT_EQ(3ul, v_weaks.size());
        EXPECT_EQ(vertices_cloned[4], *(v_weaks[0].lock()));
        EXPECT_EQ(vertices_cloned[5], *(v_weaks[1].lock()));
        EXPECT_EQ(vertices_cloned[6], *(v_weaks[2].lock()));

        for (const std::weak_ptr<DAGVertex>& v_weak : v_weaks) {
            std::shared_ptr<DAGVertex> vertex_ptr = v_weak.lock();
            EXPECT_TRUE(getDAG().containsVertex(*vertex_ptr));
            EXPECT_TRUE(getDAG().containsVertexByUUID(vertex_ptr->getUUID()));
            EXPECT_TRUE(getDAG().containsVertexByLabel(vertex_ptr->label()));
        }
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, cloneWithMultipleVerticesNoConnections) {
    std::vector<DAGVertex> vertices_cloned = fillDAGDefault();
    DAG dag_cloned = getDAG().clone();

    for (const DAGVertex& vertex : vertices_cloned) {
        EXPECT_TRUE(getDAG().containsVertex(vertex));
        EXPECT_TRUE(dag_cloned.containsVertex(vertex));
    }
    EXPECT_EQ(getDAG(), dag_cloned);

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, connectAndAcyclicCheck) {
    std::vector<DAGVertex> vertices_cloned = fillDAGDefault();

    /* cloned_vertices[0] -> cloned_vertices[1] */
    EXPECT_FALSE(getDAG().connectionWouldMakeCyclic(vertices_cloned[0],
                                                    vertices_cloned[1]));
    EXPECT_FALSE(getDAG().connectionWouldMakeCyclicByUUID(
            vertices_cloned[0].getUUID(), vertices_cloned[1].getUUID()));
    EXPECT_TRUE(getDAG().connect(vertices_cloned[0], vertices_cloned[1]));
    EXPECT_EQ(1ul,
              getDAG().findVertex(vertices_cloned[0]).lock()->edgeCount());
    EXPECT_EQ(0ul,
              getDAG().findVertex(vertices_cloned[0])
                      .lock()
                      ->incomingEdgeCount());
    EXPECT_EQ(0ul,
              getDAG().findVertex(vertices_cloned[1]).lock()->edgeCount());
    EXPECT_EQ(1ul,
              getDAG().findVertex(vertices_cloned[1])
                      .lock()
                      ->incomingEdgeCount());

    /* cloned_vertices[1] -> cloned_vertices[2] */
    EXPECT_FALSE(getDAG().connectionWouldMakeCyclic(vertices_cloned[1],
                                                    vertices_cloned[2]));
    EXPECT_FALSE(getDAG().connectionWouldMakeCyclicByUUID(
            vertices_cloned[1].getUUID(), vertices_cloned[2].getUUID()));
    EXPECT_TRUE(getDAG().connect(vertices_cloned[1], vertices_cloned[2]));

    /* cloned_vertices[0] -> cloned_vertices[2] */
    EXPECT_FALSE(getDAG().connectionWouldMakeCyclic(vertices_cloned[0],
                                                    vertices_cloned[2]));
    EXPECT_FALSE(getDAG().connectionWouldMakeCyclicByUUID(
            vertices_cloned[0].getUUID(), vertices_cloned[2].getUUID()));
    EXPECT_TRUE(getDAG().connect(vertices_cloned[0], vertices_cloned[2]));

    /* cloned_vertices[2] -> cloned_vertices[0] */
    EXPECT_TRUE(getDAG().connectionWouldMakeCyclic(vertices_cloned[2],
                                                   vertices_cloned[0]));
    EXPECT_TRUE(getDAG().connectionWouldMakeCyclicByUUID(
            vertices_cloned[2].getUUID(), vertices_cloned[0].getUUID()));
    EXPECT_THROW(getDAG().connect(vertices_cloned[2], vertices_cloned[0]),
                 DAG::DAGException);

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, connectAllByLabelAndAcyclicCheck) {
    std::vector<DAGVertex> vertices_cloned = fillDAGDefault();

    EXPECT_TRUE(getDAG().connectAllByLabel("1", "2"));
    EXPECT_THROW(getDAG().connectAllByLabel("2", "1"), DAG::DAGException);
    EXPECT_TRUE(getDAG().connectAllByLabel("1a", "5"));

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, addAndConnectAndAcyclicCheck) {
    std::vector<DAGVertex> vertices_cloned = fillDAGDefault();
    DAG test_dag("test_dag");

    {
        DAGVertex clone0 = vertices_cloned[0].clone();
        DAGVertex clone1 = vertices_cloned[1].clone();
        EXPECT_TRUE(
                test_dag.addAndConnect(std::move(clone0), std::move(clone1)));
    }

    {
        DAGVertex clone0 = vertices_cloned[1].clone();
        DAGVertex clone1 = vertices_cloned[0].clone();
        EXPECT_THROW(
                test_dag.addAndConnect(std::move(clone0), std::move(clone1)),
                DAG::DAGException);
    }

    {
        DAGVertex clone0 = vertices_cloned[2].clone();
        DAGVertex clone1 = vertices_cloned[3].clone();
        EXPECT_TRUE(
                test_dag.addAndConnect(std::move(clone0), std::move(clone1)));
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, connectByUUID) {
    std::vector<DAGVertex> vertices_cloned = fillDAGDefault();
    DAG test_dag("test_dag");

    {
        DAGVertex clone0 = vertices_cloned[0].clone();
        DAGVertex clone1 = vertices_cloned[1].clone();
        test_dag.addVertex(vertices_cloned[0].clone());
        test_dag.addVertex(vertices_cloned[1].clone());
        EXPECT_TRUE(
                test_dag.connectByUUID(clone0.getUUID(), clone1.getUUID()));

        std::weak_ptr<DAGVertex> clone0_confirm = test_dag.findVertex(clone0);
        EXPECT_NE(nullptr, clone0_confirm.lock());
        std::weak_ptr<DAGVertex> clone1_confirm = test_dag.findVertex(clone1);
        EXPECT_NE(nullptr, clone1_confirm.lock());
        EXPECT_TRUE(clone0_confirm.lock()->containsConnectionTo(
                *(clone1_confirm.lock().get())));
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, areConnected) {
    std::vector<DAGVertex> vertices_cloned = fillDAGDefault();

    {
        DAGVertex clone0 = vertices_cloned[0].clone();
        DAGVertex clone1 = vertices_cloned[1].clone();
        DAGVertex clone2 = vertices_cloned[2].clone();
        EXPECT_TRUE(getDAG().connect(clone0, clone1));
        EXPECT_TRUE(getDAG().areConnected(clone0, clone1));
        EXPECT_FALSE(getDAG().areConnected(clone1, clone2));
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, areConnectedByUUID) {
    std::vector<DAGVertex> vertices_cloned = fillDAGDefault();

    {
        DAGVertex clone0 = vertices_cloned[0].clone();
        DAGVertex clone1 = vertices_cloned[1].clone();
        DAGVertex clone2 = vertices_cloned[2].clone();
        EXPECT_TRUE(getDAG().connect(clone0, clone1));
        EXPECT_TRUE(getDAG().areConnectedByUUID(clone0.getUUID(),
                                                clone1.getUUID()));
        EXPECT_FALSE(getDAG().areConnectedByUUID(clone1.getUUID(),
                                                 clone2.getUUID()));
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, allAreConnectedByLabel) {
    std::vector<DAGVertex> vertices_cloned = fillDAGDefault();

    EXPECT_TRUE(getDAG().connectAllByLabel("1", "2"));
    EXPECT_TRUE(getDAG().connectAllByLabel("1a", "5"));

    EXPECT_TRUE(getDAG().allAreConnectedByLabel("1", "2"));
    EXPECT_TRUE(getDAG().allAreConnectedByLabel("1a", "5"));
    EXPECT_FALSE(getDAG().allAreConnectedByLabel("3", "5"));
    EXPECT_FALSE(getDAG().allAreConnectedByLabel("5", "3"));

    EXPECT_TRUE(getDAG().connectAllByLabel("3", "5"));
    EXPECT_TRUE(getDAG().allAreConnectedByLabel("3", "5"));
    EXPECT_FALSE(getDAG().allAreConnectedByLabel("5", "3"));

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, getVertexAt) {
    std::vector<DAGVertex> vertices_cloned = fillDAGDefault();
    ASSERT_EQ(vertices_cloned.size(), getDAG().vertexCount());
    for (std::size_t i = 0; i < vertices_cloned.size(); ++i) {
        EXPECT_EQ(vertices_cloned[i], *(getDAG().getVertexAt(i)));
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, cloneConnections) {
    std::vector<DAGVertex> vertices_cloned = fillDAGDefault();
    for (std::size_t i = 1; i < vertices_cloned.size(); ++i) {
        getDAG().connect(vertices_cloned[0], vertices_cloned[i]);
    }
    DAGVertex vertex = vertices_cloned[0].clone();
    getDAG().cloneConnections(*(getDAG().getVertexAt(0)), vertex);
    EXPECT_EQ(vertex, *(getDAG().getVertexAt(0)));

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, copyCtor) {
    {
        DAG d_copy(getDAG());  // NOLINT
        EXPECT_EQ(d_copy, getDAG());
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());

    {
        std::vector<DAGVertex> vertices_cloned = fillDAGDefault();
        DAG d_copy(getDAG());
        EXPECT_EQ(d_copy, getDAG());
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());

    {
        std::vector<DAGVertex> vertices_cloned = fillDAGDefault();
        getDAG().connectAllByLabel("1", "2");
        getDAG().connectAllByLabel("2", "3");
        getDAG().connectAllByLabel("3", "5");
        DAG d_copy(getDAG());
        EXPECT_EQ(d_copy, getDAG());
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, assignmentOperator) {
    {
        DAG d_copy;
        d_copy = getDAG();
        EXPECT_EQ(d_copy, getDAG());
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());

    {
        std::vector<DAGVertex> vertices_cloned = fillDAGDefault();
        DAG d_copy;
        d_copy = getDAG();
        EXPECT_EQ(d_copy, getDAG());
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());

    {
        std::vector<DAGVertex> vertices_cloned = fillDAGDefault();
        getDAG().connectAllByLabel("1", "2");
        getDAG().connectAllByLabel("2", "3");
        getDAG().connectAllByLabel("3", "5");
        DAG d_copy;
        d_copy = getDAG();
        EXPECT_EQ(d_copy, getDAG());
    }

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, removeVertex) {
    fillDAGDefault();
    DAG g_clone = getDAG().clone();

    auto find_v_1a = g_clone.findAllVerticesWithLabel("1a");
    ASSERT_EQ(1u, find_v_1a.size());
    EXPECT_EQ("1a", find_v_1a[0].lock()->label());
    g_clone.connectAllByLabel("1a", "1");
    g_clone.connectAllByLabel("1a", "1b");
    g_clone.connectAllByLabel("1a", "2");
    g_clone.connectAllByLabel("1a", "3");
    g_clone.connectAllByLabel("1a", "4");
    g_clone.connectAllByLabel("1a", "5");
    g_clone.linearTraversal([&](std::shared_ptr<DAGVertex> vertex_ptr) {
        ASSERT_NE(nullptr, vertex_ptr.get());
        if (vertex_ptr->label() != "1a") {
            EXPECT_EQ(1u, vertex_ptr->incomingEdgeCount());
        } else {
            EXPECT_EQ(0u, vertex_ptr->incomingEdgeCount());
        }
    });

    ASSERT_TRUE(g_clone.removeVertex(*(find_v_1a[0].lock())));

    EXPECT_EQ(0u, g_clone.findAllVerticesWithLabel("1a").size());
    g_clone.linearTraversal([&](std::shared_ptr<DAGVertex> vertex_ptr) {
        ASSERT_NE(nullptr, vertex_ptr.get());
        EXPECT_EQ(0u, vertex_ptr->incomingEdgeCount());
    });

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, removeVertexByUUID) {
    fillDAGDefault();
    DAG g_clone = getDAG().clone();

    auto find_v_1a = g_clone.findAllVerticesWithLabel("1a");
    g_clone.connectAllByLabel("1a", "1");
    g_clone.connectAllByLabel("1a", "1b");
    g_clone.connectAllByLabel("1a", "2");
    g_clone.connectAllByLabel("1a", "3");
    g_clone.connectAllByLabel("1a", "4");
    g_clone.connectAllByLabel("1a", "5");
    ASSERT_EQ(11u, g_clone.vertexCount());
    ASSERT_EQ(1u, find_v_1a.size());
    EXPECT_EQ("1a", find_v_1a[0].lock()->label());

    ASSERT_TRUE(g_clone.removeVertexByUUID(find_v_1a[0].lock()->getUUID()));

    EXPECT_EQ(0u, g_clone.findAllVerticesWithLabel("1a").size());
    g_clone.linearTraversal([&](std::shared_ptr<DAGVertex> vertex_ptr) {
        ASSERT_NE(nullptr, vertex_ptr.get());
        EXPECT_EQ(0u, vertex_ptr->incomingEdgeCount());
    });

    getDAG().reset();
    EXPECT_EQ(0ul, getDAG().edgeCount());
    EXPECT_EQ(0ul, getDAG().vertexCount());
}

TEST_F(TestDAG, removeVerticesWithLabel) {
    fillDAGDefaultWithTasks();
    DAG g_clone = getDAG().clone();

    auto find_v_1a = g_clone.findAllVerticesWithLabel("1a");
    g_clone.connectAllByLabel("1a", "1");
    g_clone.connectAllByLabel("1a", "1b");
    g_clone.connectAllByLabel("1a", "2");
    g_clone.connectAllByLabel("1a", "3");
    g_clone.connectAllByLabel("1a", "4");
    g_clone.connectAllByLabel("1a", "5");
    g_clone.connectAllByLabel("1", "2");
    EXPECT_EQ(11u, g_clone.vertexCount());
    ASSERT_TRUE(g_clone.removeAllVertexWithLabel("1"));
    ASSERT_TRUE(g_clone.removeAllVertexWithLabel("1a"));
    EXPECT_EQ(8u, g_clone.vertexCount());

    g_clone.linearTraversal([&](std::shared_ptr<DAGVertex> vertex_ptr) {
        ASSERT_NE(nullptr, vertex_ptr.get());
        EXPECT_EQ(0u, vertex_ptr->incomingEdgeCount());
    });
}
}  // namespace com::dag_scheduler
