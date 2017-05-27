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

    TEST(U_DAG_EDGE, ctor)
    {
      dag_edge e;
      EXPECT_EQ(dag_edge::status::initialized, e.current_status());
      EXPECT_EQ(std::string("initialized"), e.current_status_as_string());
      EXPECT_FALSE(e.uuid().as_string().empty());
    }

    TEST(U_DAG_EDGE, dtor)
    {
      dag_edge e;
      e.~dag_edge();
      EXPECT_EQ(dag_edge::status::non_traverable, e.current_status());
      EXPECT_FALSE(e.uuid().is_initialized());
    }

    TEST(U_DAG_EDGE, mtor)
    {
      dag_edge e1;
      dag_edge e2(std::move(e1));

      EXPECT_EQ(dag_edge::status::non_traverable, e1.current_status());
      EXPECT_FALSE(e1.uuid().is_initialized());
      EXPECT_EQ(dag_edge::status::initialized, e2.current_status());
      EXPECT_TRUE(e2.uuid().is_initialized());

      // Test safety of move ctor by moving an object that has already been
      // moved.
      dag_edge e3(std::move(e1));
      EXPECT_EQ(dag_edge::status::non_traverable, e3.current_status());
    }

    TEST(U_DAG_EDGE, massign)
    {
      dag_edge e1;
      dag_edge e2;
      e2 = std::move(e1);

      EXPECT_EQ(dag_edge::status::non_traverable, e1.current_status());
      EXPECT_FALSE(e1.uuid().is_initialized());
      EXPECT_EQ(dag_edge::status::initialized, e2.current_status());
      EXPECT_TRUE(e2.uuid().is_initialized());
    }

    TEST(U_DAG_EDGE, clone)
    {
      dag_edge e;
      auto v1 = std::make_shared<dag_vertex>("to");
      EXPECT_TRUE(e.connect_to(v1));

      dag_edge clone = e.clone();
      EXPECT_NE(clone.get_connection().lock(), e.get_connection().lock());
      // Note that since and edge contains a weak_ptr to the dag_vertex it
      // is connected to we cannot clone that since it would hold a pointer
      // to a dag_vertex in another graph.
      EXPECT_EQ(nullptr, clone.get_connection().lock());
      // The \pdag_vertex class would have to connect the edge to the other
      // \pdag_vertex that it is connected to.
      auto tmp = std::make_shared<dag_vertex>(
        e.get_connection().lock()->clone());
      clone.connect_to(tmp);
      EXPECT_NE(nullptr, clone.get_connection().lock());
      EXPECT_EQ(*(e.get_connection().lock()),
        *(clone.get_connection().lock()));

      // After reconnecting edge to clone the edges should be the same.
      EXPECT_EQ(e, clone);
    }

    TEST(U_DAG_EDGE, connections)
    {
      dag_edge e;
      auto v1 = std::make_shared<dag_vertex>("to");
      EXPECT_TRUE(e.connect_to(v1));
      EXPECT_TRUE(e.is_a_connection_to(*v1));

      // You cannot reuse v1 here because e has a weak ptr to v1. Killing
      // v1 will kill e's weak ref to v1. So we create a new vertex.
      auto v2 = std::make_shared<dag_vertex>("not to");
      EXPECT_FALSE(e.is_a_connection_to(*v2));

      // Kill e's connection. Evil things happend here.
      v1 = std::make_shared<dag_vertex>("from");
      EXPECT_FALSE(e.is_a_connection_to(*v1));
    }
  }
}
