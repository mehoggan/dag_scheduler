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
    /**
     * @brief A class that represents a directed edge in a acyclic graph
     *        (dag).
     *
     * A class that represents a directed edge in a acyclic graph (dag) and
     * operations that can be performed on it. Users can check for connected
     * \ref dag_vertex and get access to the connected \ref dag_vertex.
     */
    class DLLSPEC_DAGTASKS dag_edge
    {
    private:
      friend class dag;
      friend class dag_vertex;
      friend struct dag_vertex::dag_vertex_connection;
      friend bool dag_topological_sort(dag &g,
        std::list<dag_vertex> &sorted_vertices); 

    public:
      // TODO: Use dag class to manage status.
      enum class status
      {
        initialized,
        traversed,
        non_traverable
      };

    public:
      /**
       * @brief A constructor for a \ref dag_edge
       *
       * A constructor for a \ref dag_edge.
       */
      dag_edge();

      /**
       * @brief A destructor for a \ref dag_edge
       *
       * A destructor for a \ref dag_edge
       */
      virtual ~dag_edge();

      /**
       * @brief A move constructor for a \ref dag_edge
       *
       * A move constructor for a \ref dag_edge
       *
       * @param[out] other The \ref dag_edge to move into (*this).
       */
      dag_edge(dag_edge &&other);

      //! 
      /*!
      */
      /**
       * @brief A assignment move operator for a \ref dag_edge
       *
       * A assignment move operator for a \ref dag_edge
       *
       * @param[out] rhs The \ref dag_edge to move into (*this).
       *
       * @return A reference to (*this).
       */
      dag_edge &operator=(dag_edge &&rhs);

      /**
       * @brief Clones this object into an identical dag_edge.
       *
       * A member function of \ref dag_edge that makes an identical copy of
       * this and returns it. However, it resets the connection which can only
       * be made from an object that knows about the connection.
       *
       * @return A \ref dag_edge that is an identical copy of this. 
       */
      dag_edge clone();

      /**
       * @brief Get a weak reference to the \ref dag_vertex pointed to by
       *        this.
       *
       * A member function of \ref dag_edge that returns a weak reference to
       * the \ref dag_vertex that this edge points to.
       *
       * @return A \ref std::weak_ptr<\ref dag_vertex> that points to the
       *         \ref dag_vertex that this points to. 
       */
      std::weak_ptr<dag_vertex> get_connection();

      /**
       * @brief Checks to see if this \ref dag_edge points to \ref v
       *
       * @param[in] v A \ref dag_vertex for which we are checking for a
       *              connection to.
       *
       * @return true if \ref this points to \ref v. False other wise. 
       */
      bool is_a_connection_to(const dag_vertex &v) const;

      /**
       * @brief A getter for the \ref uuid of \ref this.
       *
       * A member function of \ref dag_edge that return the \ref uuid that
       * uniqly identifies any clone of \ref this.
       *
       * @return A \ref uuid that uniqly identifies any clone of \ref this. 
       */
      const uuid &get_uuid() const;

      //! TODO (mhoggan): Add doc string once implemented.
      const status &current_status() const;

      //! TODO (mhoggan): Add doc string once implemented.
      std::string current_status_as_string() const;

    public:
      /**
       * @brief A stream operator for writting a \ref dag_egde to a stream.
       *
       * @param[out] out The stream to write \p e to.
       * @param[in] e The \ref dag_edge to write to \p out.
       *
       * @return A reference to \p out after it was streamed to.
       */
      friend std::ostream &operator<<(std::ostream &out, const dag_edge &e);

      /**
       * @brief Comparison operator to compare equivalnce of two
       *        \ref dag_edge (s).
       *
       * Two \ref dag_edge (s) are considered equivalent if:
       *   * First they have the same uuid.
       *   * Second if they have the same status.
       *   * Third if they have the same connection (TODO not well defined.)
       *
       * @param[in] lhs The \ref dag_edge on the left hand side of the '=='.
       * @param[in] rhs The \ref dag_edge on the right hand side of the '=='.
       *
       * @return true if \p lhs == \p rhs.
       */
      friend bool operator==(const dag_edge &lhs, const dag_edge &rhs);

      /**
       * @brief Comparison operator to compare non equivalnce of two
       *        \ref dag_edge (s).
       *
       * Two \ref dag_edge (s) are considered equivalent if:
       *     * First they have the same uuid.
       *     * Second if they have the same status.
       *     * Third if they have the same connection (TODO not well defined.)
       *
       * @param[in] lhs The \ref dag_edge on the left hand side of the '!='.
       * @param[in] rhs The \ref dag_edge on the right hand side of the '!='.
       *
       * @return true if \p lhs != \p rhs.
       */
      friend bool operator!=(const dag_edge &lhs, const dag_edge &rhs);

    protected:
      bool connect_to(std::shared_ptr<dag_vertex> v);
      dag_edge(const dag_edge &other);
      dag_edge &operator=(const dag_edge &rhs);

    private:
      class uuid uuid_;
      status current_status_;
      std::weak_ptr<dag_vertex> connection_;

    private:
      FRIEND_TEST(TestDagEdge, copy_ctor);
      FRIEND_TEST(TestDagEdge, assignment_operator);
      FRIEND_TEST(TestDagEdge, connect_and_clone);
      FRIEND_TEST(TestDagEdge, connect_to_null);
      FRIEND_TEST(TestDagEdge, connections);
      FRIEND_TEST(TestDagEdge, equality_operators_with_connection);
    };
  }
}
#endif
