#include <gtest/gtest.h>

#include "dag_scheduler/dag.h"
#include "dag_scheduler/dag_algorithms.h"

#include <iostream>

namespace com
{
  namespace dag_scheduler
  {
    class TestDag : public ::testing::Test
    {
    protected:
      virtual void SetUp() {}

      virtual void TearDown() {}

      DAG &get_dag()
      {
        return d_;
      }

      std::vector<DAGVertex> fill_dag_default()
      {
        DAGVertex v0("1"), v1("1"), v2("1a"), v3("1b"), v4("2"), v5("2"),
          v6("2"), v7("3"), v8("3"), v9("4"), v10("5");
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
        vertices_to_add.push_back(std::move(v10));

        std::vector<DAGVertex> vertices_cloned;
        vertices_cloned.reserve(vertices_to_add.size());
        std::for_each(vertices_to_add.begin(), vertices_to_add.end(),
          [&](DAGVertex &v) {
            DAGVertex v_clone_1 = v.clone();
            DAGVertex v_clone_2 = v.clone();
            DAGVertex v_clone_3 = v.clone();

            vertices_cloned.push_back(std::move(v_clone_1));
            get_dag().add_vertex(std::move(v_clone_2));

            std::weak_ptr<DAGVertex> v_weak =
              get_dag().find_vertex_by_uuid(v_clone_3.get_uuid());
            EXPECT_FALSE(v_weak.expired());
          }
        );
        EXPECT_EQ(vertices_to_add.size(), get_dag().vertex_count());
        vertices_to_add.clear();

        return vertices_cloned;
      }

    private:
      DAG d_;
    };

    TEST_F(TestDag, ctor)
    {
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, ctor_with_title)
    {
      DAG test_dag("test_dag");

      EXPECT_EQ(0ul, test_dag.edge_count());
      EXPECT_EQ(0ul, test_dag.vertex_count());
      EXPECT_EQ(std::string("test_dag"), test_dag.title());
    }

    TEST_F(TestDag, move_ctor_no_edges_no_vertices)
    {
      DAG d;

      DAG d_moved(std::move(d));
      EXPECT_EQ(0ul, d_moved.edge_count());
      EXPECT_EQ(0ul, d_moved.vertex_count());
    }

    TEST_F(TestDag, assignment_move_operator_no_edges_no_vertices)
    {
      DAG d;

      DAG d_moved;
      d_moved = std::move(d);
      EXPECT_EQ(0ul, d_moved.edge_count());
      EXPECT_EQ(0ul, d_moved.vertex_count());
    }

    TEST_F(TestDag, clone_no_edges_no_vertices)
    {
      DAG d;

      DAG d_cloned = d.clone();
      EXPECT_EQ(0ul, d_cloned.edge_count());
      EXPECT_EQ(0ul, d_cloned.vertex_count());
    }

    TEST_F(TestDag, add_vertex_and_reset)
    {
      DAGVertex v1("1");
      get_dag().add_vertex(std::move(v1));
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(1ul, get_dag().vertex_count());

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, add_single_vertex_and_find_vertex_by_vertex_and_uuid)
    {
      DAGVertex v1("1");
      DAGVertex v1_cloned = v1.clone();
      get_dag().add_vertex(std::move(v1));

      std::weak_ptr<DAGVertex> v1_weak = get_dag().find_vertex(v1_cloned);
      EXPECT_EQ(v1_cloned, *(v1_weak.lock()));

      std::weak_ptr<DAGVertex> v2_weak = get_dag().find_vertex_by_uuid(
        v1_cloned.get_uuid());
      EXPECT_EQ(v1_cloned, *(v2_weak.lock()));
      EXPECT_TRUE(get_dag().contains_vertex(v1_cloned));
      EXPECT_TRUE(get_dag().contains_vertex_by_uuid(v1_cloned.get_uuid()));
      EXPECT_TRUE(get_dag().contains_vertex_by_label(v1_cloned.label()));

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
      EXPECT_TRUE(v1_weak.expired());
      EXPECT_TRUE(v2_weak.expired());
    }

    TEST_F(TestDag, add_single_vertex_and_find_vertex_by_label)
    {
      DAGVertex v1("1");
      DAGVertex v1_cloned = v1.clone();
      get_dag().add_vertex(std::move(v1));

      std::vector<std::weak_ptr<DAGVertex>> vertices =
        get_dag().find_all_verticies_with_label(v1_cloned.label());
      EXPECT_EQ(1ul, vertices.size());
      EXPECT_EQ(v1_cloned, *(vertices[0].lock()));

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
      EXPECT_TRUE(vertices[0].expired());
    }

    TEST_F(TestDag, add_vertices_with_same_and_different_labels)
    {
      std::vector<DAGVertex> vertices_cloned = fill_dag_default();

      {
        std::weak_ptr<DAGVertex> v_weak =
          get_dag().find_vertex(vertices_cloned[0]);
        ASSERT_FALSE(v_weak.expired());
        EXPECT_EQ(vertices_cloned[0], *(v_weak.lock()));
        EXPECT_NE(vertices_cloned[1], *(v_weak.lock()));

        std::shared_ptr<DAGVertex> v = v_weak.lock();
        EXPECT_TRUE(get_dag().contains_vertex(*v));
        EXPECT_TRUE(get_dag().contains_vertex_by_uuid(v->get_uuid()));
        EXPECT_TRUE(get_dag().contains_vertex_by_label(v->label()));
      }

      {
        std::weak_ptr<DAGVertex> v_weak =
          get_dag().find_vertex_by_uuid(vertices_cloned[1].get_uuid());
        ASSERT_FALSE(v_weak.expired());
        EXPECT_NE(vertices_cloned[0], *(v_weak.lock()));
        EXPECT_EQ(vertices_cloned[1], *(v_weak.lock()));

        std::shared_ptr<DAGVertex> v = v_weak.lock();
        EXPECT_TRUE(get_dag().contains_vertex(*v));
        EXPECT_TRUE(get_dag().contains_vertex_by_uuid(v->get_uuid()));
        EXPECT_TRUE(get_dag().contains_vertex_by_label(v->label()));
      }

      {
        std::weak_ptr<DAGVertex> v_weak =
          get_dag().find_vertex_by_uuid(vertices_cloned[6].get_uuid());
        ASSERT_FALSE(v_weak.expired());
        EXPECT_NE(vertices_cloned[4], *(v_weak.lock()));
        EXPECT_NE(vertices_cloned[5], *(v_weak.lock()));
        EXPECT_EQ(vertices_cloned[6], *(v_weak.lock()));

        std::shared_ptr<DAGVertex> v = v_weak.lock();
        EXPECT_TRUE(get_dag().contains_vertex(*v));
        EXPECT_TRUE(get_dag().contains_vertex_by_uuid(v->get_uuid()));
        EXPECT_TRUE(get_dag().contains_vertex_by_label(v->label()));
      }

      {
        std::vector<std::weak_ptr<DAGVertex>> v_weaks =
          get_dag().find_all_verticies_with_label("2");
        EXPECT_EQ(3ul, v_weaks.size());
        EXPECT_EQ(vertices_cloned[4], *(v_weaks[0].lock()));
        EXPECT_EQ(vertices_cloned[5], *(v_weaks[1].lock()));
        EXPECT_EQ(vertices_cloned[6], *(v_weaks[2].lock()));

        for (std::weak_ptr<DAGVertex> v_weak : v_weaks) {
          std::shared_ptr<DAGVertex> v = v_weak.lock();
          EXPECT_TRUE(get_dag().contains_vertex(*v));
          EXPECT_TRUE(get_dag().contains_vertex_by_uuid(v->get_uuid()));
          EXPECT_TRUE(get_dag().contains_vertex_by_label(v->label()));
        }
      }

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, clone_with_multiple_vertices_no_connections)
    {
      std::vector<DAGVertex> vertices_cloned = fill_dag_default();
      DAG dag_cloned = get_dag().clone();

      for (const DAGVertex &v : vertices_cloned) {
        EXPECT_TRUE(get_dag().contains_vertex(v));
        EXPECT_TRUE(dag_cloned.contains_vertex(v));
      }
      EXPECT_EQ(get_dag(), dag_cloned);

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, connect_and_acyclic_check)
    {
      std::vector<DAGVertex> vertices_cloned = fill_dag_default();

      /* cloned_vertices[0] -> cloned_vertices[1] */
      EXPECT_FALSE(get_dag().connection_would_make_cyclic(
        vertices_cloned[0], vertices_cloned[1]));
      EXPECT_FALSE(get_dag().connection_would_make_cyclic_by_uuid(
        vertices_cloned[0].get_uuid(), vertices_cloned[1].get_uuid()));
      EXPECT_TRUE(get_dag().connect(vertices_cloned[0], vertices_cloned[1]));
      EXPECT_EQ(1ul, get_dag().find_vertex(
        vertices_cloned[0]).lock()->edge_count());
      EXPECT_EQ(0ul, get_dag().find_vertex(
        vertices_cloned[0]).lock()->incomming_edge_count());
      EXPECT_EQ(0ul, get_dag().find_vertex(
        vertices_cloned[1]).lock()->edge_count());
      EXPECT_EQ(1ul, get_dag().find_vertex(
        vertices_cloned[1]).lock()->incomming_edge_count());

      /* cloned_vertices[1] -> cloned_vertices[2] */
      EXPECT_FALSE(get_dag().connection_would_make_cyclic(
        vertices_cloned[1], vertices_cloned[2]));
      EXPECT_FALSE(get_dag().connection_would_make_cyclic_by_uuid(
        vertices_cloned[1].get_uuid(), vertices_cloned[2].get_uuid()));
      EXPECT_TRUE(get_dag().connect(vertices_cloned[1], vertices_cloned[2]));

      /* cloned_vertices[0] -> cloned_vertices[2] */
      EXPECT_FALSE(get_dag().connection_would_make_cyclic(
        vertices_cloned[0], vertices_cloned[2]));
      EXPECT_FALSE(get_dag().connection_would_make_cyclic_by_uuid(
        vertices_cloned[0].get_uuid(), vertices_cloned[2].get_uuid()));
      EXPECT_TRUE(get_dag().connect(vertices_cloned[0], vertices_cloned[2]));

      /* cloned_vertices[2] -> cloned_vertices[0] */
      EXPECT_TRUE(get_dag().connection_would_make_cyclic(
        vertices_cloned[2], vertices_cloned[0]));
      EXPECT_TRUE(get_dag().connection_would_make_cyclic_by_uuid(
        vertices_cloned[2].get_uuid(), vertices_cloned[0].get_uuid()));
      EXPECT_THROW(get_dag().connect(vertices_cloned[2], vertices_cloned[0]),
        DAG::DAGException);

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, connect_all_by_label_and_acyclic_check)
    {
      std::vector<DAGVertex> vertices_cloned = fill_dag_default();

      EXPECT_TRUE(get_dag().connect_all_by_label("1", "2"));
      EXPECT_THROW(get_dag().connect_all_by_label("2", "1"),
        DAG::DAGException);
      EXPECT_TRUE(get_dag().connect_all_by_label("1a", "5"));

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, add_and_connect_and_acyclic_check)
    {
      std::vector<DAGVertex> vertices_cloned = fill_dag_default();
      DAG test_dag("test_dag");

      {
        DAGVertex clone0 = vertices_cloned[0].clone();
        DAGVertex clone1 = vertices_cloned[1].clone();
        EXPECT_TRUE(test_dag.add_and_connect(
            std::move(clone0),
            std::move(clone1)
          )
        );
      }

      {
        DAGVertex clone0 = vertices_cloned[1].clone();
        DAGVertex clone1 = vertices_cloned[0].clone();
        EXPECT_THROW(test_dag.add_and_connect(
            std::move(clone0),
            std::move(clone1)
          ),
          DAG::DAGException
        );
      }

      {
        DAGVertex clone0 = vertices_cloned[2].clone();
        DAGVertex clone1 = vertices_cloned[3].clone();
        EXPECT_TRUE(test_dag.add_and_connect(
            std::move(clone0),
            std::move(clone1)
          )
        );
      }

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, connect_by_uuid)
    {
      std::vector<DAGVertex> vertices_cloned = fill_dag_default();
      DAG test_dag("test_dag");

      {
        DAGVertex clone0 = vertices_cloned[0].clone();
        DAGVertex clone1 = vertices_cloned[1].clone();
        test_dag.add_vertex(vertices_cloned[0].clone());
        test_dag.add_vertex(vertices_cloned[1].clone());
        EXPECT_TRUE(test_dag.connect_by_uuid(
            clone0.get_uuid(),
            clone1.get_uuid()
          )
        );

        std::weak_ptr<DAGVertex> clone0_confirm = test_dag.find_vertex(
          clone0);
        EXPECT_NE(nullptr, clone0_confirm.lock());
        std::weak_ptr<DAGVertex> clone1_confirm = test_dag.find_vertex(
          clone1);
        EXPECT_NE(nullptr, clone1_confirm.lock());
        EXPECT_TRUE(clone0_confirm.lock()->contains_connection_to(
          *(clone1_confirm.lock().get())));
      }

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, are_connected)
    {
      std::vector<DAGVertex> vertices_cloned = fill_dag_default();

      {
        DAGVertex clone0 = vertices_cloned[0].clone();
        DAGVertex clone1 = vertices_cloned[1].clone();
        DAGVertex clone2 = vertices_cloned[2].clone();
        EXPECT_TRUE(get_dag().connect(clone0, clone1));
        EXPECT_TRUE(get_dag().are_connected(clone0, clone1));
        EXPECT_FALSE(get_dag().are_connected(clone1, clone2));
      }

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, are_connected_by_uuid)
    {
      std::vector<DAGVertex> vertices_cloned = fill_dag_default();

      {
        DAGVertex clone0 = vertices_cloned[0].clone();
        DAGVertex clone1 = vertices_cloned[1].clone();
        DAGVertex clone2 = vertices_cloned[2].clone();
        EXPECT_TRUE(get_dag().connect(clone0, clone1));
        EXPECT_TRUE(get_dag().are_connected_by_uuid(
            clone0.get_uuid(),
            clone1.get_uuid()
          )
        );
        EXPECT_FALSE(get_dag().are_connected_by_uuid(
            clone1.get_uuid(),
            clone2.get_uuid()
          )
        );
      }

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, all_are_connected_by_label)
    {
      std::vector<DAGVertex> vertices_cloned = fill_dag_default();

      EXPECT_TRUE(get_dag().connect_all_by_label("1", "2"));
      EXPECT_TRUE(get_dag().connect_all_by_label("1a", "5"));

      EXPECT_TRUE(get_dag().all_are_connected_by_label("1", "2"));
      EXPECT_TRUE(get_dag().all_are_connected_by_label("1a", "5"));
      EXPECT_FALSE(get_dag().all_are_connected_by_label("3", "5"));
      EXPECT_FALSE(get_dag().all_are_connected_by_label("5", "3"));

      EXPECT_TRUE(get_dag().connect_all_by_label("3", "5"));
      EXPECT_TRUE(get_dag().all_are_connected_by_label("3", "5"));
      EXPECT_FALSE(get_dag().all_are_connected_by_label("5", "3"));

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, get_vertex_at)
    {
      std::vector<DAGVertex> vertices_cloned = fill_dag_default();
      ASSERT_EQ(vertices_cloned.size(), get_dag().vertex_count());
      for (std::size_t i = 0; i < vertices_cloned.size(); ++i) {
        EXPECT_EQ(vertices_cloned[i], *(get_dag().get_vertex_at(i)));
      }

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, clone_connections)
    {
      std::vector<DAGVertex> vertices_cloned = fill_dag_default();
      for (std::size_t i = 1; i < vertices_cloned.size(); ++i) {
        get_dag().connect(vertices_cloned[0], vertices_cloned[i]);
      }
      DAGVertex v = vertices_cloned[0].clone();
      get_dag().clone_connections(*(get_dag().get_vertex_at(0)), v);
      EXPECT_EQ(v, *(get_dag().get_vertex_at(0)));

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, copy_ctor)
    {
      {
        DAG d_copy(get_dag());
        EXPECT_EQ(d_copy, get_dag());
      }

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());

      {
        std::vector<DAGVertex> vertices_cloned = fill_dag_default();
        DAG d_copy(get_dag());
        EXPECT_EQ(d_copy, get_dag());
      }

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());

      {
        std::vector<DAGVertex> vertices_cloned = fill_dag_default();
        get_dag().connect_all_by_label("1", "2");
        get_dag().connect_all_by_label("2", "3");
        get_dag().connect_all_by_label("3", "5");
        DAG d_copy(get_dag());
        EXPECT_EQ(d_copy, get_dag());
      }

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, assignment_operator)
    {
      {
        DAG d_copy;
        d_copy = get_dag();
        EXPECT_EQ(d_copy, get_dag());
      }

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());

      {
        std::vector<DAGVertex> vertices_cloned = fill_dag_default();
        DAG d_copy;
        d_copy = get_dag();
        EXPECT_EQ(d_copy, get_dag());
      }

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());

      {
        std::vector<DAGVertex> vertices_cloned = fill_dag_default();
        get_dag().connect_all_by_label("1", "2");
        get_dag().connect_all_by_label("2", "3");
        get_dag().connect_all_by_label("3", "5");
        DAG d_copy;
        d_copy = get_dag();
        EXPECT_EQ(d_copy, get_dag());
      }

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, remove_vertex)
    {
      fill_dag_default();
      DAG g_clone = get_dag().clone();

      auto find_v_1a = g_clone.find_all_verticies_with_label("1a");
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
            EXPECT_EQ(1u, v->incomming_edge_count());
          } else {
            EXPECT_EQ(0u, v->incomming_edge_count());
          }
        });

      ASSERT_TRUE(g_clone.remove_vertex(*(find_v_1a[0].lock())));

      EXPECT_EQ(0u, g_clone.find_all_verticies_with_label("1a").size());
      g_clone.linear_traversal([&](std::shared_ptr<DAGVertex> v) {
          ASSERT_NE(nullptr, v.get());
          EXPECT_EQ(0u, v->incomming_edge_count());
        });

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, remove_vertex_by_uuid)
    {
      fill_dag_default();
      DAG g_clone = get_dag().clone();

      auto find_v_1a = g_clone.find_all_verticies_with_label("1a");
      g_clone.connect_all_by_label("1a", "1");
      g_clone.connect_all_by_label("1a", "1b");
      g_clone.connect_all_by_label("1a", "2");
      g_clone.connect_all_by_label("1a", "3");
      g_clone.connect_all_by_label("1a", "4");
      g_clone.connect_all_by_label("1a", "5");
      ASSERT_EQ(11u, g_clone.vertex_count());
      ASSERT_EQ(1u, find_v_1a.size());
      EXPECT_EQ("1a", find_v_1a[0].lock()->label());

      ASSERT_TRUE(
        g_clone.remove_vertex_by_uuid(find_v_1a[0].lock()->get_uuid()));

      EXPECT_EQ(0u, g_clone.find_all_verticies_with_label("1a").size());
      g_clone.linear_traversal([&](std::shared_ptr<DAGVertex> v) {
          ASSERT_NE(nullptr, v.get());
          EXPECT_EQ(0u, v->incomming_edge_count());
        });

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestDag, remove_vertices_with_label)
    {
      fill_dag_default();
      DAG g_clone = get_dag().clone();

      auto find_v_1a = g_clone.find_all_verticies_with_label("1a");
      g_clone.connect_all_by_label("1a", "1");
      g_clone.connect_all_by_label("1a", "1b");
      g_clone.connect_all_by_label("1a", "2");
      g_clone.connect_all_by_label("1a", "3");
      g_clone.connect_all_by_label("1a", "4");
      g_clone.connect_all_by_label("1a", "5");
      g_clone.connect_all_by_label("1", "2");
      EXPECT_EQ(11u, g_clone.vertex_count());
      ASSERT_TRUE(g_clone.remove_all_vertex_with_label("1"));
      ASSERT_TRUE(g_clone.remove_all_vertex_with_label("1a"));
      EXPECT_EQ(8u, g_clone.vertex_count());

      g_clone.linear_traversal([&](std::shared_ptr<DAGVertex> v) {
          ASSERT_NE(nullptr, v.get());
          EXPECT_EQ(0u, v->incomming_edge_count());
        });
    }
  }
}
