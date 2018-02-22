#include <gtest/gtest.h>

#include "dag_scheduler/dag_edge.h"
#include "dag_scheduler/dag_vertex.h"

#include <iostream>

namespace com
{
  namespace dag_scheduler
  {
    class TestDagVertex : public ::testing::Test
    {
    protected:
      virtual void SetUp()
      {
        v_ptr_ = std::make_shared<dag_vertex>("TestDagVertexPtr");
        v_ = dag_vertex("TestDagVertex");
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

    TEST_F(TestDagVertex, ctor)
    {
      dag_vertex v;
      EXPECT_EQ(dag_vertex::status::initialized, v.current_status());
      EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
      EXPECT_EQ("", v.label());
      EXPECT_EQ(0ul, v.edge_count());
      EXPECT_FALSE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());
    }

    TEST_F(TestDagVertex, string_ctor)
    {
      dag_vertex v("1");
      EXPECT_EQ(dag_vertex::status::initialized, v.current_status());
      EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
      EXPECT_EQ("1", v.label());
      EXPECT_EQ(0ul, v.edge_count());
      EXPECT_FALSE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());
    }

    TEST_F(TestDagVertex, dtor_no_edges)
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

    TEST_F(TestDagVertex, move_ctor_no_edges)
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

    TEST_F(TestDagVertex, assignment_move_operator_no_edges)
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

    TEST_F(TestDagVertex, clone_no_edges)
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

    TEST_F(TestDagVertex, copy_ctor_no_edges)
    {
      dag_vertex v("1");
      dag_vertex v_copied(v);

      EXPECT_EQ(dag_vertex::status::initialized, v_copied.current_status());
      EXPECT_EQ(std::string("initialized"),
        v_copied.current_status_as_string());
      EXPECT_EQ("1", v_copied.label());
      EXPECT_EQ(0ul, v_copied.edge_count());
      EXPECT_FALSE(v_copied.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v_copied.incomming_edge_count());

      EXPECT_EQ(dag_vertex::status::initialized, v.current_status());
      EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
      EXPECT_EQ("1", v.label());
      EXPECT_EQ(0ul, v.edge_count());
      EXPECT_FALSE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());
    }

    TEST_F(TestDagVertex, assignment_operator_no_edges)
    {
      dag_vertex v("1");
      dag_vertex v_copied;
      v_copied = v;

      EXPECT_EQ(dag_vertex::status::initialized, v_copied.current_status());
      EXPECT_EQ(std::string("initialized"),
        v_copied.current_status_as_string());
      EXPECT_EQ("1", v_copied.label());
      EXPECT_EQ(0ul, v_copied.edge_count());
      EXPECT_FALSE(v_copied.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v_copied.incomming_edge_count());

      EXPECT_EQ(dag_vertex::status::initialized, v.current_status());
      EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
      EXPECT_EQ("1", v.label());
      EXPECT_EQ(0ul, v.edge_count());
      EXPECT_FALSE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());
    }

    TEST_F(TestDagVertex, connect_and_contains_connection)
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
      ASSERT_EQ(0ul, get_dag_vertex().edge_count());
    }

    TEST_F(TestDagVertex, visit_all_edges_points_to_actual_verticies)
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
      ASSERT_EQ(0ul, get_dag_vertex().edge_count());
    }

    TEST_F(TestDagVertex, clone_all_edges_and_restablish_connections)
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

      std::vector<dag_vertex::dag_vertex_connection> edges =
        get_dag_vertex().clone_all_connections();
      // First vertex connects to itself.
      EXPECT_EQ(connections.size(), edges.size());

      dag_vertex tmp = get_dag_vertex().clone();
      std::vector<std::shared_ptr<dag_vertex>> connections_cloned =
        tmp.restablish_connections(edges);
      EXPECT_EQ(get_dag_vertex(), tmp);

      std::size_t index = 0;
      get_dag_vertex().visit_all_edges([&](const dag_edge &e) {
          dag_edge &e_ref = *const_cast<dag_edge *>(&e);
          std::size_t j = 0;
          tmp.visit_all_edges([&](const dag_edge &e_clone) {
              if (j == index) {
                dag_edge &e_ref_clone = *const_cast<dag_edge *>(&e_clone);
                EXPECT_EQ(*(e_ref.get_connection().lock()),
                  *(e_ref_clone.get_connection().lock()));
              }
              ++j;
            }
          );
          ++index;
        }
      );

      get_dag_vertex().clear_edges();
      ASSERT_EQ(0ul, get_dag_vertex().edge_count());
    }

    TEST_F(TestDagVertex, has_incomming_edge_and_incomming_edge_count)
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
      EXPECT_EQ(0ul, get_dag_vertex_ptr()->edge_count());
      EXPECT_EQ(count, get_dag_vertex_ptr()->incomming_edge_count());
    }

    TEST_F(TestDagVertex, move_ctor_with_edges)
    {
      dag_vertex v("orig");

      std::vector<std::shared_ptr<dag_vertex>> connections = {
        std::make_shared<dag_vertex>("1"),
        std::make_shared<dag_vertex>("2"),
        std::make_shared<dag_vertex>("3"),
        std::make_shared<dag_vertex>("4"),
        std::make_shared<dag_vertex>("5"),
        std::make_shared<dag_vertex>("6"),
      };

      for (std::shared_ptr<dag_vertex> c : connections) {
        v.connect(c);
      }

      dag_vertex v_moved = std::move(v);

      std::size_t index = 0;
      v.visit_all_edges([&](const dag_edge &e) {
          dag_edge &e_ref = *const_cast<dag_edge *>(&e);
          EXPECT_NE(e_ref.get_connection().lock(), connections[index]);
          EXPECT_NE(*(e_ref.get_connection().lock()), *(connections[index]));
          ++index;
        }
      );

      index = 0;
      v_moved.visit_all_edges([&](const dag_edge &e) {
          dag_edge &e_ref = *const_cast<dag_edge *>(&e);
          EXPECT_EQ(e_ref.get_connection().lock(), connections[index]);
          EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
          ++index;
        }
      );

      EXPECT_EQ(dag_vertex::status::initialized, v_moved.current_status());
      EXPECT_EQ(std::string("initialized"),
        v_moved.current_status_as_string());
      EXPECT_EQ("orig", v_moved.label());
      EXPECT_EQ(connections.size(), v_moved.edge_count());
      EXPECT_FALSE(v_moved.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v_moved.incomming_edge_count());

      EXPECT_EQ(dag_vertex::status::invalid, v.current_status());
      EXPECT_EQ(std::string("invalid"), v.current_status_as_string());
      EXPECT_EQ("", v.label());
      EXPECT_EQ(0ul, v.edge_count());
      EXPECT_TRUE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());

      v.clear_edges();
      EXPECT_EQ(0ul, v.edge_count());

      v_moved.clear_edges();
      EXPECT_EQ(0ul, v_moved.edge_count());
    }

    TEST_F(TestDagVertex, assignment_move_operator_with_edges)
    {
      dag_vertex v("orig");
      dag_vertex v_moved;

      std::vector<std::shared_ptr<dag_vertex>> connections = {
        std::make_shared<dag_vertex>("1"),
        std::make_shared<dag_vertex>("2"),
        std::make_shared<dag_vertex>("3"),
        std::make_shared<dag_vertex>("4"),
        std::make_shared<dag_vertex>("5"),
        std::make_shared<dag_vertex>("6"),
      };

      for (std::shared_ptr<dag_vertex> c : connections) {
        v.connect(c);
      }

      v_moved = std::move(v);

      std::size_t index = 0;
      v.visit_all_edges([&](const dag_edge &e) {
          dag_edge &e_ref = *const_cast<dag_edge *>(&e);
          EXPECT_NE(e_ref.get_connection().lock(), connections[index]);
          EXPECT_NE(*(e_ref.get_connection().lock()), *(connections[index]));
          ++index;
        }
      );

      index = 0;
      v_moved.visit_all_edges([&](const dag_edge &e) {
          dag_edge &e_ref = *const_cast<dag_edge *>(&e);
          EXPECT_EQ(e_ref.get_connection().lock(), connections[index]);
          EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
          ++index;
        }
      );

      EXPECT_EQ(dag_vertex::status::initialized, v_moved.current_status());
      EXPECT_EQ(std::string("initialized"),
        v_moved.current_status_as_string());
      EXPECT_EQ("orig", v_moved.label());
      EXPECT_EQ(connections.size(), v_moved.edge_count());
      EXPECT_FALSE(v_moved.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v_moved.incomming_edge_count());

      EXPECT_EQ(dag_vertex::status::invalid, v.current_status());
      EXPECT_EQ(std::string("invalid"), v.current_status_as_string());
      EXPECT_EQ("", v.label());
      EXPECT_EQ(0ul, v.edge_count());
      EXPECT_TRUE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());

      v.clear_edges();
      EXPECT_EQ(0ul, v.edge_count());

      v_moved.clear_edges();
      EXPECT_EQ(0ul, v_moved.edge_count());
    }

    TEST_F(TestDagVertex, clone_with_edges)
    {
      dag_vertex v("orig");

      std::vector<std::shared_ptr<dag_vertex>> connections = {
        std::make_shared<dag_vertex>("1"),
        std::make_shared<dag_vertex>("2"),
        std::make_shared<dag_vertex>("3"),
        std::make_shared<dag_vertex>("4"),
        std::make_shared<dag_vertex>("5"),
        std::make_shared<dag_vertex>("6"),
      };

      for (std::shared_ptr<dag_vertex> c : connections) {
        v.connect(c);
      }
      ASSERT_EQ(connections.size(), v.edge_count());

      dag_vertex v_cloned = v.clone();
      ASSERT_EQ(0ul, v_cloned.edge_count());

      // We cannot add back the connections since the edge adds a weak_ptr
      // to a dag_vertex we no longer can duplicate. This has to be done
      // outside the class by the code that is cloning the dag_vertex.
      // dag_graph should be the object that orchestrates that.
      std::vector<dag_vertex::dag_vertex_connection> orig_connections =
      v.clone_all_connections();
      std::vector<std::shared_ptr<dag_vertex>> cloned_connections =
        v_cloned.restablish_connections(orig_connections);

      std::size_t v_edge_count = v.edge_count();
      std::size_t v_cloned_edge_count = v_cloned.edge_count();
      ASSERT_EQ(v_edge_count, v_cloned_edge_count);

      ASSERT_EQ(v, v_cloned);
      ASSERT_EQ(v_cloned, v);

      std::size_t index = 0;
      v.visit_all_edges([&](const dag_edge &e) {
          dag_edge &e_ref = *const_cast<dag_edge *>(&e);
          EXPECT_EQ(e_ref.get_connection().lock(), connections[index]);
          EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
          ++index;
        }
      );

      index = 0;
      v_cloned.visit_all_edges([&](const dag_edge &e) {
          dag_edge &e_ref = *const_cast<dag_edge *>(&e);
          EXPECT_NE(e_ref.get_connection().lock(), connections[index]);
          EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
          ++index;
          (void)e_ref;
        }
      );

      EXPECT_EQ(dag_vertex::status::initialized, v_cloned.current_status());
      EXPECT_EQ(std::string("initialized"),
        v_cloned.current_status_as_string());
      EXPECT_EQ("orig", v_cloned.label());
      EXPECT_EQ(connections.size(), v_cloned.edge_count());
      EXPECT_FALSE(v_cloned.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v_cloned.incomming_edge_count());

      EXPECT_EQ(dag_vertex::status::initialized, v.current_status());
      EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
      EXPECT_EQ("orig", v.label());
      EXPECT_EQ(connections.size(), v.edge_count());
      EXPECT_FALSE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());
    }

    TEST_F(TestDagVertex, add_incomming_edge)
    {
      dag_vertex v("orig");
      v.add_incomming_edge();
      EXPECT_EQ(1, v.incomming_edge_count());
    }

    TEST_F(TestDagVertex, sub_incomming_edge)
    {
      dag_vertex v("orig");
      v.add_incomming_edge();
      v.add_incomming_edge();
      v.sub_incomming_edge();
      EXPECT_EQ(1, v.incomming_edge_count());
    }

    TEST_F(TestDagVertex, clear_edges)
    {
      dag_vertex v("orig");
      std::vector<std::shared_ptr<dag_vertex>> connections = {
        std::make_shared<dag_vertex>("1"),
        std::make_shared<dag_vertex>("2"),
        std::make_shared<dag_vertex>("3"),
        std::make_shared<dag_vertex>("4"),
        std::make_shared<dag_vertex>("5"),
        std::make_shared<dag_vertex>("6"),
      };

      for (std::shared_ptr<dag_vertex> c : connections) {
        v.connect(c);
      }
      EXPECT_EQ(6, v.edge_count());
      v.clear_edges();
      EXPECT_EQ(0, v.edge_count());
    }

    TEST_F(TestDagVertex, reset_incomming_edge_count)
    {
      dag_vertex v("orig");
      v.add_incomming_edge();
      v.add_incomming_edge();
      v.add_incomming_edge();
      v.add_incomming_edge();
      v.add_incomming_edge();
      EXPECT_EQ(5, v.incomming_edge_count());
      v.reset_incomming_edge_count();
      EXPECT_EQ(0, v.incomming_edge_count());
    }

    TEST_F(TestDagVertex, get_edge_at)
    {
      dag_vertex v("orig");
      std::vector<std::shared_ptr<dag_vertex>> connections = {
        std::make_shared<dag_vertex>("1"),
        std::make_shared<dag_vertex>("2"),
        std::make_shared<dag_vertex>("3"),
        std::make_shared<dag_vertex>("4"),
        std::make_shared<dag_vertex>("5"),
        std::make_shared<dag_vertex>("6"),
      };

      for (std::shared_ptr<dag_vertex> c : connections) {
        v.connect(c);
      }

      // Make use_count the same.
      connections.clear();

      std::size_t index = 0;
      v.visit_all_edges([&](const dag_edge &e) {
          EXPECT_EQ(e, v.get_edge_at(index));
          ++index;
        }
      );
    }

    TEST_F(TestDagVertex, copy_ctor_with_edges)
    {
      dag_vertex v("orig");

      std::vector<std::shared_ptr<dag_vertex>> connections = {
        std::make_shared<dag_vertex>("1"),
        std::make_shared<dag_vertex>("2"),
        std::make_shared<dag_vertex>("3"),
        std::make_shared<dag_vertex>("4"),
        std::make_shared<dag_vertex>("5"),
        std::make_shared<dag_vertex>("6"),
      };

      for (std::shared_ptr<dag_vertex> c : connections) {
        v.connect(c);
      }
      ASSERT_EQ(connections.size(), v.edge_count());

      dag_vertex v_copied(v.clone());
      ASSERT_EQ(0ul, v_copied.edge_count());

      // We cannot add back the connections since the edge adds a weak_ptr
      // to a dag_vertex we no longer can duplicate. This has to be done
      // outside the class by the code that is cloning the dag_vertex.
      // dag_graph should be the object that orchestrates that.
      std::vector<dag_vertex::dag_vertex_connection> orig_connections =
      v.clone_all_connections();
      std::vector<std::shared_ptr<dag_vertex>> copied_connections =
        v_copied.restablish_connections(orig_connections);

      std::size_t v_edge_count = v.edge_count();
      std::size_t v_copied_edge_count = v_copied.edge_count();
      ASSERT_EQ(v_edge_count, v_copied_edge_count);

      ASSERT_EQ(v, v_copied);
      ASSERT_EQ(v_copied, v);

      std::size_t index = 0;
      v.visit_all_edges([&](const dag_edge &e) {
          dag_edge &e_ref = *const_cast<dag_edge *>(&e);
          EXPECT_EQ(e_ref.get_connection().lock(), connections[index]);
          EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
          ++index;
        }
      );

      index = 0;
      v_copied.visit_all_edges([&](const dag_edge &e) {
          dag_edge &e_ref = *const_cast<dag_edge *>(&e);
          EXPECT_NE(e_ref.get_connection().lock(), connections[index]);
          EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
          ++index;
          (void)e_ref;
        }
      );

      EXPECT_EQ(dag_vertex::status::initialized, v_copied.current_status());
      EXPECT_EQ(std::string("initialized"),
        v_copied.current_status_as_string());
      EXPECT_EQ("orig", v_copied.label());
      EXPECT_EQ(connections.size(), v_copied.edge_count());
      EXPECT_FALSE(v_copied.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v_copied.incomming_edge_count());

      EXPECT_EQ(dag_vertex::status::initialized, v.current_status());
      EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
      EXPECT_EQ("orig", v.label());
      EXPECT_EQ(connections.size(), v.edge_count());
      EXPECT_FALSE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());
    }

    TEST_F(TestDagVertex, assignment_operator_with_edges)
    {
      dag_vertex v("orig");

      std::vector<std::shared_ptr<dag_vertex>> connections = {
        std::make_shared<dag_vertex>("1"),
        std::make_shared<dag_vertex>("2"),
        std::make_shared<dag_vertex>("3"),
        std::make_shared<dag_vertex>("4"),
        std::make_shared<dag_vertex>("5"),
        std::make_shared<dag_vertex>("6"),
      };

      for (std::shared_ptr<dag_vertex> c : connections) {
        v.connect(c);
      }
      ASSERT_EQ(connections.size(), v.edge_count());

      dag_vertex v_copied;
      v_copied = v.clone();
      ASSERT_EQ(0ul, v_copied.edge_count());

      // We cannot add back the connections since the edge adds a weak_ptr
      // to a dag_vertex we no longer can duplicate. This has to be done
      // outside the class by the code that is cloning the dag_vertex. dag_graph
      // should be the object that orchestrates that.
      std::vector<dag_vertex::dag_vertex_connection> orig_connections =
      v.clone_all_connections();
      std::vector<std::shared_ptr<dag_vertex>> copied_connections =
        v_copied.restablish_connections(orig_connections);

      std::size_t v_edge_count = v.edge_count();
      std::size_t v_copied_edge_count = v_copied.edge_count();
      ASSERT_EQ(v_edge_count, v_copied_edge_count);

      ASSERT_EQ(v, v_copied);
      ASSERT_EQ(v_copied, v);

      std::size_t index = 0;
      v.visit_all_edges([&](const dag_edge &e) {
          dag_edge &e_ref = *const_cast<dag_edge *>(&e);
          EXPECT_EQ(e_ref.get_connection().lock(), connections[index]);
          EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
          ++index;
        }
      );

      index = 0;
      v_copied.visit_all_edges([&](const dag_edge &e) {
          dag_edge &e_ref = *const_cast<dag_edge *>(&e);
          EXPECT_NE(e_ref.get_connection().lock(), connections[index]);
          EXPECT_EQ(*(e_ref.get_connection().lock()), *(connections[index]));
          ++index;
          (void)e_ref;
        }
      );

      EXPECT_EQ(dag_vertex::status::initialized, v_copied.current_status());
      EXPECT_EQ(std::string("initialized"),
        v_copied.current_status_as_string());
      EXPECT_EQ("orig", v_copied.label());
      EXPECT_EQ(connections.size(), v_copied.edge_count());
      EXPECT_FALSE(v_copied.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v_copied.incomming_edge_count());

      EXPECT_EQ(dag_vertex::status::initialized, v.current_status());
      EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
      EXPECT_EQ("orig", v.label());
      EXPECT_EQ(connections.size(), v.edge_count());
      EXPECT_FALSE(v.get_uuid().as_string().empty());
      EXPECT_EQ(0ul, v.incomming_edge_count());
    }
  }
}
