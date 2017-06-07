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
  }
}
