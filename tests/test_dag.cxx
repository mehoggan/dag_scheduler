#include <gtest/gtest.h>

#include "u_dagtasks/dag.h"
#include "u_dagtasks/dag_algorithms.h"

namespace uber
{
  namespace u_dagtasks
  {
    class TestUDag : public ::testing::Test
    {
    protected:
      virtual void SetUp() {}

      virtual void TearDown() {}

      dag &get_dag()
      {
        return d_;
      }

      std::vector<dag_vertex> fill_dag_default()
      {
        dag_vertex v0("1"), v1("1"), v2("1a"), v3("1b"), v4("2"), v5("2"),
          v6("2"), v7("3"), v8("4"), v9("5");
        std::vector<dag_vertex> vertices_to_add;
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

        std::vector<dag_vertex> vertices_cloned;
        vertices_cloned.reserve(vertices_to_add.size());
        std::for_each(vertices_to_add.begin(), vertices_to_add.end(),
          [&](dag_vertex &v) {
            dag_vertex v_clone_1 = v.clone();
            dag_vertex v_clone_2 = v.clone();
            dag_vertex v_clone_3 = v.clone();

            vertices_cloned.push_back(std::move(v_clone_1));
            get_dag().add_vertex(std::move(v_clone_2));

            std::weak_ptr<dag_vertex> v_weak =
              get_dag().find_vertex_by_uuid(v_clone_3.get_uuid());
            EXPECT_FALSE(v_weak.expired());
          }
        );
        EXPECT_EQ(vertices_to_add.size(), get_dag().vertex_count());
        vertices_to_add.clear();

        return vertices_cloned;
      }

    private:
      dag d_;
    };

    TEST_F(TestUDag, ctor)
    {
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestUDag, dtor_no_edges_no_vertices)
    {
      dag d;

      d.~dag();
      EXPECT_EQ(0ul, d.edge_count());
      EXPECT_EQ(0ul, d.vertex_count());
    }

    TEST_F(TestUDag, move_ctor_no_edges_no_vertices)
    {
      dag d;

      dag d_moved(std::move(d));
      EXPECT_EQ(0ul, d_moved.edge_count());
      EXPECT_EQ(0ul, d_moved.vertex_count());
    }

    TEST_F(TestUDag, assignment_move_operator_no_edges_no_vertices)
    {
      dag d;

      dag d_moved;
      d_moved = std::move(d);
      EXPECT_EQ(0ul, d_moved.edge_count());
      EXPECT_EQ(0ul, d_moved.vertex_count());
    }

    TEST_F(TestUDag, clone_no_edges_no_vertices)
    {
      dag d;

      dag d_cloned = d.clone();
      EXPECT_EQ(0ul, d_cloned.edge_count());
      EXPECT_EQ(0ul, d_cloned.vertex_count());
    }

    TEST_F(TestUDag, add_vertex_and_reset)
    {
      dag_vertex v1("1");
      get_dag().add_vertex(std::move(v1));
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(1ul, get_dag().vertex_count());

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestUDag, add_single_vertex_and_find_vertex_by_vertex_and_uuid)
    {
      dag_vertex v1("1");
      dag_vertex v1_cloned = v1.clone();
      get_dag().add_vertex(std::move(v1));

      std::weak_ptr<dag_vertex> v1_weak = get_dag().find_vertex(v1_cloned);
      EXPECT_EQ(v1_cloned, *(v1_weak.lock()));

      std::weak_ptr<dag_vertex> v2_weak = get_dag().find_vertex_by_uuid(
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

    TEST_F(TestUDag, add_single_vertex_and_find_vertex_by_label)
    {
      dag_vertex v1("1");
      dag_vertex v1_cloned = v1.clone();
      get_dag().add_vertex(std::move(v1));

      std::vector<std::weak_ptr<dag_vertex>> vertices =
        get_dag().find_all_verticies_with_label(v1_cloned.label());
      EXPECT_EQ(1ul, vertices.size());
      EXPECT_EQ(v1_cloned, *(vertices[0].lock()));

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
      EXPECT_TRUE(vertices[0].expired());
    }

    TEST_F(TestUDag, add_vertices_with_same_and_different_labels)
    {
      std::vector<dag_vertex> vertices_cloned = fill_dag_default();

      {
        std::weak_ptr<dag_vertex> v_weak =
          get_dag().find_vertex(vertices_cloned[0]);
        ASSERT_FALSE(v_weak.expired());
        EXPECT_EQ(vertices_cloned[0], *(v_weak.lock()));
        EXPECT_NE(vertices_cloned[1], *(v_weak.lock()));

        std::shared_ptr<dag_vertex> v = v_weak.lock();
        EXPECT_TRUE(get_dag().contains_vertex(*v));
        EXPECT_TRUE(get_dag().contains_vertex_by_uuid(v->get_uuid()));
        EXPECT_TRUE(get_dag().contains_vertex_by_label(v->label()));
      }

      {
        std::weak_ptr<dag_vertex> v_weak =
          get_dag().find_vertex_by_uuid(vertices_cloned[1].get_uuid());
        ASSERT_FALSE(v_weak.expired());
        EXPECT_NE(vertices_cloned[0], *(v_weak.lock()));
        EXPECT_EQ(vertices_cloned[1], *(v_weak.lock()));

        std::shared_ptr<dag_vertex> v = v_weak.lock();
        EXPECT_TRUE(get_dag().contains_vertex(*v));
        EXPECT_TRUE(get_dag().contains_vertex_by_uuid(v->get_uuid()));
        EXPECT_TRUE(get_dag().contains_vertex_by_label(v->label()));
      }

      {
        std::weak_ptr<dag_vertex> v_weak =
          get_dag().find_vertex_by_uuid(vertices_cloned[6].get_uuid());
        ASSERT_FALSE(v_weak.expired());
        EXPECT_NE(vertices_cloned[4], *(v_weak.lock()));
        EXPECT_NE(vertices_cloned[5], *(v_weak.lock()));
        EXPECT_EQ(vertices_cloned[6], *(v_weak.lock()));

        std::shared_ptr<dag_vertex> v = v_weak.lock();
        EXPECT_TRUE(get_dag().contains_vertex(*v));
        EXPECT_TRUE(get_dag().contains_vertex_by_uuid(v->get_uuid()));
        EXPECT_TRUE(get_dag().contains_vertex_by_label(v->label()));
      }

      {
        std::vector<std::weak_ptr<dag_vertex>> v_weaks =
          get_dag().find_all_verticies_with_label("2");
        EXPECT_EQ(3ul, v_weaks.size());
        EXPECT_EQ(vertices_cloned[4], *(v_weaks[0].lock()));
        EXPECT_EQ(vertices_cloned[5], *(v_weaks[1].lock()));
        EXPECT_EQ(vertices_cloned[6], *(v_weaks[2].lock()));

        for (std::weak_ptr<dag_vertex> v_weak : v_weaks) {
          std::shared_ptr<dag_vertex> v = v_weak.lock();
          EXPECT_TRUE(get_dag().contains_vertex(*v));
          EXPECT_TRUE(get_dag().contains_vertex_by_uuid(v->get_uuid()));
          EXPECT_TRUE(get_dag().contains_vertex_by_label(v->label()));
        }
      }

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestUDag, clone_with_multiple_vertices_no_connections)
    {
      std::vector<dag_vertex> vertices_cloned = fill_dag_default();
      dag dag_cloned = get_dag().clone();

      for (const dag_vertex &v : vertices_cloned) {
        EXPECT_TRUE(get_dag().contains_vertex(v));
        EXPECT_TRUE(dag_cloned.contains_vertex(v));
      }
      EXPECT_EQ(get_dag(), dag_cloned);

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestUDag, connect_and_acyclic_check)
    {
      std::vector<dag_vertex> vertices_cloned = fill_dag_default();

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

      get_dag().reset();
      EXPECT_EQ(0ul, get_dag().edge_count());
      EXPECT_EQ(0ul, get_dag().vertex_count());
    }

    TEST_F(TestUDag, clone_with_multiple_vertices_and_connections)
    {
    }
  }
}
