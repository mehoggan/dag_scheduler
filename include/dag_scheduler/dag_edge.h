#ifndef DAG_EDGE_H_INCLUDED
#define DAG_EDGE_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/dag_vertex.h"
#include "dag_scheduler/uuid.h"

#include <functional>
#include <list>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <gtest/gtest_prod.h>

namespace com
{
  namespace dag_scheduler
  {
    class DLLSPEC_DAGTASKS dag_edge
    {
    private:
      friend class dag;
      friend class dag_vertex;
      friend struct dag_vertex::dag_vertex_connection;
      friend bool dag_topological_sort(dag &g,
        std::list<dag_vertex> &sorted_vertices);

      FRIEND_TEST(TestUDagEdge, copy_ctor);
      FRIEND_TEST(TestUDagEdge, assignment_operator);
      FRIEND_TEST(TestUDagEdge, connect_and_clone);
      FRIEND_TEST(TestUDagEdge, connect_to_null);
      FRIEND_TEST(TestUDagEdge, connections);
      FRIEND_TEST(TestUDagEdge, equality_operators_with_connection);

    public:
      // TODO: Use dag class to manage status.
      enum class status
      {
        initialized,
        traversed,
        non_traverable
      };

    public:
      dag_edge();
      virtual ~dag_edge();

      dag_edge(dag_edge &&other);
      dag_edge &operator=(dag_edge &&rhs);
      dag_edge clone();

      std::weak_ptr<dag_vertex> get_connection();
      bool is_a_connection_to(const dag_vertex &v) const;

      const uuid &get_uuid() const;
      const status &current_status() const;
      std::string current_status_as_string() const;

    protected:
      bool connect_to(std::shared_ptr<dag_vertex> v);

      dag_edge(const dag_edge &other);
      dag_edge &operator=(const dag_edge &rhs);

    private:
      class uuid uuid_;
      status current_status_;
      std::weak_ptr<dag_vertex> connection_;

    public:
      friend std::ostream &operator<<(std::ostream &out, const dag_edge &e);
      friend bool operator==(const dag_edge &lhs, const dag_edge &rhs);
      friend bool operator!=(const dag_edge &lhs, const dag_edge &rhs);
    };
  }
}
#endif
