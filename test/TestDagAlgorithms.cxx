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

#include "dag_scheduler/DagAlgorithms.h"

namespace com::dag_scheduler {
class TestDAGAlgorithms : public ::testing::Test {
protected:
    void SetUp() override {}

    void TearDown() override {}

    DAG& getDAG() { return d_; }

    std::vector<DAGVertex> fillDAGDefault() {
        DAGVertex vertex_0("a"), vertex_1("b"), vertex_2("c"), vertex_3("d"),
                vertex_4("e"), vertex_5("f"), vertex_6("g"), vertex_7("h"),
                vertex_8("i"), vertex_9("j");
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

private:
    DAG d_;
};

TEST_F(TestDAGAlgorithms, dagVerticesWithNoIncomingEdges) {
    {
        std::vector<DAGVertex> vertices = fillDAGDefault();

        getDAG().connect(vertices[0], vertices[1]);  // a -> b
        getDAG().connect(vertices[0], vertices[2]);  // a -> c
        getDAG().connect(vertices[0], vertices[4]);  // a -> e
        getDAG().connect(vertices[1], vertices[3]);  // b -> d
        getDAG().connect(vertices[1], vertices[5]);  // b -> f
        getDAG().connect(vertices[2], vertices[3]);  // c -> d
        getDAG().connect(vertices[4], vertices[5]);  // e -> f
        getDAG().connect(vertices[4], vertices[6]);  // e -> g
        getDAG().connect(vertices[5], vertices[6]);  // f -> g
        getDAG().connect(vertices[5], vertices[7]);  // f -> h
        getDAG().connect(vertices[5], vertices[8]);  // f -> i
        getDAG().connect(vertices[5], vertices[9]);  // f -> j
        getDAG().connect(vertices[6], vertices[7]);  // g -> h

        std::vector<std::shared_ptr<DAGVertex>> actual =
                dagVerticesWithNoIncomingEdges(getDAG());

        EXPECT_EQ(1u, actual.size());
        EXPECT_EQ("a", actual[0]->label());

        getDAG().reset();
    }
}

TEST_F(TestDAGAlgorithms, dag_topological_sort) {
    std::vector<DAGVertex> vertices = fillDAGDefault();

    getDAG().connect(vertices[0], vertices[1]);
    getDAG().connect(vertices[1], vertices[2]);
    getDAG().connect(vertices[2], vertices[3]);
    getDAG().connect(vertices[3], vertices[4]);
    getDAG().connect(vertices[4], vertices[5]);
    getDAG().connect(vertices[5], vertices[6]);
    getDAG().connect(vertices[6], vertices[7]);
    getDAG().connect(vertices[7], vertices[8]);
    getDAG().connect(vertices[8], vertices[9]);

    DAG g_clone = getDAG().clone();
    ASSERT_EQ(getDAG(), g_clone);

    std::list<DAGVertex> sorted_vertices;
    dagTopologicalSort(g_clone, sorted_vertices);

    std::size_t index = 0;
    for (DAGVertex& vertex : sorted_vertices) {
        EXPECT_EQ(vertex, vertices[index]);
        ++index;
    }

    getDAG().reset();
}

TEST_F(TestDAGAlgorithms, dag_topological_sort_non_linear) {
    std::vector<DAGVertex> vertices = fillDAGDefault();

    getDAG().connect(vertices[0], vertices[1]);  // a -> b
    getDAG().connect(vertices[0], vertices[2]);  // a -> c
    getDAG().connect(vertices[0], vertices[4]);  // a -> e
    getDAG().connect(vertices[1], vertices[3]);  // b -> d
    getDAG().connect(vertices[1], vertices[5]);  // b -> f
    getDAG().connect(vertices[2], vertices[3]);  // c -> d
    getDAG().connect(vertices[4], vertices[5]);  // e -> f
    getDAG().connect(vertices[4], vertices[6]);  // e -> g
    getDAG().connect(vertices[5], vertices[6]);  // f -> g
    getDAG().connect(vertices[5], vertices[7]);  // f -> h
    getDAG().connect(vertices[5], vertices[8]);  // f -> i
    getDAG().connect(vertices[5], vertices[9]);  // f -> j
    getDAG().connect(vertices[6], vertices[7]);  // g -> h

    DAG g_clone = getDAG().clone();
    ASSERT_EQ(getDAG(), g_clone);

    std::list<DAGVertex> sorted_vertices;
    dagTopologicalSort(g_clone, sorted_vertices);
    ASSERT_EQ(g_clone.vertexCount(), sorted_vertices.size());

    std::vector<DAGVertex> expected;
    DAGVertex tmp_vertex;
    tmp_vertex = vertices[0].clone();
    expected.push_back(std::move(tmp_vertex));
    tmp_vertex = vertices[1].clone();
    expected.push_back(std::move(tmp_vertex));
    tmp_vertex = vertices[2].clone();
    expected.push_back(std::move(tmp_vertex));
    tmp_vertex = vertices[4].clone();
    expected.push_back(std::move(tmp_vertex));
    tmp_vertex = vertices[3].clone();
    expected.push_back(std::move(tmp_vertex));
    tmp_vertex = vertices[5].clone();
    expected.push_back(std::move(tmp_vertex));
    tmp_vertex = vertices[6].clone();
    expected.push_back(std::move(tmp_vertex));
    tmp_vertex = vertices[8].clone();
    expected.push_back(std::move(tmp_vertex));
    tmp_vertex = vertices[9].clone();
    expected.push_back(std::move(tmp_vertex));
    tmp_vertex = vertices[7].clone();
    expected.push_back(std::move(tmp_vertex));

    std::size_t index = 0;
    for (DAGVertex& vertex : sorted_vertices) {
        EXPECT_EQ(expected[index], vertex);
        ++index;
    }

    getDAG().reset();
}

TEST_F(TestDAGAlgorithms, process_dag) {
    {
        std::vector<DAGVertex> vertices = fillDAGDefault();

        getDAG().connect(vertices[0], vertices[1]);  // a -> b
        getDAG().connect(vertices[0], vertices[2]);  // a -> c
        getDAG().connect(vertices[0], vertices[4]);  // a -> e
        getDAG().connect(vertices[1], vertices[3]);  // b -> d
        getDAG().connect(vertices[1], vertices[5]);  // b -> f
        getDAG().connect(vertices[2], vertices[3]);  // c -> d
        getDAG().connect(vertices[4], vertices[5]);  // e -> f
        getDAG().connect(vertices[4], vertices[6]);  // e -> g
        getDAG().connect(vertices[5], vertices[6]);  // f -> g
        getDAG().connect(vertices[5], vertices[7]);  // f -> h
        getDAG().connect(vertices[5], vertices[8]);  // f -> i
        getDAG().connect(vertices[5], vertices[9]);  // f -> j
        getDAG().connect(vertices[6], vertices[7]);  // g -> h

        ProcessedOrder_t ordered_batches;

        TaskScheduler task_scheduler;
        EXPECT_TRUE(processDAG(getDAG(), ordered_batches, task_scheduler));

        ASSERT_EQ(5u, ordered_batches.size());
        ASSERT_EQ(1u, ordered_batches[0].size());
        EXPECT_EQ("a", ordered_batches[0][0].label());
        ASSERT_EQ(3u, ordered_batches[1].size());
        EXPECT_EQ("b", ordered_batches[1][0].label());
        EXPECT_EQ("c", ordered_batches[1][1].label());
        EXPECT_EQ("e", ordered_batches[1][2].label());
        ASSERT_EQ(2u, ordered_batches[2].size());
        EXPECT_EQ("d", ordered_batches[2][0].label());
        EXPECT_EQ("f", ordered_batches[2][1].label());
        ASSERT_EQ(3u, ordered_batches[3].size());
        EXPECT_EQ("g", ordered_batches[3][0].label());
        EXPECT_EQ("i", ordered_batches[3][1].label());
        EXPECT_EQ("j", ordered_batches[3][2].label());
        ASSERT_EQ(1u, ordered_batches[4].size());
        EXPECT_EQ("h", ordered_batches[4][0].label());

        getDAG().reset();
    }
}

TEST_F(TestDAGAlgorithms, process_dag_cyclic) {
    {
        std::vector<DAGVertex> vertices = fillDAGDefault();

        getDAG().connect(vertices[0], vertices[1]);  // a -> b
        getDAG().connect(vertices[0], vertices[2]);  // a -> c
        getDAG().connect(vertices[0], vertices[4]);  // a -> e
        getDAG().connect(vertices[1], vertices[3]);  // b -> d
        getDAG().connect(vertices[1], vertices[5]);  // b -> f
        getDAG().connect(vertices[2], vertices[3]);  // c -> d
        getDAG().connect(vertices[4], vertices[5]);  // e -> f
        getDAG().connect(vertices[4], vertices[6]);  // e -> g
        getDAG().connect(vertices[5], vertices[6]);  // f -> g
        getDAG().connect(vertices[5], vertices[7]);  // f -> h
        getDAG().connect(vertices[5], vertices[8]);  // f -> i
        getDAG().connect(vertices[5], vertices[9]);  // f -> j
        getDAG().connect(vertices[6], vertices[7]);  // g -> h
        std::weak_ptr<DAGVertex> vertex_7 = getDAG().findVertex(vertices[7]);
        std::weak_ptr<DAGVertex> vertex_0 = getDAG().findVertex(vertices[0]);
        vertex_7.lock()->connect(vertex_0.lock());

        ProcessedOrder_t ordered_batches;

        TaskScheduler task_scheduler;
        EXPECT_FALSE(processDAG(getDAG(), ordered_batches, task_scheduler));
        getDAG().reset();
    }
}
}  // namespace com::dag_scheduler
