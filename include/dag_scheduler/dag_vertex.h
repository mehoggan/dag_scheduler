#ifndef DAG_VERTEX_H_INCLUDED
#define DAG_VERTEX_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/uuid.h"

#include <atomic>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <gtest/gtest_prod.h>

namespace com
{
  namespace dag_scheduler
  {
    class dag_edge;
    class dag;

    //! A class that represents a vertex or node in a acyclic graph (dag).
    /*!
      A class that represents a vertex or node in a acyclic graph (dag) and
      operations that can be performed on it. Users can check for connected
      \ref dag_vertex and get access to the connected \ref dag_vertex. In
      addition users can connect instance of \ref dag_vertex, find out
      how many \ref dag_edges come from an instance, and check to see how
      many \ref dag_edges point at a given instance.
    */
    class DLLSPEC_DAGTASKS dag_vertex
    {
    private:
      friend class dag_edge;
      friend class dag;
      friend struct dag_vertex_connection;

      FRIEND_TEST(TestDagVertex, connect_and_contains_connection);
      FRIEND_TEST(TestDagVertex,
        visit_all_edges_points_to_actual_verticies);
      FRIEND_TEST(TestDagVertex, clone_all_edges_and_restablish_connections);
      FRIEND_TEST(TestDagVertex, move_ctor_with_edges);
      FRIEND_TEST(TestDagVertex, assignment_move_operator_with_edges);
      FRIEND_TEST(TestDagVertex, copy_ctor_no_edges);
      FRIEND_TEST(TestDagVertex, assignment_operator_no_edges);
      FRIEND_TEST(TestDagVertex, clone_all_edges);
      FRIEND_TEST(TestDagVertex, copy_ctor_with_edges);
      FRIEND_TEST(TestDagVertex, assignment_operator_with_edges);
      FRIEND_TEST(TestDagVertex, add_incomming_edge);
      FRIEND_TEST(TestDagVertex, sub_incomming_edge);
      FRIEND_TEST(TestDagVertex, clear_edges);
      FRIEND_TEST(TestDagVertex, reset_incomming_edge_count);
      FRIEND_TEST(TestDagVertex, get_edge_at);

    public:
      //! A utility struct used to reconnect a \ref dag_vertex
      /*!
          This is a utility class used to reconnect two \ref dag_vertex
          and their associated \ref dag_edge.
      */
      struct dag_vertex_connection
      {
      public:
        //! A constructor for a \ref dag_vertex_connection
        /*!
          A constructor for a \ref dag_vertex_connection
        */
        dag_vertex_connection(); // For std containers.

        //! A constructor for a \ref dag_vertex_connection
        /*!
          A constructor for a \ref dag_vertex_connection

          \param[in] edge The \ref dag_edge that points to \ref vertex
          \param[in] vertex The \ref dag_vertex pointed to by \ref edge.
        */
        dag_vertex_connection(dag_edge &edge, dag_vertex &vertex);

        //! A getter for the \ref dag_edge that points to \ref vertex_
        /*!
          A getter for the \ref dag_edge that points to \ref vertex_

          \return A const reference to the member variable edge_
        */
        const dag_edge &edge() const;

        //! A getter for the \ref dag_vertex that is pointed to by \ref edge_
        /*!
          A getter for the \ref dag_vertex that is pointed to by \ref edge_

          \return A const reference to the member variable vertex_
        */
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
      //! A constructor for a \ref dag_vertex
      /*!
        A constructor for a \ref dag_vertex
      */
      dag_vertex();

      //! A constructor for a \ref dag_vertex
      /*!
        A constructor for a \ref dag_vertex

        \param[in] label A string by which to identify one or more
                         \ref dag_vertex.
      */
      explicit dag_vertex(const std::string &label);

      //! A destructor for a \ref dag_vertex
      /*!
        A destructor for a \ref dag_vertex
      */
      virtual ~dag_vertex();

      //! A move constructor for a \ref dag_vertex
      /*!
        A move constructor for a \ref dag_vertex
      */
      dag_vertex(dag_vertex &&other);

      //! A assignment move operator for a \ref dag_vertex
      /*!
        A assignment move operator for a \ref dag_vertex
      */
      dag_vertex &operator=(dag_vertex &&rhs);

      //! Clones this object into an identical dag_vertex
      /*!
        A member function of \ref dag_vertex that makes an identical copy of
        this and returns it. However, does not preseve \ref dag_edges.

        \return A \ref dag_vertex that is an identical copy of this.
      */
      dag_vertex clone();

      //! A connect a ref counted instance of a \ref dag_vertex to this.
      /*!
      */
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
      const dag_edge &get_edge_at(std::size_t i) const;

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