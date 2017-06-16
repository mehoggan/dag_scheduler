#ifndef DAG_VERTEX_H_INCLUDED
#define DAG_VERTEX_H_INCLUDED

#include "declspec.h"

#include "u_dagtasks/uuid.h"

#include <atomic>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <gtest/gtest_prod.h>

namespace uber
{
  namespace u_dagtasks
  {
    class dag_edge;
    class dag;

    class DLLSPEC_U_DAGTASKS dag_vertex
    {
    private:
      friend class dag_edge;
      friend class dag;
      friend struct dag_vertex_connection;

      FRIEND_TEST(TestUDagVertex, connect_and_contains_connection);
      FRIEND_TEST(TestUDagVertex,
        visit_all_edges_points_to_actual_verticies);
      FRIEND_TEST(TestUDagVertex, clone_all_edges_and_restablish_connections);
      FRIEND_TEST(TestUDagVertex, move_ctor_with_edges);
      FRIEND_TEST(TestUDagVertex, assignment_move_operator_with_edges);
      FRIEND_TEST(TestUDagVertex, copy_ctor_no_edges);
      FRIEND_TEST(TestUDagVertex, assignment_operator_no_edges);
      FRIEND_TEST(TestUDagVertex, clone_all_edges);
      FRIEND_TEST(TestUDagVertex, copy_ctor_with_edges);
      FRIEND_TEST(TestUDagVertex, assignment_operator_with_edges);

    public:
      struct dag_vertex_connection
      {
      public:
        dag_vertex_connection(); // For std containers.
        dag_vertex_connection(dag_edge &edge, dag_vertex &vertex);

        const dag_edge &edge() const;
        const dag_vertex &vertex() const;

      private:
        std::unique_ptr<dag_edge> edge_;
        std::unique_ptr<dag_vertex> vertex_;
      };

      // TODO: Use dag class to manage status.
      enum class status
      {
        initialized,
        scheduled,
        running,
        passed,
        failed,
        invalid
      };

    public:
      dag_vertex();
      explicit dag_vertex(const std::string &label);
      virtual ~dag_vertex();

      dag_vertex(dag_vertex &&other);
      dag_vertex &operator=(dag_vertex &&rhs);

      dag_vertex clone();

      bool connect(std::shared_ptr<dag_vertex> other);
      bool contains_connection_to(const dag_vertex &other);
      std::vector<std::shared_ptr<dag_vertex>> restablish_connections(
        std::vector<dag_vertex_connection> &connections);

      std::size_t edge_count() const;

      void visit_all_edges(std::function<void (const dag_edge &)> cb) const;
      std::vector<dag_vertex_connection> clone_all_connections();

      bool has_incomming_edges() const;
      std::size_t incomming_edge_count() const;

      const uuid &get_uuid() const;
      const status &current_status() const;
      std::string current_status_as_string() const;
      const std::string &label() const;

    protected:
      void add_incomming_edge();
      void sub_incomming_edge();
      void clear_edges();
      void reset_incomming_edge_count();

      dag_vertex(const dag_vertex &other);
      dag_vertex &operator=(const dag_vertex &rhs);

    private:
      class uuid uuid_;
      status current_status_;
      std::string label_;
      std::vector<std::unique_ptr<dag_edge>> edges_;
      std::atomic<std::size_t> incomming_edge_count_;

    public:
      friend std::ostream &operator<<(std::ostream &out,
        const dag_vertex &v);
      friend bool operator==(const dag_vertex &lhs, const dag_vertex &rhs);
      friend bool operator!=(const dag_vertex &lhs, const dag_vertex &rhs);
    };
  }
}
#endif
