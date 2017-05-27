#include <gtest/gtest.h>

#include "u_dagtasks/dag.h"

#include <iostream>

namespace uber
{
  namespace u_dagtasks
  {
    class TestUDag : public ::testing::Test
    {
    protected:
      virtual void SetUp() {}
      virtual void TearDown() {}
    };

    TEST(U_DAG, connection_would_make_cyclic)
    {
      dag d;
      dag_vertex v1("1");
      dag_vertex v1_clone = v1.clone();
      d.add_vertex(v1); // Add moves v1.

      dag_vertex v2("2");
      dag_vertex v2_clone = v2.clone();
      d.add_vertex(v2); // Add moves v2

      dag_vertex v3("3");
      dag_vertex v3_clone = v3.clone();
      d.add_vertex(v3); // Add moves v3

      EXPECT_FALSE(d.are_connected(v1_clone, v2_clone));
      try {
        d.connect(v1_clone, v2_clone);
      } catch (std::exception &e) {
        std::cout << "ERROR: Exception = " << e.what() << std::endl;
      }
      EXPECT_TRUE(d.are_connected(v1_clone, v2_clone));

      EXPECT_FALSE(d.are_connected(v2_clone, v3_clone));
      try {
        d.connect(v2_clone, v3_clone);
      } catch (std::exception &e) {
        std::cout << "ERROR: Exception = " << e.what() << std::endl;
      }
      EXPECT_TRUE(d.are_connected(v2, v3));

      /*
      EXPECT_TRUE(d.connection_would_make_cyclic(v3, v1));
      EXPECT_FALSE(d.connection_would_make_cyclic(v1, v3));
      */
    }

    /*
    TEST(U_DAG, ctor_and_not_contains)
    {
      dag d;
      EXPECT_EQ(false, d.contains_vertex_by_label("not there"));
    }

    TEST(U_DAG, clone)
    {

      dag d;

      dag_vertex v1("1");
      dag_vertex v1_clone = v1.clone();
      d.add_vertex(v1);

      dag_vertex v2("2");
      dag_vertex v2_clone = v2.clone();
      d.add_vertex(v2);

      dag_vertex v3("3");
      dag_vertex v3_clone = v3.clone();
      d.add_vertex(v3);

      dag_vertex v4("4");
      dag_vertex v4_clone = v4.clone();
      d.add_vertex(v4);

      dag_vertex v5("5");
      dag_vertex v5_clone = v5.clone();
      d.add_vertex(v5);

      dag_vertex v6("6");
      dag_vertex v6_clone = v6.clone();
      d.add_vertex(v6);

      dag clone = d.clone();

      //ASSERT_EQ(6, d.vertex_count());
      //ASSERT_EQ(2, d.edge_count());

      //ASSERT_EQ(6, clone.vertex_count());
      //ASSERT_EQ(2, clone.edge_count());
    }

    TEST(U_DAG, contains)
    {
      dag d;

      dag_vertex v1("label_1");
      d.add_vertex(v1);
      EXPECT_FALSE(d.contains_vertex_by_label(v1.label())) << "Remember "
        << " move sematics are used here (label=--" << v1.label() << "--).";
      EXPECT_TRUE(d.contains_vertex_by_label("label_1"));
      EXPECT_FALSE(d.contains_vertex_by_label("label_2"));
      dag_vertex v2("label_2");
      d.add_vertex(v2);
      EXPECT_TRUE(d.contains_vertex_by_label("label_2"));
    }

    TEST(U_DAG, add_vertex)
    {
      dag d;

      dag_vertex v1("label_1");
      dag_vertex v2("label_2");
      dag_vertex v1_dup("label_1");

      EXPECT_TRUE(d.add_vertex(v1));
      EXPECT_TRUE(d.add_vertex(v2));
      EXPECT_FALSE(d.add_vertex(v1_dup));
    }

    TEST(U_DAG, find)
    {
      dag d;

      dag_vertex v1("label_1");
      d.add_vertex(v1);
      EXPECT_TRUE(d.find_vertex_by_label(v1.label()).expired())
        << "Remember move sematics are used here (label=--"
        << v1.label() << "--).";

      auto label_ptr = d.find_vertex_by_label("label_1");

      EXPECT_FALSE(label_ptr.expired());
      EXPECT_EQ("label_1", label_ptr.lock()->label());

      label_ptr = d.find_vertex_by_label("label_2");
      EXPECT_TRUE(label_ptr.expired());

      dag_vertex v2("label_2");
      d.add_vertex(v2);
      label_ptr = d.find_vertex_by_label("label_2");
      EXPECT_FALSE(label_ptr.expired());
    }

    TEST(U_DAG, connect)
    {
      dag d;

      dag_vertex v1("label_1");
      dag_vertex v2("label_2");

      d.add_and_connect(v1, v2);

      EXPECT_TRUE(d.are_connected_by_label("label_1", "label_2"));

      dag_vertex v3("label_3");
      d.add_vertex(v3);
      d.connect_by_label("label_1", "label_3");

      EXPECT_TRUE(d.are_connected_by_label("label_1", "label_3"));
    }
    */
  }
}
