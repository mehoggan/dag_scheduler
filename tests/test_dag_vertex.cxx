#include <gtest/gtest.h>

#include "u_dagtasks/dag_edge.h"
#include "u_dagtasks/dag_vertex.h"

#include <iostream>

namespace uber
{
  namespace u_dagtasks
  {
    class TestUDagVertex : public ::testing::Test
    {
    protected:
      virtual void SetUp()
      {
        v_ptr_ = std::make_shared<dag_vertex>("TestUDagVertexPtr");
        v_ = dag_vertex("TestUDagVertex");
      }

      virtual void TearDown()
      {
        v_ptr_.reset();
      }

      dag_vertex &get_dag_vertex()
      {
        return v_;
      }

      std::shared_ptr<dag_vertex> get_dag_vertex_ptr()
      {
        return v_ptr_;
      }

    private:
      std::shared_ptr<dag_vertex> v_ptr_;
      dag_vertex v_;
    };

    TEST_F(TestUDagVertex, ctor)
    {
      dag_vertex v;
      EXPECT_EQ(dag_vertex::status::initialized, v.current_status());
      EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
      EXPECT_EQ("", v.label());
      EXPECT_EQ(0ul, v.edge_count());
      EXPECT_FALSE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());
    }

    TEST_F(TestUDagVertex, string_ctor)
    {
      dag_vertex v("1");
      EXPECT_EQ(dag_vertex::status::initialized, v.current_status());
      EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
      EXPECT_EQ("1", v.label());
      EXPECT_EQ(0ul, v.edge_count());
      EXPECT_FALSE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());
    }

    TEST_F(TestUDagVertex, dtor_no_edges)
    {
      dag_vertex v("1");
      v.~dag_vertex();
      EXPECT_EQ(dag_vertex::status::invalid, v.current_status());
      EXPECT_EQ(std::string("invalid"), v.current_status_as_string());
      EXPECT_EQ("", v.label());
      EXPECT_EQ(0ul, v.edge_count());
      EXPECT_TRUE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());
    }

    TEST_F(TestUDagVertex, move_ctor_no_edges)
    {
      dag_vertex v("1");
      dag_vertex v_moved = std::move(v);

      EXPECT_EQ(dag_vertex::status::initialized, v_moved.current_status());
      EXPECT_EQ(std::string("initialized"),
        v_moved.current_status_as_string());
      EXPECT_EQ("1", v_moved.label());
      EXPECT_EQ(0ul, v_moved.edge_count());
      EXPECT_FALSE(v_moved.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v_moved.incomming_edge_count());

      EXPECT_EQ(dag_vertex::status::invalid, v.current_status());
      EXPECT_EQ(std::string("invalid"), v.current_status_as_string());
      EXPECT_EQ("", v.label());
      EXPECT_EQ(0ul, v.edge_count());
      EXPECT_TRUE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());
    }

    TEST_F(TestUDagVertex, assignment_move_operator_no_edges)
    {
      dag_vertex v("1");
      dag_vertex v_moved;
      v_moved = std::move(v);

      EXPECT_EQ(dag_vertex::status::initialized, v_moved.current_status());
      EXPECT_EQ(std::string("initialized"),
        v_moved.current_status_as_string());
      EXPECT_EQ("1", v_moved.label());
      EXPECT_EQ(0ul, v_moved.edge_count());
      EXPECT_FALSE(v_moved.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v_moved.incomming_edge_count());

      EXPECT_EQ(dag_vertex::status::invalid, v.current_status());
      EXPECT_EQ(std::string("invalid"), v.current_status_as_string());
      EXPECT_EQ("", v.label());
      EXPECT_EQ(0ul, v.edge_count());
      EXPECT_TRUE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());
    }

    TEST_F(TestUDagVertex, clone_no_edges)
    {
      dag_vertex v("1");
      dag_vertex v_cloned = v.clone();

      EXPECT_EQ(dag_vertex::status::initialized, v_cloned.current_status());
      EXPECT_EQ(std::string("initialized"),
        v_cloned.current_status_as_string());
      EXPECT_EQ("1", v_cloned.label());
      EXPECT_EQ(0ul, v_cloned.edge_count());
      EXPECT_FALSE(v_cloned.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v_cloned.incomming_edge_count());

      EXPECT_EQ(dag_vertex::status::initialized, v.current_status());
      EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
      EXPECT_EQ("1", v.label());
      EXPECT_EQ(0ul, v.edge_count());
      EXPECT_FALSE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());
    }

    TEST_F(TestUDagVertex, connect_and_contains_connection)
    {
      {
        std::shared_ptr<dag_vertex> v = std::make_shared<dag_vertex>("2");
        EXPECT_TRUE(get_dag_vertex().connect(v));
        EXPECT_TRUE(get_dag_vertex().contains_connection_to(*v));
        EXPECT_EQ(1ul, get_dag_vertex().edge_count());
      }
      {
        std::shared_ptr<dag_vertex> v = std::make_shared<dag_vertex>("2");
        EXPECT_TRUE(get_dag_vertex().connect(v));
        EXPECT_TRUE(get_dag_vertex().contains_connection_to(*v));
        EXPECT_EQ(2ul, get_dag_vertex().edge_count());
      }
      {
        std::shared_ptr<dag_vertex> v = std::make_shared<dag_vertex>("3");
        EXPECT_TRUE(get_dag_vertex().connect(v));
        EXPECT_TRUE(get_dag_vertex().contains_connection_to(*v));
        EXPECT_EQ(3ul, get_dag_vertex().edge_count());
      }
      {
        std::shared_ptr<dag_vertex> dup =
          std::make_shared<dag_vertex>("dup");
        EXPECT_TRUE(get_dag_vertex().connect(dup));
        EXPECT_TRUE(get_dag_vertex().contains_connection_to(*dup));
        EXPECT_EQ(4ul, get_dag_vertex().edge_count());
        EXPECT_FALSE(get_dag_vertex().connect(dup));
        EXPECT_TRUE(get_dag_vertex().contains_connection_to(*dup));
        EXPECT_EQ(4ul, get_dag_vertex().edge_count());
      }
      get_dag_vertex().clear_edges();
      ASSERT_EQ(0, get_dag_vertex().edge_count());
    }

    TEST_F(TestUDagVertex, visit_all_edges_points_to_actual_verticies)
    {
      std::vector<std::shared_ptr<dag_vertex>> connections = {
        std::make_shared<dag_vertex>("1"),
        std::make_shared<dag_vertex>("2"),
        std::make_shared<dag_vertex>("3"),
        std::make_shared<dag_vertex>("4"),
        std::make_shared<dag_vertex>("5"),
        std::make_shared<dag_vertex>("6"),
      };

      for (std::shared_ptr<dag_vertex> v : connections) {
        get_dag_vertex().connect(v);
      }
      EXPECT_EQ(connections.size(), get_dag_vertex().edge_count());

      std::size_t index = 0;
      get_dag_vertex().visit_all_edges([&](const dag_edge &e) {
          dag_edge &e_ref = *const_cast<dag_edge *>(&e);
          EXPECT_EQ(connections[index], e_ref.get_connection().lock());
          EXPECT_EQ(*connections[index], *(e_ref.get_connection().lock()));
          ++index;
        }
      );
      EXPECT_EQ(get_dag_vertex().edge_count(), index);

      get_dag_vertex().clear_edges();
      ASSERT_EQ(0, get_dag_vertex().edge_count());
    }

    TEST_F(TestUDagVertex, has_incomming_edge_and_incomming_edge_count)
    {
      std::vector<std::shared_ptr<dag_vertex>> connections = {
        std::make_shared<dag_vertex>("1"),
        std::make_shared<dag_vertex>("2"),
        std::make_shared<dag_vertex>("3"),
        std::make_shared<dag_vertex>("4"),
        std::make_shared<dag_vertex>("5"),
        std::make_shared<dag_vertex>("6"),
      };

      std::size_t count = 0;
      for (std::shared_ptr<dag_vertex> v : connections) {
        EXPECT_TRUE((*v).connect(get_dag_vertex_ptr()));
        ++count;
        EXPECT_EQ(count, get_dag_vertex_ptr()->incomming_edge_count());
        EXPECT_TRUE(get_dag_vertex_ptr()->has_incomming_edges());

        EXPECT_EQ(1ul, v->edge_count());
        v->visit_all_edges([&](const dag_edge &e) {
            dag_edge &e_ref = *const_cast<dag_edge *>(&e);
            EXPECT_EQ(e_ref.get_connection().lock(), get_dag_vertex_ptr());
            EXPECT_EQ(*(e_ref.get_connection().lock()),
              *(get_dag_vertex_ptr()));
          }
        );
      }
      EXPECT_EQ(0, get_dag_vertex_ptr()->edge_count());
      EXPECT_EQ(count, get_dag_vertex_ptr()->incomming_edge_count());
    }
  }
}
