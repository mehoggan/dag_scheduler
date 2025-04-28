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
class TestDagVertex : public ::testing::Test {
protected:
    void SetUp() override {
        v_ptr_ = std::make_shared<DAGVertex>("TestDagVertexPtr");
        v_ = DAGVertex("TestDagVertex");
    }

    void TearDown() override { v_ptr_.reset(); }

    DAGVertex& getDAGVertex() { return v_; }

    std::shared_ptr<DAGVertex> getDAGVertexPtr() { return v_ptr_; }

private:
    std::shared_ptr<DAGVertex> v_ptr_;
    DAGVertex v_;
};

TEST_F(TestDagVertex, ctor) {
    DAGVertex vertex;
    EXPECT_EQ(DAGVertex::Status::INITIALIZED, vertex.currentStatus());
    EXPECT_EQ(std::string("initialized"), vertex.currentStatusAsString());
    EXPECT_EQ(vertex.getUUID().asString(), vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_FALSE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
    EXPECT_EQ(nullptr, vertex.getTask());
}

TEST_F(TestDagVertex, string_ctor) {
    DAGVertex v("1");
    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.currentStatus());
    EXPECT_EQ(std::string("initialized"), v.currentStatusAsString());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_FALSE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
    EXPECT_EQ(nullptr, v.getTask());
}

TEST_F(TestDagVertex, string_task_ctor) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->getUUID().asString();
    DAGVertex v("1", std::move(task));
    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.currentStatus());
    EXPECT_EQ(std::string("initialized"), v.currentStatusAsString());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_FALSE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
    EXPECT_EQ("1 Task", v.getTask()->label());
    EXPECT_EQ(task_uuid, v.getTask()->getUUID().asString());
}

TEST_F(TestDagVertex, string_task_uuid_ctor) {
    class UUID user_generated_uuid;
    const std::string user_generated_uuid_str = user_generated_uuid.asString();
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->getUUID().asString();
    DAGVertex v("1", std::move(task), std::move(user_generated_uuid));
    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.currentStatus());
    EXPECT_EQ(std::string("initialized"), v.currentStatusAsString());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_FALSE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
    EXPECT_EQ("1 Task", v.getTask()->label());
    EXPECT_EQ(task_uuid, v.getTask()->getUUID().asString());
}

TEST_F(TestDagVertex, dtor_no_edges) {
    DAGVertex v("1");
    v.~DAGVertex();
    EXPECT_EQ(DAGVertex::Status::INVALID, v.currentStatus());
    EXPECT_EQ(std::string("invalid"), v.currentStatusAsString());
    EXPECT_EQ("", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_TRUE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
}

TEST_F(TestDagVertex, move_ctor_no_edges) {
    DAGVertex v("1");
    DAGVertex v_moved = std::move(v);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_moved.currentStatusAsString());
    EXPECT_EQ("1", v_moved.label());
    EXPECT_EQ(0ul, v_moved.edgeCount());
    EXPECT_FALSE(v_moved.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_moved.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INVALID, v.currentStatus());
    EXPECT_EQ(std::string("invalid"), v.currentStatusAsString());
    EXPECT_EQ("", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_TRUE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
    EXPECT_EQ(nullptr, v_moved.getTask());
}

TEST_F(TestDagVertex, assignment_move_operator_no_edges) {
    DAGVertex v("1");
    DAGVertex v_moved;
    v_moved = std::move(v);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_moved.currentStatusAsString());
    EXPECT_EQ("1", v_moved.label());
    EXPECT_EQ(0ul, v_moved.edgeCount());
    EXPECT_FALSE(v_moved.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_moved.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INVALID, v.currentStatus());
    EXPECT_EQ(std::string("invalid"), v.currentStatusAsString());
    EXPECT_EQ("", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_TRUE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
    EXPECT_EQ(nullptr, v_moved.getTask());
}

TEST_F(TestDagVertex, clone_no_edges) {
    DAGVertex v("1");
    DAGVertex v_cloned = v.clone();

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_cloned.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_cloned.currentStatusAsString());
    EXPECT_EQ("1", v_cloned.label());
    EXPECT_EQ(0ul, v_cloned.edgeCount());
    EXPECT_FALSE(v_cloned.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_cloned.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.currentStatus());
    EXPECT_EQ(std::string("initialized"), v.currentStatusAsString());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_FALSE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
    EXPECT_EQ(nullptr, v_cloned.getTask());
}

TEST_F(TestDagVertex, copy_ctor_no_edges) {
    DAGVertex v("1");
    DAGVertex v_copied(v);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_copied.currentStatusAsString());
    EXPECT_EQ("1", v_copied.label());
    EXPECT_EQ(0ul, v_copied.edgeCount());
    EXPECT_FALSE(v_copied.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_copied.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.currentStatus());
    EXPECT_EQ(std::string("initialized"), v.currentStatusAsString());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_FALSE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
    EXPECT_EQ(nullptr, v_copied.getTask());
}

TEST_F(TestDagVertex, assignment_operator_no_edges) {
    DAGVertex v("1");
    DAGVertex v_copied;
    v_copied = v;

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_copied.currentStatusAsString());
    EXPECT_EQ("1", v_copied.label());
    EXPECT_EQ(0ul, v_copied.edgeCount());
    EXPECT_FALSE(v_copied.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_copied.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.currentStatus());
    EXPECT_EQ(std::string("initialized"), v.currentStatusAsString());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_FALSE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
    EXPECT_EQ(nullptr, v_copied.getTask());
}

TEST_F(TestDagVertex, move_ctor_no_edges_with_task) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->getUUID().asString();
    DAGVertex v("1", std::move(task));
    DAGVertex v_moved = std::move(v);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_moved.currentStatusAsString());
    EXPECT_EQ("1", v_moved.label());
    EXPECT_EQ(0ul, v_moved.edgeCount());
    EXPECT_FALSE(v_moved.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_moved.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INVALID, v.currentStatus());
    EXPECT_EQ(std::string("invalid"), v.currentStatusAsString());
    EXPECT_EQ("", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_TRUE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
    EXPECT_EQ("1 Task", v_moved.getTask()->label());
    EXPECT_EQ(task_uuid, v_moved.getTask()->getUUID().asString());
}

TEST_F(TestDagVertex, assignment_move_operator_no_edges_with_task) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->getUUID().asString();
    DAGVertex v("1", std::move(task));
    DAGVertex v_moved;
    v_moved = std::move(v);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_moved.currentStatusAsString());
    EXPECT_EQ("1", v_moved.label());
    EXPECT_EQ(0ul, v_moved.edgeCount());
    EXPECT_FALSE(v_moved.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_moved.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INVALID, v.currentStatus());
    EXPECT_EQ(std::string("invalid"), v.currentStatusAsString());
    EXPECT_EQ("", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_TRUE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
    EXPECT_EQ("1 Task", v_moved.getTask()->label());
    EXPECT_EQ(task_uuid, v_moved.getTask()->getUUID().asString());
}

TEST_F(TestDagVertex, clone_no_edges_with_task) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->getUUID().asString();
    DAGVertex v("1", std::move(task));
    DAGVertex v_cloned = v.clone();

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_cloned.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_cloned.currentStatusAsString());
    EXPECT_EQ("1", v_cloned.label());
    EXPECT_EQ(0ul, v_cloned.edgeCount());
    EXPECT_FALSE(v_cloned.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_cloned.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.currentStatus());
    EXPECT_EQ(std::string("initialized"), v.currentStatusAsString());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_FALSE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
    EXPECT_NE(nullptr, v_cloned.getTask());
    EXPECT_EQ(v_cloned, v);
}

TEST_F(TestDagVertex, copy_ctor_no_edges_with_task) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->getUUID().asString();
    DAGVertex v("1", std::move(task));
    DAGVertex v_copied(v);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_copied.currentStatusAsString());
    EXPECT_EQ("1", v_copied.label());
    EXPECT_EQ(0ul, v_copied.edgeCount());
    EXPECT_FALSE(v_copied.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_copied.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.currentStatus());
    EXPECT_EQ(std::string("initialized"), v.currentStatusAsString());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_FALSE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
    EXPECT_NE(nullptr, v_copied.getTask());
    EXPECT_EQ(v_copied, v);
}

TEST_F(TestDagVertex, assignment_operator_no_edges_with_task) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->getUUID().asString();
    DAGVertex v("1", std::move(task));
    DAGVertex v_copied;
    v_copied = v;

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_copied.currentStatusAsString());
    EXPECT_EQ("1", v_copied.label());
    EXPECT_EQ(0ul, v_copied.edgeCount());
    EXPECT_FALSE(v_copied.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_copied.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.currentStatus());
    EXPECT_EQ(std::string("initialized"), v.currentStatusAsString());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_FALSE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
    EXPECT_NE(nullptr, v_copied.getTask());
    EXPECT_EQ(v_copied, v);
}

TEST_F(TestDagVertex, connect_and_contains_connection) {
    {
        std::shared_ptr<DAGVertex> v = std::make_shared<DAGVertex>("2");
        EXPECT_TRUE(getDAGVertex().connect(v));
        EXPECT_TRUE(getDAGVertex().containsConnectionTo(*v));
        EXPECT_EQ(1ul, getDAGVertex().edgeCount());
    }
    {
        std::shared_ptr<DAGVertex> v = std::make_shared<DAGVertex>("2");
        EXPECT_TRUE(getDAGVertex().connect(v));
        EXPECT_TRUE(getDAGVertex().containsConnectionTo(*v));
        EXPECT_EQ(2ul, getDAGVertex().edgeCount());
    }
    {
        std::shared_ptr<DAGVertex> v = std::make_shared<DAGVertex>("3");
        EXPECT_TRUE(getDAGVertex().connect(v));
        EXPECT_TRUE(getDAGVertex().containsConnectionTo(*v));
        EXPECT_EQ(3ul, getDAGVertex().edgeCount());
    }
    {
        std::shared_ptr<DAGVertex> dup = std::make_shared<DAGVertex>("dup");
        EXPECT_TRUE(getDAGVertex().connect(dup));
        EXPECT_TRUE(getDAGVertex().containsConnectionTo(*dup));
        EXPECT_EQ(4ul, getDAGVertex().edgeCount());
        EXPECT_FALSE(getDAGVertex().connect(dup));
        EXPECT_TRUE(getDAGVertex().containsConnectionTo(*dup));
        EXPECT_EQ(4ul, getDAGVertex().edgeCount());
    }
    getDAGVertex().clearEdges();
    ASSERT_EQ(0ul, getDAGVertex().edgeCount());
}

TEST_F(TestDagVertex, visit_all_edges_points_to_actual_vertices) {
    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& v : connections) {
        getDAGVertex().connect(v);
    }
    EXPECT_EQ(connections.size(), getDAGVertex().edgeCount());

    std::size_t index = 0;
    getDAGVertex().visitAllEdges([&](const DAGEdge& e) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
        EXPECT_EQ(connections[index], e_ref.getConnection().lock());
        EXPECT_EQ(*connections[index], *(e_ref.getConnection().lock()));
        ++index;
    });
    EXPECT_EQ(getDAGVertex().edgeCount(), index);

    getDAGVertex().clearEdges();
    ASSERT_EQ(0ul, getDAGVertex().edgeCount());
}

TEST_F(TestDagVertex, clone_all_edges_and_reestablish_connections) {
    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& v : connections) {
        getDAGVertex().connect(v);
    }
    EXPECT_EQ(connections.size(), getDAGVertex().edgeCount());

    std::vector<DAGVertex::DAGVertexConnection> edges =
            getDAGVertex().cloneAllConnections();
    // First vertex connects to itself.
    EXPECT_EQ(connections.size(), edges.size());

    DAGVertex tmp = getDAGVertex().clone();
    std::vector<std::shared_ptr<DAGVertex>> connections_cloned =
            tmp.reestablish_connections(edges);
    EXPECT_EQ(getDAGVertex(), tmp);

    std::size_t index = 0;
    getDAGVertex().visit_all_edges([&](const DAGEdge& e) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
        std::size_t j = 0;
        tmp.visit_all_edges([&](const DAGEdge& e_clone) {
            if (j == index) {
                DAGEdge& e_ref_clone = *const_cast<DAGEdge*>(&e_clone);
                EXPECT_EQ(*(e_ref.get_connection().lock()),
                          *(e_ref_clone.get_connection().lock()));
            }
            ++j;
        });
        ++index;
    });

    getDAGVertex().clearEdges();
    ASSERT_EQ(0ul, getDAGVertex().edgeCount());
}

TEST_F(TestDagVertex, has_incoming_edge_and_incomingEdgeCount) {
    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    std::size_t count = 0;
    for (std::shared_ptr<DAGVertex>& v : connections) {
        EXPECT_TRUE((*v).connect(getDAGVertex_ptr()));
        ++count;
        EXPECT_EQ(count, getDAGVertex_ptr()->incomingEdgeCount());
        EXPECT_TRUE(getDAGVertex_ptr()->has_incoming_edges());

        EXPECT_EQ(1ul, v->edgeCount());
        v->visit_all_edges([&](const DAGEdge& e) {
            DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
            EXPECT_EQ(e_ref.get_connection().lock(), getDAGVertex_ptr());
            EXPECT_EQ(*(e_ref.get_connection().lock()), *(getDAGVertex_ptr()));
        });
    }
    EXPECT_EQ(0ul, getDAGVertex_ptr()->edgeCount());
    EXPECT_EQ(count, getDAGVertex_ptr()->incomingEdgeCount());
}

TEST_F(TestDagVertex, move_ctor_with_edges) {
    DAGVertex v("orig");

    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& c : connections) {
        v.connect(c);
    }

    DAGVertex v_moved = std::move(v);

    std::size_t index = 0;
    v.visit_all_edges([&](const DAGEdge& e) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
        EXPECT_NE(e_ref.get_connection().lock(), connections[index]);
        EXPECT_NE(*(e_ref.get_connection().lock()), *(connections[index]));
        ++index;
    });

    index = 0;
    v_moved.visit_all_edges([&](const DAGEdge& e) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
        EXPECT_EQ(e_ref.get_connection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
        ++index;
    });

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_moved.currentStatusAsString());
    EXPECT_EQ("orig", v_moved.label());
    EXPECT_EQ(connections.size(), v_moved.edgeCount());
    EXPECT_FALSE(v_moved.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_moved.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INVALID, v.currentStatus());
    EXPECT_EQ(std::string("invalid"), v.currentStatusAsString());
    EXPECT_EQ("", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_TRUE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());

    v.clearEdges();
    EXPECT_EQ(0ul, v.edgeCount());

    v_moved.clearEdges();
    EXPECT_EQ(0ul, v_moved.edgeCount());
}

TEST_F(TestDagVertex, assignment_move_operator_with_edges) {
    DAGVertex v("orig");
    DAGVertex v_moved;

    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& c : connections) {
        v.connect(c);
    }

    v_moved = std::move(v);

    std::size_t index = 0;
    v.visit_all_edges([&](const DAGEdge& e) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
        EXPECT_NE(e_ref.get_connection().lock(), connections[index]);
        EXPECT_NE(*(e_ref.get_connection().lock()), *(connections[index]));
        ++index;
    });

    index = 0;
    v_moved.visit_all_edges([&](const DAGEdge& e) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
        EXPECT_EQ(e_ref.get_connection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
        ++index;
    });

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_moved.currentStatusAsString());
    EXPECT_EQ("orig", v_moved.label());
    EXPECT_EQ(connections.size(), v_moved.edgeCount());
    EXPECT_FALSE(v_moved.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_moved.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INVALID, v.currentStatus());
    EXPECT_EQ(std::string("invalid"), v.currentStatusAsString());
    EXPECT_EQ("", v.label());
    EXPECT_EQ(0ul, v.edgeCount());
    EXPECT_TRUE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());

    v.clearEdges();
    EXPECT_EQ(0ul, v.edgeCount());

    v_moved.clearEdges();
    EXPECT_EQ(0ul, v_moved.edgeCount());
}

TEST_F(TestDagVertex, clone_with_edges) {
    DAGVertex v("orig");

    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& c : connections) {
        v.connect(c);
    }
    ASSERT_EQ(connections.size(), v.edgeCount());

    DAGVertex v_cloned = v.clone();
    ASSERT_EQ(0ul, v_cloned.edgeCount());

    // We cannot add back the connections since the edge adds a weak_ptr
    // to a DAGVertex we no longer can duplicate. This has to be done
    // outside the class by the code that is cloning the DAGVertex.
    // DAG_t should be the object that orchestrates that.
    std::vector<DAGVertex::DAGVertex_connection> orig_connections =
            v.clone_all_connections();
    std::vector<std::shared_ptr<DAGVertex>> cloned_connections =
            v_cloned.reestablish_connections(orig_connections);

    std::size_t v_edgeCount = v.edgeCount();
    std::size_t v_cloned_edgeCount = v_cloned.edgeCount();
    ASSERT_EQ(v_edgeCount, v_cloned_edgeCount);

    ASSERT_EQ(v, v_cloned);
    ASSERT_EQ(v_cloned, v);

    std::size_t index = 0;
    v.visit_all_edges([&](const DAGEdge& e) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
        EXPECT_EQ(e_ref.get_connection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
        ++index;
    });

    index = 0;
    v_cloned.visit_all_edges([&](const DAGEdge& e) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
        EXPECT_NE(e_ref.get_connection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
        ++index;
        (void)e_ref;
    });

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_cloned.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_cloned.currentStatusAsString());
    EXPECT_EQ("orig", v_cloned.label());
    EXPECT_EQ(connections.size(), v_cloned.edgeCount());
    EXPECT_FALSE(v_cloned.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_cloned.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.currentStatus());
    EXPECT_EQ(std::string("initialized"), v.currentStatusAsString());
    EXPECT_EQ("orig", v.label());
    EXPECT_EQ(connections.size(), v.edgeCount());
    EXPECT_FALSE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
}

TEST_F(TestDagVertex, add_incoming_edge) {
    DAGVertex v("orig");
    v.add_incoming_edge();
    EXPECT_EQ(1, v.incomingEdgeCount());
}

TEST_F(TestDagVertex, sub_incoming_edge) {
    DAGVertex v("orig");
    v.add_incoming_edge();
    v.add_incoming_edge();
    v.sub_incoming_edge();
    EXPECT_EQ(1, v.incomingEdgeCount());
}

TEST_F(TestDagVertex, clearEdges) {
    DAGVertex v("orig");
    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& c : connections) {
        v.connect(c);
    }
    EXPECT_EQ(6, v.edgeCount());
    v.clearEdges();
    EXPECT_EQ(0, v.edgeCount());
}

TEST_F(TestDagVertex, reset_incomingEdgeCount) {
    DAGVertex v("orig");
    v.add_incoming_edge();
    v.add_incoming_edge();
    v.add_incoming_edge();
    v.add_incoming_edge();
    v.add_incoming_edge();
    EXPECT_EQ(5, v.incomingEdgeCount());
    v.reset_incomingEdgeCount();
    EXPECT_EQ(0, v.incomingEdgeCount());
}

TEST_F(TestDagVertex, get_edge_at) {
    DAGVertex v("orig");
    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& c : connections) {
        v.connect(c);
    }

    // Make use_count the same.
    connections.clear();

    std::size_t index = 0;
    v.visit_all_edges([&](const DAGEdge& e) {
        EXPECT_EQ(e, v.get_edge_at(index));
        ++index;
    });
}

TEST_F(TestDagVertex, copy_ctor_with_edges) {
    DAGVertex v("orig");

    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& c : connections) {
        v.connect(c);
    }
    ASSERT_EQ(connections.size(), v.edgeCount());

    DAGVertex v_copied(v.clone());
    ASSERT_EQ(0ul, v_copied.edgeCount());

    // We cannot add back the connections since the edge adds a weak_ptr
    // to a DAGVertex we no longer can duplicate. This has to be done
    // outside the class by the code that is cloning the DAGVertex.
    // DAG_t should be the object that orchestrates that.
    std::vector<DAGVertex::DAGVertex_connection> orig_connections =
            v.clone_all_connections();
    std::vector<std::shared_ptr<DAGVertex>> copied_connections =
            v_copied.reestablish_connections(orig_connections);

    std::size_t v_edgeCount = v.edgeCount();
    std::size_t v_copied_edgeCount = v_copied.edgeCount();
    ASSERT_EQ(v_edgeCount, v_copied_edgeCount);

    ASSERT_EQ(v, v_copied);
    ASSERT_EQ(v_copied, v);

    std::size_t index = 0;
    v.visit_all_edges([&](const DAGEdge& e) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
        EXPECT_EQ(e_ref.get_connection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
        ++index;
    });

    index = 0;
    v_copied.visit_all_edges([&](const DAGEdge& e) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
        EXPECT_NE(e_ref.get_connection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
        ++index;
        (void)e_ref;
    });

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_copied.currentStatusAsString());
    EXPECT_EQ("orig", v_copied.label());
    EXPECT_EQ(connections.size(), v_copied.edgeCount());
    EXPECT_FALSE(v_copied.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_copied.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.currentStatus());
    EXPECT_EQ(std::string("initialized"), v.currentStatusAsString());
    EXPECT_EQ("orig", v.label());
    EXPECT_EQ(connections.size(), v.edgeCount());
    EXPECT_FALSE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
}

TEST_F(TestDagVertex, assignment_operator_with_edges) {
    DAGVertex v("orig");

    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& c : connections) {
        v.connect(c);
    }
    ASSERT_EQ(connections.size(), v.edgeCount());

    DAGVertex v_copied;
    v_copied = v.clone();
    ASSERT_EQ(0ul, v_copied.edgeCount());

    // We cannot add back the connections since the edge adds a weak_ptr
    // to a DAGVertex we no longer can duplicate. This has to be done
    // outside the class by the code that is cloning the DAGVertex. DAG_t
    // should be the object that orchestrates that.
    std::vector<DAGVertex::DAGVertex_connection> orig_connections =
            v.clone_all_connections();
    std::vector<std::shared_ptr<DAGVertex>> copied_connections =
            v_copied.reestablish_connections(orig_connections);

    std::size_t v_edgeCount = v.edgeCount();
    std::size_t v_copied_edgeCount = v_copied.edgeCount();
    ASSERT_EQ(v_edgeCount, v_copied_edgeCount);

    ASSERT_EQ(v, v_copied);
    ASSERT_EQ(v_copied, v);

    std::size_t index = 0;
    v.visit_all_edges([&](const DAGEdge& e) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
        EXPECT_EQ(e_ref.get_connection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
        ++index;
    });

    index = 0;
    v_copied.visit_all_edges([&](const DAGEdge& e) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
        EXPECT_NE(e_ref.get_connection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
        ++index;
        (void)e_ref;
    });

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_copied.currentStatusAsString());
    EXPECT_EQ("orig", v_copied.label());
    EXPECT_EQ(connections.size(), v_copied.edgeCount());
    EXPECT_FALSE(v_copied.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_copied.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.currentStatus());
    EXPECT_EQ(std::string("initialized"), v.currentStatusAsString());
    EXPECT_EQ("orig", v.label());
    EXPECT_EQ(connections.size(), v.edgeCount());
    EXPECT_FALSE(v.getUUID().asString().empty());
    EXPECT_EQ(0ul, v.incomingEdgeCount());
}
}  // namespace com::dag_scheduler
