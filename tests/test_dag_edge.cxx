#include <gtest/gtest.h>

#include "u_dagtasks/dag_edge.h"
#include "u_dagtasks/dag_vertex.h"

#include <iostream>
#include <memory>

namespace uber
{
  namespace u_dagtasks
  {
    class TestUDagEdge : public ::testing::Test
    {
    protected:
      virtual void SetUp() {}
      virtual void TearDown() {}
    };

    TEST_F(TestUDagEdge, ctor)
    {
      dag_edge e;
      EXPECT_EQ(dag_edge::status::initialized, e.current_status());
      EXPECT_EQ(std::string("initialized"), e.current_status_as_string());
      EXPECT_FALSE(e.get_uuid().as_string().empty());
    }

    TEST_F(TestUDagEdge, dtor)
    {
      dag_edge e;
      e.~dag_edge();
      EXPECT_EQ(dag_edge::status::non_traverable, e.current_status());
      EXPECT_FALSE(e.get_uuid().is_initialized());
      EXPECT_EQ(nullptr, e.get_connection().lock().get());
    }

    TEST_F(TestUDagEdge, mtor)
    {
      dag_edge e1;
      dag_edge e2(std::move(e1));

      EXPECT_EQ(dag_edge::status::non_traverable, e1.current_status());
      EXPECT_FALSE(e1.get_uuid().is_initialized());
      EXPECT_EQ(nullptr, e1.get_connection().lock().get());

      EXPECT_EQ(dag_edge::status::initialized, e2.current_status());
      EXPECT_TRUE(e2.get_uuid().is_initialized());
      EXPECT_EQ(nullptr, e2.get_connection().lock().get());

      // Test safety of move ctor by moving an object that has already been
      // moved.
      dag_edge e3(std::move(e1));
      EXPECT_EQ(dag_edge::status::non_traverable, e3.current_status());
      EXPECT_EQ(nullptr, e3.get_connection().lock().get());
    }

    TEST_F(TestUDagEdge, massign)
    {
      dag_edge e1;
      dag_edge e2;
      e2 = std::move(e1);

      EXPECT_EQ(dag_edge::status::non_traverable, e1.current_status());
      EXPECT_FALSE(e1.get_uuid().is_initialized());
      EXPECT_EQ(nullptr, e1.get_connection().lock().get());

      EXPECT_EQ(dag_edge::status::initialized, e2.current_status());
      EXPECT_TRUE(e2.get_uuid().is_initialized());
      EXPECT_EQ(nullptr, e2.get_connection().lock().get());
    }

    TEST_F(TestUDagEdge, connect_and_clone)
    {
      dag_edge e;
      std::shared_ptr<dag_vertex> v1 = std::make_shared<dag_vertex>("to");
      EXPECT_TRUE(e.connect_to(v1));
      dag_edge clone = e.clone();

      EXPECT_NE(clone.get_connection().lock(), e.get_connection().lock());
      EXPECT_NE(nullptr, e.get_connection().lock().get());
      EXPECT_EQ("to", e.get_connection().lock()->label());
      EXPECT_EQ(2u, e.get_connection().lock().use_count());
      EXPECT_EQ(nullptr, clone.get_connection().lock());

      std::shared_ptr<dag_vertex> tmp = std::make_shared<dag_vertex>(
        e.get_connection().lock()->clone());
      clone.connect_to(tmp);
      std::uintptr_t clone_connect_addr = reinterpret_cast<std::uintptr_t>(
        &(*(clone.get_connection().lock().get())));
      std::uintptr_t e_connect_addr = reinterpret_cast<std::uintptr_t>(
        &(*(e.get_connection().lock().get())));

      EXPECT_NE(clone_connect_addr, e_connect_addr);
      EXPECT_NE(nullptr, clone.get_connection().lock());
      EXPECT_EQ(*(e.get_connection().lock()),
        *(clone.get_connection().lock()));
      EXPECT_EQ(2u, e.get_connection().lock().use_count());
      EXPECT_EQ(2u, clone.get_connection().lock().use_count());
      EXPECT_EQ("to", e.get_connection().lock()->label());
      EXPECT_EQ("to", clone.get_connection().lock()->label());

      // After reconnecting edge to clone the edges should be the same.
      EXPECT_EQ(e, clone);
    }

    TEST_F(TestUDagEdge, connect_to_null)
    {
      dag_edge e;
      std::shared_ptr<dag_vertex> v1 = std::make_shared<dag_vertex>("to");
      EXPECT_TRUE(e.connect_to(v1));
      EXPECT_EQ(e.get_connection().lock(), v1);
      EXPECT_EQ(1ul, v1->incomming_edge_count());
      EXPECT_EQ(2l, e.get_connection().lock().use_count());
      EXPECT_FALSE(e.connect_to(nullptr));
      EXPECT_EQ(nullptr, e.get_connection().lock());
      EXPECT_EQ(0ul, v1->incomming_edge_count());
    }

    TEST_F(TestUDagEdge, connections)
    {
      dag_edge e;
      std::shared_ptr<dag_vertex> v1 = std::make_shared<dag_vertex>("to");
      EXPECT_TRUE(e.connect_to(v1));
      EXPECT_TRUE(e.is_a_connection_to(*v1));

      std::shared_ptr<dag_vertex> v2 = std::make_shared<dag_vertex>("ot");
      EXPECT_FALSE(e.is_a_connection_to(*v2));

      v1 = std::make_shared<dag_vertex>("from");
      EXPECT_FALSE(e.is_a_connection_to(*v1));
    }

    TEST_F(TestUDagEdge, copy_ctor)
    {
      dag_edge e;
      dag_edge e_copy(e);

      EXPECT_EQ(dag_edge::status::initialized, e.current_status());
      EXPECT_TRUE(e.get_uuid().is_initialized());
      EXPECT_EQ(nullptr, e.get_connection().lock().get());

      EXPECT_EQ(dag_edge::status::initialized, e_copy.current_status());
      EXPECT_TRUE(e_copy.get_uuid().is_initialized());
      EXPECT_EQ(nullptr, e_copy.get_connection().lock().get());

      EXPECT_EQ(e, e_copy);

      std::shared_ptr<dag_vertex> v1 = std::make_shared<dag_vertex>("to");
      EXPECT_TRUE(e.connect_to(v1));

      EXPECT_NE(e, e_copy);

      dag_edge e_copy_post_connect(e);
      std::shared_ptr<dag_vertex> v1_clone =
        std::make_shared<dag_vertex>(v1->clone());
      EXPECT_NE(e, e_copy_post_connect);
      e_copy_post_connect.connect_to(v1_clone);
      EXPECT_EQ(e, e_copy_post_connect);
    }

    TEST_F(TestUDagEdge, assignment_operator)
    {
      dag_edge e;
      dag_edge e_copy;
      e_copy = e;

      EXPECT_EQ(dag_edge::status::initialized, e.current_status());
      EXPECT_TRUE(e.get_uuid().is_initialized());
      EXPECT_EQ(nullptr, e.get_connection().lock().get());

      EXPECT_EQ(dag_edge::status::initialized, e_copy.current_status());
      EXPECT_TRUE(e_copy.get_uuid().is_initialized());
      EXPECT_EQ(nullptr, e_copy.get_connection().lock().get());

      EXPECT_EQ(e, e_copy);

      std::shared_ptr<dag_vertex> v1 = std::make_shared<dag_vertex>("to");
      EXPECT_TRUE(e.connect_to(v1));

      EXPECT_NE(e, e_copy);

      dag_edge e_copy_post_connect;
      e_copy_post_connect = e;
      std::shared_ptr<dag_vertex> v1_clone =
        std::make_shared<dag_vertex>(v1->clone());
      EXPECT_NE(e, e_copy_post_connect);
      e_copy_post_connect.connect_to(v1_clone);
      EXPECT_EQ(e, e_copy_post_connect);
    }

    TEST_F(TestUDagEdge, equality_operators_no_connection)
    {
      dag_edge e;
      dag_edge e_copy = e.clone();

      EXPECT_EQ(e, e_copy);
      EXPECT_EQ(e_copy, e);
      EXPECT_FALSE(e != e_copy);
      EXPECT_FALSE(e_copy != e);
    }

    TEST_F(TestUDagEdge, equality_operators_with_connection)
    {
      dag_edge e;
      dag_edge e_clone = e.clone();
      std::shared_ptr<dag_vertex> v1 = std::make_shared<dag_vertex>("to");
      std::shared_ptr<dag_vertex> v1_clone = std::make_shared<dag_vertex>(
        v1->clone()
      );
      e.connect_to(v1);
      e_clone.connect_to(v1_clone);

      EXPECT_EQ(e, e_clone);
      EXPECT_EQ(e_clone, e);
      EXPECT_FALSE(e != e_clone);
      EXPECT_FALSE(e_clone != e);
    }
  }
}
