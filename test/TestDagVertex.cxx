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

#include <iostream>
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

    DAGVertex& get_dag_vertex() { return v_; }

    std::shared_ptr<DAGVertex> get_dag_vertex_ptr() { return v_ptr_; }

private:
    std::shared_ptr<DAGVertex> v_ptr_;
    DAGVertex v_;
};

TEST_F(TestDagVertex, ctor) {
    DAGVertex v;
    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.current_status());
    EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
    EXPECT_EQ(v.get_uuid().as_string(), v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_FALSE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
    EXPECT_EQ(nullptr, v.get_task());
}

TEST_F(TestDagVertex, string_ctor) {
    DAGVertex v("1");
    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.current_status());
    EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_FALSE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
    EXPECT_EQ(nullptr, v.get_task());
}

TEST_F(TestDagVertex, string_task_ctor) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->get_uuid().as_string();
    DAGVertex v("1", std::move(task));
    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.current_status());
    EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_FALSE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
    EXPECT_EQ("1 Task", v.get_task()->label());
    EXPECT_EQ(task_uuid, v.get_task()->get_uuid().as_string());
}

TEST_F(TestDagVertex, string_task_uuid_ctor) {
    class UUID user_generated_uuid;
    const std::string user_generated_uuid_str = user_generated_uuid.as_string();
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->get_uuid().as_string();
    DAGVertex v("1", std::move(task), std::move(user_generated_uuid));
    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.current_status());
    EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_FALSE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
    EXPECT_EQ("1 Task", v.get_task()->label());
    EXPECT_EQ(task_uuid, v.get_task()->get_uuid().as_string());
}

TEST_F(TestDagVertex, dtor_no_edges) {
    DAGVertex v("1");
    v.~DAGVertex();
    EXPECT_EQ(DAGVertex::Status::INVALID, v.current_status());
    EXPECT_EQ(std::string("invalid"), v.current_status_as_string());
    EXPECT_EQ("", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_TRUE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
}

TEST_F(TestDagVertex, move_ctor_no_edges) {
    DAGVertex v("1");
    DAGVertex v_moved = std::move(v);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.current_status());
    EXPECT_EQ(std::string("initialized"), v_moved.current_status_as_string());
    EXPECT_EQ("1", v_moved.label());
    EXPECT_EQ(0ul, v_moved.edge_count());
    EXPECT_FALSE(v_moved.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_moved.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INVALID, v.current_status());
    EXPECT_EQ(std::string("invalid"), v.current_status_as_string());
    EXPECT_EQ("", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_TRUE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
    EXPECT_EQ(nullptr, v_moved.get_task());
}

TEST_F(TestDagVertex, assignment_move_operator_no_edges) {
    DAGVertex v("1");
    DAGVertex v_moved;
    v_moved = std::move(v);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.current_status());
    EXPECT_EQ(std::string("initialized"), v_moved.current_status_as_string());
    EXPECT_EQ("1", v_moved.label());
    EXPECT_EQ(0ul, v_moved.edge_count());
    EXPECT_FALSE(v_moved.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_moved.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INVALID, v.current_status());
    EXPECT_EQ(std::string("invalid"), v.current_status_as_string());
    EXPECT_EQ("", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_TRUE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
    EXPECT_EQ(nullptr, v_moved.get_task());
}

TEST_F(TestDagVertex, clone_no_edges) {
    DAGVertex v("1");
    DAGVertex v_cloned = v.clone();

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_cloned.current_status());
    EXPECT_EQ(std::string("initialized"), v_cloned.current_status_as_string());
    EXPECT_EQ("1", v_cloned.label());
    EXPECT_EQ(0ul, v_cloned.edge_count());
    EXPECT_FALSE(v_cloned.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_cloned.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.current_status());
    EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_FALSE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
    EXPECT_EQ(nullptr, v_cloned.get_task());
}

TEST_F(TestDagVertex, copy_ctor_no_edges) {
    DAGVertex v("1");
    DAGVertex v_copied(v);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.current_status());
    EXPECT_EQ(std::string("initialized"), v_copied.current_status_as_string());
    EXPECT_EQ("1", v_copied.label());
    EXPECT_EQ(0ul, v_copied.edge_count());
    EXPECT_FALSE(v_copied.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_copied.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.current_status());
    EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_FALSE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
    EXPECT_EQ(nullptr, v_copied.get_task());
}

TEST_F(TestDagVertex, assignment_operator_no_edges) {
    DAGVertex v("1");
    DAGVertex v_copied;
    v_copied = v;

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.current_status());
    EXPECT_EQ(std::string("initialized"), v_copied.current_status_as_string());
    EXPECT_EQ("1", v_copied.label());
    EXPECT_EQ(0ul, v_copied.edge_count());
    EXPECT_FALSE(v_copied.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_copied.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.current_status());
    EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_FALSE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
    EXPECT_EQ(nullptr, v_copied.get_task());
}

TEST_F(TestDagVertex, move_ctor_no_edges_with_task) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->get_uuid().as_string();
    DAGVertex v("1", std::move(task));
    DAGVertex v_moved = std::move(v);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.current_status());
    EXPECT_EQ(std::string("initialized"), v_moved.current_status_as_string());
    EXPECT_EQ("1", v_moved.label());
    EXPECT_EQ(0ul, v_moved.edge_count());
    EXPECT_FALSE(v_moved.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_moved.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INVALID, v.current_status());
    EXPECT_EQ(std::string("invalid"), v.current_status_as_string());
    EXPECT_EQ("", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_TRUE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
    EXPECT_EQ("1 Task", v_moved.get_task()->label());
    EXPECT_EQ(task_uuid, v_moved.get_task()->get_uuid().as_string());
}

TEST_F(TestDagVertex, assignment_move_operator_no_edges_with_task) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->get_uuid().as_string();
    DAGVertex v("1", std::move(task));
    DAGVertex v_moved;
    v_moved = std::move(v);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.current_status());
    EXPECT_EQ(std::string("initialized"), v_moved.current_status_as_string());
    EXPECT_EQ("1", v_moved.label());
    EXPECT_EQ(0ul, v_moved.edge_count());
    EXPECT_FALSE(v_moved.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_moved.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INVALID, v.current_status());
    EXPECT_EQ(std::string("invalid"), v.current_status_as_string());
    EXPECT_EQ("", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_TRUE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
    EXPECT_EQ("1 Task", v_moved.get_task()->label());
    EXPECT_EQ(task_uuid, v_moved.get_task()->get_uuid().as_string());
}

TEST_F(TestDagVertex, clone_no_edges_with_task) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->get_uuid().as_string();
    DAGVertex v("1", std::move(task));
    DAGVertex v_cloned = v.clone();

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_cloned.current_status());
    EXPECT_EQ(std::string("initialized"), v_cloned.current_status_as_string());
    EXPECT_EQ("1", v_cloned.label());
    EXPECT_EQ(0ul, v_cloned.edge_count());
    EXPECT_FALSE(v_cloned.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_cloned.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.current_status());
    EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_FALSE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
    EXPECT_NE(nullptr, v_cloned.get_task());
    EXPECT_EQ(v_cloned, v);
}

TEST_F(TestDagVertex, copy_ctor_no_edges_with_task) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->get_uuid().as_string();
    DAGVertex v("1", std::move(task));
    DAGVertex v_copied(v);

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.current_status());
    EXPECT_EQ(std::string("initialized"), v_copied.current_status_as_string());
    EXPECT_EQ("1", v_copied.label());
    EXPECT_EQ(0ul, v_copied.edge_count());
    EXPECT_FALSE(v_copied.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_copied.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.current_status());
    EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_FALSE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
    EXPECT_NE(nullptr, v_copied.get_task());
    EXPECT_EQ(v_copied, v);
}

TEST_F(TestDagVertex, assignment_operator_no_edges_with_task) {
    std::vector<std::unique_ptr<TaskStage>> stages{};
    auto task = std::make_unique<Task>(stages, "1 Task");
    std::string task_uuid = task->get_uuid().as_string();
    DAGVertex v("1", std::move(task));
    DAGVertex v_copied;
    v_copied = v;

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.current_status());
    EXPECT_EQ(std::string("initialized"), v_copied.current_status_as_string());
    EXPECT_EQ("1", v_copied.label());
    EXPECT_EQ(0ul, v_copied.edge_count());
    EXPECT_FALSE(v_copied.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_copied.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.current_status());
    EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
    EXPECT_EQ("1", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_FALSE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
    EXPECT_NE(nullptr, v_copied.get_task());
    EXPECT_EQ(v_copied, v);
}

TEST_F(TestDagVertex, connect_and_contains_connection) {
    {
        std::shared_ptr<DAGVertex> v = std::make_shared<DAGVertex>("2");
        EXPECT_TRUE(get_dag_vertex().connect(v));
        EXPECT_TRUE(get_dag_vertex().contains_connection_to(*v));
        EXPECT_EQ(1ul, get_dag_vertex().edge_count());
    }
    {
        std::shared_ptr<DAGVertex> v = std::make_shared<DAGVertex>("2");
        EXPECT_TRUE(get_dag_vertex().connect(v));
        EXPECT_TRUE(get_dag_vertex().contains_connection_to(*v));
        EXPECT_EQ(2ul, get_dag_vertex().edge_count());
    }
    {
        std::shared_ptr<DAGVertex> v = std::make_shared<DAGVertex>("3");
        EXPECT_TRUE(get_dag_vertex().connect(v));
        EXPECT_TRUE(get_dag_vertex().contains_connection_to(*v));
        EXPECT_EQ(3ul, get_dag_vertex().edge_count());
    }
    {
        std::shared_ptr<DAGVertex> dup = std::make_shared<DAGVertex>("dup");
        EXPECT_TRUE(get_dag_vertex().connect(dup));
        EXPECT_TRUE(get_dag_vertex().contains_connection_to(*dup));
        EXPECT_EQ(4ul, get_dag_vertex().edge_count());
        EXPECT_FALSE(get_dag_vertex().connect(dup));
        EXPECT_TRUE(get_dag_vertex().contains_connection_to(*dup));
        EXPECT_EQ(4ul, get_dag_vertex().edge_count());
    }
    get_dag_vertex().clear_edges();
    ASSERT_EQ(0ul, get_dag_vertex().edge_count());
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
        get_dag_vertex().connect(v);
    }
    EXPECT_EQ(connections.size(), get_dag_vertex().edge_count());

    std::size_t index = 0;
    get_dag_vertex().visit_all_edges([&](const DAGEdge& e) {
        DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
        EXPECT_EQ(connections[index], e_ref.get_connection().lock());
        EXPECT_EQ(*connections[index], *(e_ref.get_connection().lock()));
        ++index;
    });
    EXPECT_EQ(get_dag_vertex().edge_count(), index);

    get_dag_vertex().clear_edges();
    ASSERT_EQ(0ul, get_dag_vertex().edge_count());
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
        get_dag_vertex().connect(v);
    }
    EXPECT_EQ(connections.size(), get_dag_vertex().edge_count());

    std::vector<DAGVertex::DAGVertex_connection> edges =
            get_dag_vertex().clone_all_connections();
    // First vertex connects to itself.
    EXPECT_EQ(connections.size(), edges.size());

    DAGVertex tmp = get_dag_vertex().clone();
    std::vector<std::shared_ptr<DAGVertex>> connections_cloned =
            tmp.reestablish_connections(edges);
    EXPECT_EQ(get_dag_vertex(), tmp);

    std::size_t index = 0;
    get_dag_vertex().visit_all_edges([&](const DAGEdge& e) {
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

    get_dag_vertex().clear_edges();
    ASSERT_EQ(0ul, get_dag_vertex().edge_count());
}

TEST_F(TestDagVertex, has_incoming_edge_and_incoming_edge_count) {
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
        EXPECT_TRUE((*v).connect(get_dag_vertex_ptr()));
        ++count;
        EXPECT_EQ(count, get_dag_vertex_ptr()->incoming_edge_count());
        EXPECT_TRUE(get_dag_vertex_ptr()->has_incoming_edges());

        EXPECT_EQ(1ul, v->edge_count());
        v->visit_all_edges([&](const DAGEdge& e) {
            DAGEdge& e_ref = *const_cast<DAGEdge*>(&e);
            EXPECT_EQ(e_ref.get_connection().lock(), get_dag_vertex_ptr());
            EXPECT_EQ(*(e_ref.get_connection().lock()),
                      *(get_dag_vertex_ptr()));
        });
    }
    EXPECT_EQ(0ul, get_dag_vertex_ptr()->edge_count());
    EXPECT_EQ(count, get_dag_vertex_ptr()->incoming_edge_count());
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

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.current_status());
    EXPECT_EQ(std::string("initialized"), v_moved.current_status_as_string());
    EXPECT_EQ("orig", v_moved.label());
    EXPECT_EQ(connections.size(), v_moved.edge_count());
    EXPECT_FALSE(v_moved.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_moved.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INVALID, v.current_status());
    EXPECT_EQ(std::string("invalid"), v.current_status_as_string());
    EXPECT_EQ("", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_TRUE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());

    v.clear_edges();
    EXPECT_EQ(0ul, v.edge_count());

    v_moved.clear_edges();
    EXPECT_EQ(0ul, v_moved.edge_count());
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

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_moved.current_status());
    EXPECT_EQ(std::string("initialized"), v_moved.current_status_as_string());
    EXPECT_EQ("orig", v_moved.label());
    EXPECT_EQ(connections.size(), v_moved.edge_count());
    EXPECT_FALSE(v_moved.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_moved.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INVALID, v.current_status());
    EXPECT_EQ(std::string("invalid"), v.current_status_as_string());
    EXPECT_EQ("", v.label());
    EXPECT_EQ(0ul, v.edge_count());
    EXPECT_TRUE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());

    v.clear_edges();
    EXPECT_EQ(0ul, v.edge_count());

    v_moved.clear_edges();
    EXPECT_EQ(0ul, v_moved.edge_count());
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
    ASSERT_EQ(connections.size(), v.edge_count());

    DAGVertex v_cloned = v.clone();
    ASSERT_EQ(0ul, v_cloned.edge_count());

    // We cannot add back the connections since the edge adds a weak_ptr
    // to a DAGVertex we no longer can duplicate. This has to be done
    // outside the class by the code that is cloning the DAGVertex.
    // DAG_t should be the object that orchestrates that.
    std::vector<DAGVertex::DAGVertex_connection> orig_connections =
            v.clone_all_connections();
    std::vector<std::shared_ptr<DAGVertex>> cloned_connections =
            v_cloned.reestablish_connections(orig_connections);

    std::size_t v_edge_count = v.edge_count();
    std::size_t v_cloned_edge_count = v_cloned.edge_count();
    ASSERT_EQ(v_edge_count, v_cloned_edge_count);

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

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_cloned.current_status());
    EXPECT_EQ(std::string("initialized"), v_cloned.current_status_as_string());
    EXPECT_EQ("orig", v_cloned.label());
    EXPECT_EQ(connections.size(), v_cloned.edge_count());
    EXPECT_FALSE(v_cloned.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_cloned.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.current_status());
    EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
    EXPECT_EQ("orig", v.label());
    EXPECT_EQ(connections.size(), v.edge_count());
    EXPECT_FALSE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
}

TEST_F(TestDagVertex, add_incoming_edge) {
    DAGVertex v("orig");
    v.add_incoming_edge();
    EXPECT_EQ(1, v.incoming_edge_count());
}

TEST_F(TestDagVertex, sub_incoming_edge) {
    DAGVertex v("orig");
    v.add_incoming_edge();
    v.add_incoming_edge();
    v.sub_incoming_edge();
    EXPECT_EQ(1, v.incoming_edge_count());
}

TEST_F(TestDagVertex, clear_edges) {
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
    EXPECT_EQ(6, v.edge_count());
    v.clear_edges();
    EXPECT_EQ(0, v.edge_count());
}

TEST_F(TestDagVertex, reset_incoming_edge_count) {
    DAGVertex v("orig");
    v.add_incoming_edge();
    v.add_incoming_edge();
    v.add_incoming_edge();
    v.add_incoming_edge();
    v.add_incoming_edge();
    EXPECT_EQ(5, v.incoming_edge_count());
    v.reset_incoming_edge_count();
    EXPECT_EQ(0, v.incoming_edge_count());
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
    ASSERT_EQ(connections.size(), v.edge_count());

    DAGVertex v_copied(v.clone());
    ASSERT_EQ(0ul, v_copied.edge_count());

    // We cannot add back the connections since the edge adds a weak_ptr
    // to a DAGVertex we no longer can duplicate. This has to be done
    // outside the class by the code that is cloning the DAGVertex.
    // DAG_t should be the object that orchestrates that.
    std::vector<DAGVertex::DAGVertex_connection> orig_connections =
            v.clone_all_connections();
    std::vector<std::shared_ptr<DAGVertex>> copied_connections =
            v_copied.reestablish_connections(orig_connections);

    std::size_t v_edge_count = v.edge_count();
    std::size_t v_copied_edge_count = v_copied.edge_count();
    ASSERT_EQ(v_edge_count, v_copied_edge_count);

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

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.current_status());
    EXPECT_EQ(std::string("initialized"), v_copied.current_status_as_string());
    EXPECT_EQ("orig", v_copied.label());
    EXPECT_EQ(connections.size(), v_copied.edge_count());
    EXPECT_FALSE(v_copied.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_copied.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.current_status());
    EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
    EXPECT_EQ("orig", v.label());
    EXPECT_EQ(connections.size(), v.edge_count());
    EXPECT_FALSE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
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
    ASSERT_EQ(connections.size(), v.edge_count());

    DAGVertex v_copied;
    v_copied = v.clone();
    ASSERT_EQ(0ul, v_copied.edge_count());

    // We cannot add back the connections since the edge adds a weak_ptr
    // to a DAGVertex we no longer can duplicate. This has to be done
    // outside the class by the code that is cloning the DAGVertex. DAG_t
    // should be the object that orchestrates that.
    std::vector<DAGVertex::DAGVertex_connection> orig_connections =
            v.clone_all_connections();
    std::vector<std::shared_ptr<DAGVertex>> copied_connections =
            v_copied.reestablish_connections(orig_connections);

    std::size_t v_edge_count = v.edge_count();
    std::size_t v_copied_edge_count = v_copied.edge_count();
    ASSERT_EQ(v_edge_count, v_copied_edge_count);

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

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v_copied.current_status());
    EXPECT_EQ(std::string("initialized"), v_copied.current_status_as_string());
    EXPECT_EQ("orig", v_copied.label());
    EXPECT_EQ(connections.size(), v_copied.edge_count());
    EXPECT_FALSE(v_copied.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v_copied.incoming_edge_count());

    EXPECT_EQ(DAGVertex::Status::INITIALIZED, v.current_status());
    EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
    EXPECT_EQ("orig", v.label());
    EXPECT_EQ(connections.size(), v.edge_count());
    EXPECT_FALSE(v.get_uuid().as_string().empty());
    EXPECT_EQ(0ul, v.incoming_edge_count());
}
}  // namespace com::dag_scheduler
