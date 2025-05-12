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

#include <memory>

#include "dag_scheduler/DagEdge.h"
#include "dag_scheduler/DagVertex.h"

namespace com::dag_scheduler {
class TestDagEdge : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TestDagEdge, ctor) {
    DAGEdge edge;
    EXPECT_EQ(DAGEdge::Status::INITIALIZED, edge.currentStatus());
    EXPECT_EQ(std::string("initialized"), edge.currentStatusAsString());
    EXPECT_FALSE(edge.getUUID().asString().empty());
}

TEST_F(TestDagEdge, dtor) {
    DAGEdge edge;
    edge.~DAGEdge();
    EXPECT_EQ(DAGEdge::Status::NON_TRAVERSABLE, edge.currentStatus());
    EXPECT_FALSE(edge.getUUID().isInitialized());
    EXPECT_EQ(nullptr, edge.getConnection().lock().get());
}

TEST_F(TestDagEdge, mtor) {
    DAGEdge edge_1;
    DAGEdge edge_2(std::move(edge_1));

    EXPECT_EQ(DAGEdge::Status::NON_TRAVERSABLE,
              edge_1.currentStatus());               // NOLINT
    EXPECT_FALSE(edge_1.getUUID().isInitialized());  // NOLINT
    EXPECT_EQ(nullptr, edge_1.getConnection().lock().get());

    EXPECT_EQ(DAGEdge::Status::INITIALIZED, edge_2.currentStatus());
    EXPECT_TRUE(edge_2.getUUID().isInitialized());
    EXPECT_EQ(nullptr, edge_2.getConnection().lock().get());

    // Test safety of move ctor by moving an object that has already been
    // moved.
    DAGEdge edge_3(std::move(edge_1));
    EXPECT_EQ(DAGEdge::Status::NON_TRAVERSABLE, edge_3.currentStatus());
    EXPECT_EQ(nullptr, edge_3.getConnection().lock().get());
}

TEST_F(TestDagEdge, massign) {
    DAGEdge edge_1;
    DAGEdge edge_2;
    edge_2 = std::move(edge_1);

    EXPECT_EQ(DAGEdge::Status::NON_TRAVERSABLE,
              edge_1.currentStatus());  // NOLINT
    EXPECT_FALSE(edge_1.getUUID().isInitialized());
    EXPECT_EQ(nullptr, edge_1.getConnection().lock().get());

    EXPECT_EQ(DAGEdge::Status::INITIALIZED, edge_2.currentStatus());
    EXPECT_TRUE(edge_2.getUUID().isInitialized());
    EXPECT_EQ(nullptr, edge_2.getConnection().lock().get());
}

TEST_F(TestDagEdge, connectAndClone) {
    DAGEdge edge;
    std::shared_ptr<DAGVertex> vertex_1 = std::make_shared<DAGVertex>("to");
    EXPECT_TRUE(edge.connectTo(vertex_1));
    DAGEdge clone = edge.clone();

    EXPECT_NE(clone.getConnection().lock(), edge.getConnection().lock());
    EXPECT_NE(nullptr, edge.getConnection().lock().get());
    EXPECT_EQ("to", edge.getConnection().lock()->label());
    EXPECT_EQ(2u, edge.getConnection().lock().use_count());
    EXPECT_EQ(nullptr, clone.getConnection().lock());

    std::shared_ptr<DAGVertex> tmp_vertex =
            std::make_shared<DAGVertex>(edge.getConnection().lock()->clone());
    clone.connectTo(tmp_vertex);
    std::uintptr_t clone_connect_addr = reinterpret_cast<std::uintptr_t>(
            &(*(clone.getConnection().lock())));
    std::uintptr_t e_connect_addr = reinterpret_cast<std::uintptr_t>(
            &(*(edge.getConnection().lock())));

    EXPECT_NE(clone_connect_addr, e_connect_addr);
    EXPECT_NE(nullptr, clone.getConnection().lock());
    EXPECT_EQ(*(edge.getConnection().lock()), *(clone.getConnection().lock()));
    EXPECT_EQ(2u, edge.getConnection().lock().use_count());
    EXPECT_EQ(2u, clone.getConnection().lock().use_count());
    EXPECT_EQ("to", edge.getConnection().lock()->label());
    EXPECT_EQ("to", clone.getConnection().lock()->label());

    // After reconnecting edge to clone the edges should be the same.
    EXPECT_EQ(edge, clone);
}

TEST_F(TestDagEdge, connectToNull) {
    DAGEdge edge;
    std::shared_ptr<DAGVertex> vertex_1 = std::make_shared<DAGVertex>("to");
    EXPECT_TRUE(edge.connectTo(vertex_1));
    EXPECT_EQ(edge.getConnection().lock(), vertex_1);
    EXPECT_EQ(1ul, vertex_1->incomingEdgeCount());
    EXPECT_EQ(2l, edge.getConnection().lock().use_count());
    EXPECT_FALSE(edge.connectTo(nullptr));
    EXPECT_EQ(nullptr, edge.getConnection().lock());
    EXPECT_EQ(0ul, vertex_1->incomingEdgeCount());
}

TEST_F(TestDagEdge, connections) {
    DAGEdge edge;
    std::shared_ptr<DAGVertex> vertex_1 = std::make_shared<DAGVertex>("to");
    EXPECT_TRUE(edge.connectTo(vertex_1));
    EXPECT_TRUE(edge.isAConnectionTo(*vertex_1));

    std::shared_ptr<DAGVertex> vertex_2 = std::make_shared<DAGVertex>("ot");
    EXPECT_FALSE(edge.isAConnectionTo(*vertex_2));

    vertex_1 = std::make_shared<DAGVertex>("from");
    EXPECT_FALSE(edge.isAConnectionTo(*vertex_1));
}

TEST_F(TestDagEdge, copyCtor) {
    DAGEdge edge;
    DAGEdge e_copy(edge);

    EXPECT_EQ(DAGEdge::Status::INITIALIZED, edge.currentStatus());
    EXPECT_TRUE(edge.getUUID().isInitialized());
    EXPECT_EQ(nullptr, edge.getConnection().lock().get());

    EXPECT_EQ(DAGEdge::Status::INITIALIZED, e_copy.currentStatus());
    EXPECT_TRUE(e_copy.getUUID().isInitialized());
    EXPECT_EQ(nullptr, e_copy.getConnection().lock().get());

    EXPECT_EQ(edge, e_copy);

    std::shared_ptr<DAGVertex> vertex_1 = std::make_shared<DAGVertex>("to");
    EXPECT_TRUE(edge.connectTo(vertex_1));

    EXPECT_NE(edge, e_copy);

    DAGEdge e_copy_post_connect(edge);
    std::shared_ptr<DAGVertex> v1_clone =
            std::make_shared<DAGVertex>(vertex_1->clone());
    EXPECT_NE(edge, e_copy_post_connect);
    e_copy_post_connect.connectTo(v1_clone);
    EXPECT_EQ(edge, e_copy_post_connect);
}

TEST_F(TestDagEdge, assignmentOperator) {
    DAGEdge edge;
    DAGEdge e_copy;
    e_copy = edge;

    EXPECT_EQ(DAGEdge::Status::INITIALIZED, edge.currentStatus());
    EXPECT_TRUE(edge.getUUID().isInitialized());
    EXPECT_EQ(nullptr, edge.getConnection().lock().get());

    EXPECT_EQ(DAGEdge::Status::INITIALIZED, e_copy.currentStatus());
    EXPECT_TRUE(e_copy.getUUID().isInitialized());
    EXPECT_EQ(nullptr, e_copy.getConnection().lock().get());

    EXPECT_EQ(edge, e_copy);

    std::shared_ptr<DAGVertex> vertex_1 = std::make_shared<DAGVertex>("to");
    EXPECT_TRUE(edge.connectTo(vertex_1));

    EXPECT_NE(edge, e_copy);

    DAGEdge e_copy_post_connect;
    e_copy_post_connect = edge;
    std::shared_ptr<DAGVertex> v1_clone =
            std::make_shared<DAGVertex>(vertex_1->clone());
    EXPECT_NE(edge, e_copy_post_connect);
    e_copy_post_connect.connectTo(v1_clone);
    EXPECT_EQ(edge, e_copy_post_connect);
}

TEST_F(TestDagEdge, equalityOperatorsNoConnection) {
    DAGEdge edge;
    DAGEdge e_copy = edge.clone();

    EXPECT_EQ(edge, e_copy);
    EXPECT_EQ(e_copy, edge);
    EXPECT_FALSE(edge != e_copy);
    EXPECT_FALSE(e_copy != edge);
}

TEST_F(TestDagEdge, equalityOperatorsWithConnection) {
    DAGEdge edge;
    DAGEdge e_clone = edge.clone();
    std::shared_ptr<DAGVertex> vertex_1 = std::make_shared<DAGVertex>("to");
    std::shared_ptr<DAGVertex> v1_clone =
            std::make_shared<DAGVertex>(vertex_1->clone());
    edge.connectTo(vertex_1);
    e_clone.connectTo(v1_clone);

    EXPECT_EQ(edge, e_clone);
    EXPECT_EQ(e_clone, edge);
    EXPECT_FALSE(edge != e_clone);
    EXPECT_FALSE(e_clone != edge);
}
}  // namespace com::dag_scheduler
