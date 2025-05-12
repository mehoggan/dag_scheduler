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

TEST_F(TestDagVertex, stringCtor) {
    DAGVertex vertex("1");
    EXPECT_EQ(DAGVertex::Status::INITIALIZED, vertex.currentStatus());
    EXPECT_EQ(std::string("initialized"), vertex.currentStatusAsString());
    EXPECT_EQ("1", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_FALSE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
    EXPECT_EQ(nullptr, vertex.getTask());
}

TEST_F(TestDagVertex, stringTaskCtor) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->getUUID().asString();
    DAGVertex vertex("1", std::move(task));
    EXPECT_EQ(DAGVertex::Status::INITIALIZED, vertex.currentStatus());
    EXPECT_EQ(std::string("initialized"), vertex.currentStatusAsString());
    EXPECT_EQ("1", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_FALSE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
    EXPECT_EQ("1 Task", vertex.getTask()->label());
    EXPECT_EQ(task_uuid, vertex.getTask()->getUUID().asString());
}

TEST_F(TestDagVertex, stringTaskUUIDCtor) {
    class UUID user_generated_uuid;
    const std::string user_generated_uuid_str = user_generated_uuid.asString();
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->getUUID().asString();
    DAGVertex vertex("1", std::move(task), std::move(user_generated_uuid));
    EXPECT_EQ(DAGVertex::Status::INITIALIZED, vertex.currentStatus());
    EXPECT_EQ(std::string("initialized"), vertex.currentStatusAsString());
    EXPECT_EQ("1", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_FALSE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
    EXPECT_EQ("1 Task", vertex.getTask()->label());
    EXPECT_EQ(task_uuid, vertex.getTask()->getUUID().asString());
}

TEST_F(TestDagVertex, dtorNoEdges) {
    DAGVertex vertex("1");
    vertex.~DAGVertex();
    EXPECT_EQ(DAGVertex::Status::INVALID, vertex.currentStatus());
    EXPECT_EQ(std::string("invalid"), vertex.currentStatusAsString());
    EXPECT_EQ("", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_TRUE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
}

TEST_F(TestDagVertex, moveCtorNoEdges) {
    DAGVertex vertex("1");
    DAGVertex v_moved = std::move(vertex);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_moved.currentStatusAsString());
    EXPECT_EQ("1", v_moved.label());
    EXPECT_EQ(0ul, v_moved.edgeCount());
    EXPECT_FALSE(v_moved.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_moved.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INVALID, vertex.currentStatus());  // NOLINT
    EXPECT_EQ(std::string("invalid"), vertex.currentStatusAsString());
    EXPECT_EQ("", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_TRUE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
    EXPECT_EQ(nullptr, v_moved.getTask());
}

TEST_F(TestDagVertex, assignmentMoveOperatorNoEdges) {
    DAGVertex vertex("1");
    DAGVertex v_moved;
    v_moved = std::move(vertex);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_moved.currentStatusAsString());
    EXPECT_EQ("1", v_moved.label());
    EXPECT_EQ(0ul, v_moved.edgeCount());
    EXPECT_FALSE(v_moved.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_moved.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INVALID, vertex.currentStatus());  // NOLINT
    EXPECT_EQ(std::string("invalid"), vertex.currentStatusAsString());
    EXPECT_EQ("", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_TRUE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
    EXPECT_EQ(nullptr, v_moved.getTask());
}

TEST_F(TestDagVertex, cloneNoEdges) {
    DAGVertex vertex("1");
    DAGVertex v_cloned = vertex.clone();

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_cloned.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_cloned.currentStatusAsString());
    EXPECT_EQ("1", v_cloned.label());
    EXPECT_EQ(0ul, v_cloned.edgeCount());
    EXPECT_FALSE(v_cloned.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_cloned.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, vertex.currentStatus());
    EXPECT_EQ(std::string("initialized"), vertex.currentStatusAsString());
    EXPECT_EQ("1", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_FALSE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
    EXPECT_EQ(nullptr, v_cloned.getTask());
}

TEST_F(TestDagVertex, copyCtorNoEdges) {
    DAGVertex vertex("1");
    DAGVertex v_copied(vertex);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_copied.currentStatusAsString());
    EXPECT_EQ("1", v_copied.label());
    EXPECT_EQ(0ul, v_copied.edgeCount());
    EXPECT_FALSE(v_copied.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_copied.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, vertex.currentStatus());
    EXPECT_EQ(std::string("initialized"), vertex.currentStatusAsString());
    EXPECT_EQ("1", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_FALSE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
    EXPECT_EQ(nullptr, v_copied.getTask());
}

TEST_F(TestDagVertex, assignmentOperatorNoEdges) {
    DAGVertex vertex("1");
    DAGVertex v_copied;
    v_copied = vertex;

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_copied.currentStatusAsString());
    EXPECT_EQ("1", v_copied.label());
    EXPECT_EQ(0ul, v_copied.edgeCount());
    EXPECT_FALSE(v_copied.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_copied.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, vertex.currentStatus());
    EXPECT_EQ(std::string("initialized"), vertex.currentStatusAsString());
    EXPECT_EQ("1", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_FALSE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
    EXPECT_EQ(nullptr, v_copied.getTask());
}

TEST_F(TestDagVertex, moveCtorNoEdgesWithTask) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->getUUID().asString();
    DAGVertex vertex("1", std::move(task));
    DAGVertex v_moved = std::move(vertex);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_moved.currentStatusAsString());
    EXPECT_EQ("1", v_moved.label());
    EXPECT_EQ(0ul, v_moved.edgeCount());
    EXPECT_FALSE(v_moved.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_moved.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INVALID, vertex.currentStatus());  // NOLINT
    EXPECT_EQ(std::string("invalid"), vertex.currentStatusAsString());
    EXPECT_EQ("", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_TRUE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
    EXPECT_EQ("1 Task", v_moved.getTask()->label());
    EXPECT_EQ(task_uuid, v_moved.getTask()->getUUID().asString());
}

TEST_F(TestDagVertex, assignmentMoveOperatorNoEdgesWithTask) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->getUUID().asString();
    DAGVertex vertex("1", std::move(task));
    DAGVertex v_moved;
    v_moved = std::move(vertex);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_moved.currentStatusAsString());
    EXPECT_EQ("1", v_moved.label());
    EXPECT_EQ(0ul, v_moved.edgeCount());
    EXPECT_FALSE(v_moved.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_moved.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INVALID, vertex.currentStatus());  // NOLINT
    EXPECT_EQ(std::string("invalid"), vertex.currentStatusAsString());
    EXPECT_EQ("", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_TRUE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
    EXPECT_EQ("1 Task", v_moved.getTask()->label());
    EXPECT_EQ(task_uuid, v_moved.getTask()->getUUID().asString());
}

TEST_F(TestDagVertex, cloneNoEdgesWithTask) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->getUUID().asString();
    DAGVertex vertex("1", std::move(task));
    DAGVertex v_cloned = vertex.clone();

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_cloned.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_cloned.currentStatusAsString());
    EXPECT_EQ("1", v_cloned.label());
    EXPECT_EQ(0ul, v_cloned.edgeCount());
    EXPECT_FALSE(v_cloned.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_cloned.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, vertex.currentStatus());
    EXPECT_EQ(std::string("initialized"), vertex.currentStatusAsString());
    EXPECT_EQ("1", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_FALSE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
    EXPECT_NE(nullptr, v_cloned.getTask());
    EXPECT_EQ(v_cloned, vertex);
}

TEST_F(TestDagVertex, copyCtorNoEdgesWithTask) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->getUUID().asString();
    DAGVertex vertex("1", std::move(task));
    DAGVertex v_copied(vertex);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_copied.currentStatusAsString());
    EXPECT_EQ("1", v_copied.label());
    EXPECT_EQ(0ul, v_copied.edgeCount());
    EXPECT_FALSE(v_copied.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_copied.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, vertex.currentStatus());
    EXPECT_EQ(std::string("initialized"), vertex.currentStatusAsString());
    EXPECT_EQ("1", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_FALSE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
    EXPECT_NE(nullptr, v_copied.getTask());
    EXPECT_EQ(v_copied, vertex);
}

TEST_F(TestDagVertex, assignmentOperatorNoEdgesWithTask) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->getUUID().asString();
    DAGVertex vertex("1", std::move(task));
    DAGVertex v_copied;
    v_copied = vertex;

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_copied.currentStatusAsString());
    EXPECT_EQ("1", v_copied.label());
    EXPECT_EQ(0ul, v_copied.edgeCount());
    EXPECT_FALSE(v_copied.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_copied.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, vertex.currentStatus());
    EXPECT_EQ(std::string("initialized"), vertex.currentStatusAsString());
    EXPECT_EQ("1", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_FALSE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
    EXPECT_NE(nullptr, v_copied.getTask());
    EXPECT_EQ(v_copied, vertex);
}

TEST_F(TestDagVertex, connectAndContainsConnection) {
    {
        std::shared_ptr<DAGVertex> vertex = std::make_shared<DAGVertex>("2");
        EXPECT_TRUE(getDAGVertex().connect(vertex));
        EXPECT_TRUE(getDAGVertex().containsConnectionTo(*vertex));
        EXPECT_EQ(1ul, getDAGVertex().edgeCount());
    }
    {
        std::shared_ptr<DAGVertex> vertex = std::make_shared<DAGVertex>("2");
        EXPECT_TRUE(getDAGVertex().connect(vertex));
        EXPECT_TRUE(getDAGVertex().containsConnectionTo(*vertex));
        EXPECT_EQ(2ul, getDAGVertex().edgeCount());
    }
    {
        std::shared_ptr<DAGVertex> vertex = std::make_shared<DAGVertex>("3");
        EXPECT_TRUE(getDAGVertex().connect(vertex));
        EXPECT_TRUE(getDAGVertex().containsConnectionTo(*vertex));
        EXPECT_EQ(3ul, getDAGVertex().edgeCount());
    }
    {
        std::shared_ptr<DAGVertex> duplicate =
                std::make_shared<DAGVertex>("dup");
        EXPECT_TRUE(getDAGVertex().connect(duplicate));
        EXPECT_TRUE(getDAGVertex().containsConnectionTo(*duplicate));
        EXPECT_EQ(4ul, getDAGVertex().edgeCount());
        EXPECT_FALSE(getDAGVertex().connect(duplicate));
        EXPECT_TRUE(getDAGVertex().containsConnectionTo(*duplicate));
        EXPECT_EQ(4ul, getDAGVertex().edgeCount());
    }
    getDAGVertex().clearEdges();
    ASSERT_EQ(0ul, getDAGVertex().edgeCount());
}

TEST_F(TestDagVertex, visitAllEdgesPointsToActualVertices) {
    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& vertex : connections) {
        getDAGVertex().connect(vertex);
    }
    EXPECT_EQ(connections.size(), getDAGVertex().edgeCount());

    std::size_t index = 0;
    getDAGVertex().visitAllEdges([&](const DAGEdge& edge) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&edge);
        EXPECT_EQ(connections[index], e_ref.getConnection().lock());
        EXPECT_EQ(*connections[index], *(e_ref.getConnection().lock()));
        ++index;
    });
    EXPECT_EQ(getDAGVertex().edgeCount(), index);

    getDAGVertex().clearEdges();
    ASSERT_EQ(0ul, getDAGVertex().edgeCount());
}

TEST_F(TestDagVertex, cloneAllEdgesAndReestablishConnections) {
    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& vertex : connections) {
        getDAGVertex().connect(vertex);
    }
    EXPECT_EQ(connections.size(), getDAGVertex().edgeCount());

    std::vector<DAGVertex::DAGVertexConnection> edges =
            getDAGVertex().cloneAllConnections();
    // First vertex connects to itself.
    EXPECT_EQ(connections.size(), edges.size());

    DAGVertex tmp_vertex = getDAGVertex().clone();
    std::vector<std::shared_ptr<DAGVertex>> connections_cloned =
            tmp_vertex.reestablishConnections(edges);
    EXPECT_EQ(getDAGVertex(), tmp_vertex);

    std::size_t index = 0;
    getDAGVertex().visitAllEdges([&](const DAGEdge& edge) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&edge);
        std::size_t j_index = 0;
        tmp_vertex.visitAllEdges([&](const DAGEdge& e_clone) {
            if (j_index == index) {
                DAGEdge& e_ref_clone = *const_cast<DAGEdge*>(&e_clone);
                EXPECT_EQ(*(e_ref.getConnection().lock()),
                          *(e_ref_clone.getConnection().lock()));
            }
            ++j_index;
        });
        ++index;
    });

    getDAGVertex().clearEdges();
    ASSERT_EQ(0ul, getDAGVertex().edgeCount());
}

TEST_F(TestDagVertex, hasIncomingEdgeAndIncomingEdgeCount) {
    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    std::size_t count = 0;
    for (std::shared_ptr<DAGVertex>& vertex : connections) {
        EXPECT_TRUE((*vertex).connect(getDAGVertexPtr()));
        ++count;
        EXPECT_EQ(count, getDAGVertexPtr()->incomingEdgeCount());
        EXPECT_TRUE(getDAGVertexPtr()->hasIncomingEdges());

        EXPECT_EQ(1ul, vertex->edgeCount());
        vertex->visitAllEdges([&](const DAGEdge& edge) {
            DAGEdge& e_ref = *const_cast<DAGEdge*>(&edge);
            EXPECT_EQ(e_ref.getConnection().lock(), getDAGVertexPtr());
            EXPECT_EQ(*(e_ref.getConnection().lock()), *(getDAGVertexPtr()));
        });
    }
    EXPECT_EQ(0ul, getDAGVertexPtr()->edgeCount());
    EXPECT_EQ(count, getDAGVertexPtr()->incomingEdgeCount());
}

TEST_F(TestDagVertex, moveCtorWithEdges) {
    DAGVertex vertex("orig");

    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& connection : connections) {
        vertex.connect(connection);
    }

    DAGVertex v_moved = std::move(vertex);

    std::size_t index = 0;
    vertex.visitAllEdges([&](const DAGEdge& edge) {  // NOLINT
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&edge);
        EXPECT_NE(e_ref.getConnection().lock(), connections[index]);
        EXPECT_NE(*(e_ref.getConnection().lock()), *(connections[index]));
        ++index;
    });

    index = 0;
    v_moved.visitAllEdges([&](const DAGEdge& edge) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&edge);
        EXPECT_EQ(e_ref.getConnection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.getConnection().lock()), *(connections[index]));
        ++index;
    });

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_moved.currentStatusAsString());
    EXPECT_EQ("orig", v_moved.label());
    EXPECT_EQ(connections.size(), v_moved.edgeCount());
    EXPECT_FALSE(v_moved.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_moved.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INVALID, vertex.currentStatus());
    EXPECT_EQ(std::string("invalid"), vertex.currentStatusAsString());
    EXPECT_EQ("", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_TRUE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());

    vertex.clearEdges();
    EXPECT_EQ(0ul, vertex.edgeCount());

    v_moved.clearEdges();
    EXPECT_EQ(0ul, v_moved.edgeCount());
}

TEST_F(TestDagVertex, assignmentMoveOperatorWithEdges) {
    DAGVertex vertex("orig");
    DAGVertex v_moved;

    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& connection : connections) {
        vertex.connect(connection);
    }

    v_moved = std::move(vertex);

    std::size_t index = 0;
    vertex.visitAllEdges([&](const DAGEdge& edge) {  // NOLINT
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&edge);
        EXPECT_NE(e_ref.getConnection().lock(), connections[index]);
        EXPECT_NE(*(e_ref.getConnection().lock()), *(connections[index]));
        ++index;
    });

    index = 0;
    v_moved.visitAllEdges([&](const DAGEdge& edge) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&edge);
        EXPECT_EQ(e_ref.getConnection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.getConnection().lock()), *(connections[index]));
        ++index;
    });

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_moved.currentStatusAsString());
    EXPECT_EQ("orig", v_moved.label());
    EXPECT_EQ(connections.size(), v_moved.edgeCount());
    EXPECT_FALSE(v_moved.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_moved.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INVALID, vertex.currentStatus());
    EXPECT_EQ(std::string("invalid"), vertex.currentStatusAsString());
    EXPECT_EQ("", vertex.label());
    EXPECT_EQ(0ul, vertex.edgeCount());
    EXPECT_TRUE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());

    vertex.clearEdges();
    EXPECT_EQ(0ul, vertex.edgeCount());

    v_moved.clearEdges();
    EXPECT_EQ(0ul, v_moved.edgeCount());
}

TEST_F(TestDagVertex, cloneWithEdges) {
    DAGVertex vertex("orig");

    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& connection : connections) {
        vertex.connect(connection);
    }
    ASSERT_EQ(connections.size(), vertex.edgeCount());

    DAGVertex v_cloned = vertex.clone();
    ASSERT_EQ(0ul, v_cloned.edgeCount());

    // We cannot add back the connections since the edge adds a weak_ptr
    // to a DAGVertex we no longer can duplicate. This has to be done
    // outside the class by the code that is cloning the DAGVertex.
    // DAG_t should be the object that orchestrates that.
    std::vector<DAGVertex::DAGVertexConnection> orig_connections =
            vertex.cloneAllConnections();
    std::vector<std::shared_ptr<DAGVertex>> cloned_connections =
            v_cloned.reestablishConnections(orig_connections);

    std::size_t v_edge_count = vertex.edgeCount();
    std::size_t v_cloned_edge_count = v_cloned.edgeCount();
    ASSERT_EQ(v_edge_count, v_cloned_edge_count);

    ASSERT_EQ(vertex, v_cloned);
    ASSERT_EQ(v_cloned, vertex);

    std::size_t index = 0;
    vertex.visitAllEdges([&](const DAGEdge& edge) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&edge);
        EXPECT_EQ(e_ref.getConnection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.getConnection().lock()), *(connections[index]));
        ++index;
    });

    index = 0;
    v_cloned.visitAllEdges([&](const DAGEdge& edge) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&edge);
        EXPECT_NE(e_ref.getConnection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.getConnection().lock()), *(connections[index]));
        ++index;
        (void)e_ref;
    });

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_cloned.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_cloned.currentStatusAsString());
    EXPECT_EQ("orig", v_cloned.label());
    EXPECT_EQ(connections.size(), v_cloned.edgeCount());
    EXPECT_FALSE(v_cloned.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_cloned.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, vertex.currentStatus());
    EXPECT_EQ(std::string("initialized"), vertex.currentStatusAsString());
    EXPECT_EQ("orig", vertex.label());
    EXPECT_EQ(connections.size(), vertex.edgeCount());
    EXPECT_FALSE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
}

TEST_F(TestDagVertex, addIncomingEdge) {
    DAGVertex vertex("orig");
    vertex.addIncomingEdge();
    EXPECT_EQ(1, vertex.incomingEdgeCount());
}

TEST_F(TestDagVertex, subIncomingEdge) {
    DAGVertex vertex("orig");
    vertex.addIncomingEdge();
    vertex.addIncomingEdge();
    vertex.subIncomingEdge();
    EXPECT_EQ(1, vertex.incomingEdgeCount());
}

TEST_F(TestDagVertex, clearEdges) {
    DAGVertex vertex("orig");
    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& connection : connections) {
        vertex.connect(connection);
    }
    EXPECT_EQ(6, vertex.edgeCount());
    vertex.clearEdges();
    EXPECT_EQ(0, vertex.edgeCount());
}

TEST_F(TestDagVertex, resetIncomingEdgeCount) {
    DAGVertex vertex("orig");
    vertex.addIncomingEdge();
    vertex.addIncomingEdge();
    vertex.addIncomingEdge();
    vertex.addIncomingEdge();
    vertex.addIncomingEdge();
    EXPECT_EQ(5, vertex.incomingEdgeCount());
    vertex.resetIncomingEdgeCount();
    EXPECT_EQ(0, vertex.incomingEdgeCount());
}

TEST_F(TestDagVertex, getEdgeAt) {
    DAGVertex vertex("orig");
    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& connection : connections) {
        vertex.connect(connection);
    }

    // Make use_count the same.
    connections.clear();

    std::size_t index = 0;
    vertex.visitAllEdges([&](const DAGEdge& edge) {
        EXPECT_EQ(edge, vertex.getEdgeAt(index));
        ++index;
    });
}

TEST_F(TestDagVertex, copyCtorWithEdges) {
    DAGVertex vertex("orig");

    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& connection : connections) {
        vertex.connect(connection);
    }
    ASSERT_EQ(connections.size(), vertex.edgeCount());

    DAGVertex v_copied(vertex.clone());
    ASSERT_EQ(0ul, v_copied.edgeCount());

    // We cannot add back the connections since the edge adds a weak_ptr
    // to a DAGVertex we no longer can duplicate. This has to be done
    // outside the class by the code that is cloning the DAGVertex.
    // DAG_t should be the object that orchestrates that.
    std::vector<DAGVertex::DAGVertexConnection> orig_connections =
            vertex.cloneAllConnections();
    std::vector<std::shared_ptr<DAGVertex>> copied_connections =
            v_copied.reestablishConnections(orig_connections);

    std::size_t v_edge_count = vertex.edgeCount();
    std::size_t v_copied_edge_count = v_copied.edgeCount();
    ASSERT_EQ(v_edge_count, v_copied_edge_count);

    ASSERT_EQ(vertex, v_copied);
    ASSERT_EQ(v_copied, vertex);

    std::size_t index = 0;
    vertex.visitAllEdges([&](const DAGEdge& edge) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&edge);
        EXPECT_EQ(e_ref.getConnection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.getConnection().lock()), *(connections[index]));
        ++index;
    });

    index = 0;
    v_copied.visitAllEdges([&](const DAGEdge& edge) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&edge);
        EXPECT_NE(e_ref.getConnection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.getConnection().lock()), *(connections[index]));
        ++index;
        (void)e_ref;
    });

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_copied.currentStatusAsString());
    EXPECT_EQ("orig", v_copied.label());
    EXPECT_EQ(connections.size(), v_copied.edgeCount());
    EXPECT_FALSE(v_copied.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_copied.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, vertex.currentStatus());
    EXPECT_EQ(std::string("initialized"), vertex.currentStatusAsString());
    EXPECT_EQ("orig", vertex.label());
    EXPECT_EQ(connections.size(), vertex.edgeCount());
    EXPECT_FALSE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
}

TEST_F(TestDagVertex, assignmentOperatorWithEdges) {
    DAGVertex vertex("orig");

    std::vector<std::shared_ptr<DAGVertex>> connections = {
            std::make_shared<DAGVertex>("1"),
            std::make_shared<DAGVertex>("2"),
            std::make_shared<DAGVertex>("3"),
            std::make_shared<DAGVertex>("4"),
            std::make_shared<DAGVertex>("5"),
            std::make_shared<DAGVertex>("6"),
    };

    for (std::shared_ptr<DAGVertex>& connection : connections) {
        vertex.connect(connection);
    }
    ASSERT_EQ(connections.size(), vertex.edgeCount());

    DAGVertex v_copied;
    v_copied = vertex.clone();
    ASSERT_EQ(0ul, v_copied.edgeCount());

    // We cannot add back the connections since the edge adds a weak_ptr
    // to a DAGVertex we no longer can duplicate. This has to be done
    // outside the class by the code that is cloning the DAGVertex. DAG_t
    // should be the object that orchestrates that.
    std::vector<DAGVertex::DAGVertexConnection> orig_connections =
            vertex.cloneAllConnections();
    std::vector<std::shared_ptr<DAGVertex>> copied_connections =
            v_copied.reestablishConnections(orig_connections);

    std::size_t v_edge_count = vertex.edgeCount();
    std::size_t v_copied_edge_count = v_copied.edgeCount();
    ASSERT_EQ(v_edge_count, v_copied_edge_count);

    ASSERT_EQ(vertex, v_copied);
    ASSERT_EQ(v_copied, vertex);

    std::size_t index = 0;
    vertex.visitAllEdges([&](const DAGEdge& edge) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&edge);
        EXPECT_EQ(e_ref.getConnection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.getConnection().lock()), *(connections[index]));
        ++index;
    });

    index = 0;
    v_copied.visitAllEdges([&](const DAGEdge& edge) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&edge);
        EXPECT_NE(e_ref.getConnection().lock(), connections[index]);
        EXPECT_EQ(*(e_ref.getConnection().lock()), *(connections[index]));
        ++index;
        (void)e_ref;
    });

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.currentStatus());
    EXPECT_EQ(std::string("initialized"), v_copied.currentStatusAsString());
    EXPECT_EQ("orig", v_copied.label());
    EXPECT_EQ(connections.size(), v_copied.edgeCount());
    EXPECT_FALSE(v_copied.getUUID().asString().empty());
    EXPECT_EQ(0ul, v_copied.incomingEdgeCount());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, vertex.currentStatus());
    EXPECT_EQ(std::string("initialized"), vertex.currentStatusAsString());
    EXPECT_EQ("orig", vertex.label());
    EXPECT_EQ(connections.size(), vertex.edgeCount());
    EXPECT_FALSE(vertex.getUUID().asString().empty());
    EXPECT_EQ(0ul, vertex.incomingEdgeCount());
}
}  // namespace com::dag_scheduler
