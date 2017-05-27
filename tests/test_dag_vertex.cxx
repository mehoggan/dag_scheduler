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
      virtual void SetUp() {}
      virtual void TearDown() {}
    };

    TEST(U_DAG_VERTEX, string_ctor)
    {
      dag_vertex v("");
      EXPECT_EQ(dag_vertex::status::initialized, v.current_status());
      EXPECT_EQ(std::string("initialized"), v.current_status_as_string());
      EXPECT_FALSE(v.uuid().as_string().empty());
    }

    TEST(U_DAG_VERTEX, dtor)
    {
      dag_vertex v("");
      v.~dag_vertex();
      EXPECT_EQ(dag_vertex::status::invalid, v.current_status());
      EXPECT_EQ(std::string(""), v.label());
      EXPECT_FALSE(v.uuid().is_initialized());
    }

    TEST(U_DAG_VERTEX, mtor)
    {
      dag_vertex v1("foo");
      dag_vertex v2(std::move(v1));

      EXPECT_EQ(dag_vertex::status::invalid, v1.current_status());
      EXPECT_EQ(std::string(""), v1.label());
      EXPECT_FALSE(v1.uuid().is_initialized());
      EXPECT_EQ(dag_vertex::status::initialized, v2.current_status());
      EXPECT_EQ(std::string("foo"), v2.label());
      EXPECT_TRUE(v2.uuid().is_initialized());

      // Test safety of move ctor by moving an object that has already been
      // moved.
      dag_vertex v3(std::move(v1));
      EXPECT_EQ(dag_vertex::status::invalid, v3.current_status());
      EXPECT_EQ(std::string(""), v3.label());
    }

    TEST(U_DAG_VERTEX, massign)
    {
      dag_vertex v1("foo");
      dag_vertex v2("bar");
      v2 = std::move(v1);

      EXPECT_EQ(dag_vertex::status::invalid, v1.current_status());
      EXPECT_EQ(std::string(""), v1.label());
      EXPECT_FALSE(v1.uuid().is_initialized());
      EXPECT_EQ(dag_vertex::status::initialized, v2.current_status());
      EXPECT_EQ(std::string("foo"), v2.label());
      EXPECT_TRUE(v2.uuid().is_initialized());
    }

    TEST(U_DAG_VERTEX, clone)
    {
      dag_vertex v1("v1");
      dag_vertex v2("v2");
      dag_vertex v3("v3");
      dag_vertex v4("v4");
      dag_vertex v5("v5");
      dag_vertex v6("v6");
      dag_vertex v7("v7");

      std::vector<std::shared_ptr<dag_vertex>> adjacent_lists = {
        std::make_shared<dag_vertex>(v2.clone()),
        std::make_shared<dag_vertex>(v3.clone()),
        std::make_shared<dag_vertex>(v4.clone()),
        std::make_shared<dag_vertex>(v5.clone()),
        std::make_shared<dag_vertex>(v6.clone()),
        std::make_shared<dag_vertex>(v7.clone())
      };

      v1.connect(adjacent_lists[0]);
      EXPECT_TRUE(v1.contains_connection_to(*adjacent_lists[0]));
      v1.connect(adjacent_lists[1]);
      EXPECT_TRUE(v1.contains_connection_to(*adjacent_lists[1]));
      v1.connect(adjacent_lists[2]);
      EXPECT_TRUE(v1.contains_connection_to(*adjacent_lists[2]));
      v1.connect(adjacent_lists[3]);
      EXPECT_TRUE(v1.contains_connection_to(*adjacent_lists[3]));
      v1.connect(adjacent_lists[4]);
      EXPECT_TRUE(v1.contains_connection_to(*adjacent_lists[4]));
      v1.connect(adjacent_lists[5]);
      EXPECT_TRUE(v1.contains_connection_to(*adjacent_lists[5]));

      ASSERT_EQ(6, v1.edge_count());

      dag_vertex clone = v1.clone();

      ASSERT_EQ(6, v1.edge_count());
      ASSERT_EQ(6, clone.edge_count());

      std::size_t index = 0;
      clone.visit_all_edges([&](const dag_edge &e) {
          uintptr_t clone_address =
            reinterpret_cast<uintptr_t>(&e);
          uintptr_t v1_e_address =
            reinterpret_cast<uintptr_t>(&(v1.get_edge(index)));
          EXPECT_NE(v1_e_address, clone_address);
          EXPECT_EQ(e, v1.get_edge(index));

          ++index;
        }
      );
    }

    TEST(U_DAG_VERTEX, uniqueness)
    {
      dag_vertex v1("");
      dag_vertex v2("");

      // Not equal because each vertex has a UUID.
      EXPECT_NE(v1, v2);
    }

    TEST(U_DAG_VERTEX, update_status_to)
    {
      dag_vertex v("");
      v.update_status_to(dag_vertex::status::invalid);
      EXPECT_EQ(dag_vertex::status::initialized, v.current_status());
    }

    TEST(U_DAG_VERTEX, contains_connection_to)
    {
      dag_vertex v1("");
      dag_vertex v2("");

      EXPECT_FALSE(v1.contains_connection_to(v2));
    }

    TEST(U_DAG_VERTEX, connect)
    {
      dag_vertex v1("v1");

      auto v2 = std::make_shared<dag_vertex>("v2");
      EXPECT_TRUE(v1.connect(v2));
      // A dag_vertex has a uuid assigned to it at creation time so label is not
      // the only unique id.
      EXPECT_FALSE(v1.contains_connection_to(dag_vertex(v2->label())));
      // Note how this actually passes now that we use the original vertex.
      EXPECT_TRUE(v1.contains_connection_to(*v2));
    }
  }
}
