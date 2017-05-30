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

      dag &get_dag()
      {
        return d_;
      }

    private:
      dag d_;
    };

    TEST_F(TestUDag, equality_operator_via_clone_no_edges_with_reset)
    {
      dag d_clone = get_dag().clone();
      EXPECT_EQ(d_clone, get_dag());

      get_dag().add_vertex(std::move(dag_vertex("1")));
      get_dag().add_vertex(std::move(dag_vertex("2")));
      get_dag().add_vertex(std::move(dag_vertex("3")));
      get_dag().add_vertex(std::move(dag_vertex("4")));
      EXPECT_NE(d_clone, get_dag());

      d_clone = get_dag().clone();
      EXPECT_EQ(d_clone, get_dag());

      get_dag().reset();
      EXPECT_NE(d_clone, get_dag());
    }
  }
}
