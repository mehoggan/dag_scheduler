#include <gtest/gtest.h>

#include <iostream>

#include "dag_scheduler/dag_algorithms.h"
#include "dag_scheduler/logging.h"

namespace com {
namespace dag_scheduler {
class TestDagAlgorithms : public ::testing::Test {
protected:
    virtual void SetUp() {}

    virtual void TearDown() {}

    DAG& get_dag() { return d_; }

    std::vector<DAGVertex> fill_dag_default() {
        DAGVertex v0("a"), v1("b"), v2("c"), v3("d"), v4("e"), v5("f"), v6("g"),
                v7("h"), v8("i"), v9("j");
        std::vector<DAGVertex> vertices_to_add;
        vertices_to_add.push_back(std::move(v0));
        vertices_to_add.push_back(std::move(v1));
        vertices_to_add.push_back(std::move(v2));
        vertices_to_add.push_back(std::move(v3));
        vertices_to_add.push_back(std::move(v4));
        vertices_to_add.push_back(std::move(v5));
        vertices_to_add.push_back(std::move(v6));
        vertices_to_add.push_back(std::move(v7));
        vertices_to_add.push_back(std::move(v8));
        vertices_to_add.push_back(std::move(v9));

        std::vector<DAGVertex> vertices_cloned;
        vertices_cloned.reserve(vertices_to_add.size());
        std::for_each(
                vertices_to_add.begin(),
                vertices_to_add.end(),
                [&](DAGVertex& v) {
                    DAGVertex v_clone_1 = v.clone();
                    DAGVertex v_clone_2 = v.clone();
                    DAGVertex v_clone_3 = v.clone();

                    vertices_cloned.push_back(std::move(v_clone_1));
                    get_dag().add_vertex(std::move(v_clone_2));

                    std::weak_ptr<DAGVertex> v_weak =
                            get_dag().find_vertex_by_uuid(v_clone_3.get_uuid());
                    EXPECT_FALSE(v_weak.expired());
                });
        EXPECT_EQ(vertices_to_add.size(), get_dag().vertex_count());
        vertices_to_add.clear();

        return vertices_cloned;
    }

private:
    DAG d_;
};

TEST_F(TestDagAlgorithms, dag_vertices_with_no_incomming_edges) {
    {
        std::vector<DAGVertex> vertices = fill_dag_default();

        get_dag().connect(vertices[0], vertices[1]);  // a -> b
        get_dag().connect(vertices[0], vertices[2]);  // a -> c
        get_dag().connect(vertices[0], vertices[4]);  // a -> e
        get_dag().connect(vertices[1], vertices[3]);  // b -> d
        get_dag().connect(vertices[1], vertices[5]);  // b -> f
        get_dag().connect(vertices[2], vertices[3]);  // c -> d
        get_dag().connect(vertices[4], vertices[5]);  // e -> f
        get_dag().connect(vertices[4], vertices[6]);  // e -> g
        get_dag().connect(vertices[5], vertices[6]);  // f -> g
        get_dag().connect(vertices[5], vertices[7]);  // f -> h
        get_dag().connect(vertices[5], vertices[8]);  // f -> i
        get_dag().connect(vertices[5], vertices[9]);  // f -> j
        get_dag().connect(vertices[6], vertices[7]);  // g -> h

        std::vector<std::shared_ptr<DAGVertex>> actual =
                dag_vertices_with_no_incomming_edges(get_dag());

        EXPECT_EQ(1u, actual.size());
        EXPECT_EQ("a", actual[0]->label());

        get_dag().reset();
    }
}

TEST_F(TestDagAlgorithms, dag_topological_sort) {
    std::vector<DAGVertex> vertices = fill_dag_default();

    get_dag().connect(vertices[0], vertices[1]);
    get_dag().connect(vertices[1], vertices[2]);
    get_dag().connect(vertices[2], vertices[3]);
    get_dag().connect(vertices[3], vertices[4]);
    get_dag().connect(vertices[4], vertices[5]);
    get_dag().connect(vertices[5], vertices[6]);
    get_dag().connect(vertices[6], vertices[7]);
    get_dag().connect(vertices[7], vertices[8]);
    get_dag().connect(vertices[8], vertices[9]);

    DAG g_clone = get_dag().clone();
    ASSERT_EQ(get_dag(), g_clone);

    std::list<DAGVertex> sorted_vertices;
    dag_topological_sort(g_clone, sorted_vertices);

    std::size_t i = 0;
    for (DAGVertex& v : sorted_vertices) {
        EXPECT_EQ(v, vertices[i]);
        ++i;
    }

    get_dag().reset();
}

TEST_F(TestDagAlgorithms, dag_topological_sort_non_linear) {
    std::vector<DAGVertex> vertices = fill_dag_default();

    get_dag().connect(vertices[0], vertices[1]);  // a -> b
    get_dag().connect(vertices[0], vertices[2]);  // a -> c
    get_dag().connect(vertices[0], vertices[4]);  // a -> e
    get_dag().connect(vertices[1], vertices[3]);  // b -> d
    get_dag().connect(vertices[1], vertices[5]);  // b -> f
    get_dag().connect(vertices[2], vertices[3]);  // c -> d
    get_dag().connect(vertices[4], vertices[5]);  // e -> f
    get_dag().connect(vertices[4], vertices[6]);  // e -> g
    get_dag().connect(vertices[5], vertices[6]);  // f -> g
    get_dag().connect(vertices[5], vertices[7]);  // f -> h
    get_dag().connect(vertices[5], vertices[8]);  // f -> i
    get_dag().connect(vertices[5], vertices[9]);  // f -> j
    get_dag().connect(vertices[6], vertices[7]);  // g -> h

    DAG g_clone = get_dag().clone();
    ASSERT_EQ(get_dag(), g_clone);

    std::list<DAGVertex> sorted_vertices;
    dag_topological_sort(g_clone, sorted_vertices);
    ASSERT_EQ(g_clone.vertex_count(), sorted_vertices.size());

    std::vector<DAGVertex> expected;
    DAGVertex tmp;
    tmp = vertices[0].clone();
    expected.push_back(std::move(tmp));
    tmp = vertices[1].clone();
    expected.push_back(std::move(tmp));
    tmp = vertices[2].clone();
    expected.push_back(std::move(tmp));
    tmp = vertices[4].clone();
    expected.push_back(std::move(tmp));
    tmp = vertices[3].clone();
    expected.push_back(std::move(tmp));
    tmp = vertices[5].clone();
    expected.push_back(std::move(tmp));
    tmp = vertices[6].clone();
    expected.push_back(std::move(tmp));
    tmp = vertices[8].clone();
    expected.push_back(std::move(tmp));
    tmp = vertices[9].clone();
    expected.push_back(std::move(tmp));
    tmp = vertices[7].clone();
    expected.push_back(std::move(tmp));

    std::size_t i = 0;
    for (DAGVertex& v : sorted_vertices) {
        EXPECT_EQ(expected[i], v);
        ++i;
    }

    get_dag().reset();
}

TEST_F(TestDagAlgorithms, process_dag) {
    {
        std::vector<DAGVertex> vertices = fill_dag_default();

        get_dag().connect(vertices[0], vertices[1]);  // a -> b
        get_dag().connect(vertices[0], vertices[2]);  // a -> c
        get_dag().connect(vertices[0], vertices[4]);  // a -> e
        get_dag().connect(vertices[1], vertices[3]);  // b -> d
        get_dag().connect(vertices[1], vertices[5]);  // b -> f
        get_dag().connect(vertices[2], vertices[3]);  // c -> d
        get_dag().connect(vertices[4], vertices[5]);  // e -> f
        get_dag().connect(vertices[4], vertices[6]);  // e -> g
        get_dag().connect(vertices[5], vertices[6]);  // f -> g
        get_dag().connect(vertices[5], vertices[7]);  // f -> h
        get_dag().connect(vertices[5], vertices[8]);  // f -> i
        get_dag().connect(vertices[5], vertices[9]);  // f -> j
        get_dag().connect(vertices[6], vertices[7]);  // g -> h

        processed_order_type ordered_batches;

        TaskScheduler ts;
        EXPECT_TRUE(process_dag(get_dag(), ordered_batches, ts));

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

        get_dag().reset();
    }
}

TEST_F(TestDagAlgorithms, process_dag_cyclic) {
    {
        std::vector<DAGVertex> vertices = fill_dag_default();

        get_dag().connect(vertices[0], vertices[1]);  // a -> b
        get_dag().connect(vertices[0], vertices[2]);  // a -> c
        get_dag().connect(vertices[0], vertices[4]);  // a -> e
        get_dag().connect(vertices[1], vertices[3]);  // b -> d
        get_dag().connect(vertices[1], vertices[5]);  // b -> f
        get_dag().connect(vertices[2], vertices[3]);  // c -> d
        get_dag().connect(vertices[4], vertices[5]);  // e -> f
        get_dag().connect(vertices[4], vertices[6]);  // e -> g
        get_dag().connect(vertices[5], vertices[6]);  // f -> g
        get_dag().connect(vertices[5], vertices[7]);  // f -> h
        get_dag().connect(vertices[5], vertices[8]);  // f -> i
        get_dag().connect(vertices[5], vertices[9]);  // f -> j
        get_dag().connect(vertices[6], vertices[7]);  // g -> h
        std::weak_ptr<DAGVertex> v7 = get_dag().find_vertex(vertices[7]);
        std::weak_ptr<DAGVertex> v0 = get_dag().find_vertex(vertices[0]);
        v7.lock().get()->connect(v0.lock());

        processed_order_type ordered_batches;

        TaskScheduler ts;
        EXPECT_FALSE(process_dag(get_dag(), ordered_batches, ts));
        get_dag().reset();
    }
}
}  // namespace dag_scheduler
}  // namespace com
