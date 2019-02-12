#ifndef DAG_H_INCLUDED
#define DAG_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/dag_vertex.h"
#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/uuid.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <unordered_map>
#include <vector>

#include <gtest/gtest_prod.h>

namespace com
{
  namespace dag_scheduler
  {
    class dag_edge;
    class dag_vertex;

    /**
     * @brief A \class dag that represents a directed acyclic graph.
     *
     * A class that represents a directed acyclic graph (dag) and operations
     * that can be performed on it. You can add, find, check containment a
     * \ref dag_vertex. Additionally you can connect two \ref dag_vertex.
     * Every connection checks to preserve that acyclic portion of the graph.
     * If any connection would make the graph acyclic then an exception is
     * thrown and the connection is not made.
     */
    class DLLSPEC_DAGTASKS dag : public logged_class<dag>
    {
    public:
      /**
       * @brief An class used to indicate an exception in an operation on a
       *        dag.
       *
       * A class used to indicate an exception in an operation on a dag. One
       * such an example would be if the client tried to connect two
       * \ref dag_vertex s that would make the graph have a cycle.
       */
      class dag_exception : public std::runtime_error
      {
      public:
        /**
         * @brief ctor
         *
         * @param[in] message A constructor for a \ref dag::dag_exception.
         */
        explicit dag_exception(const char *message);

        /**
         * @brief Override for \ref std::runtime_error 's what.
         *
         * @return The error message specified in \ref dag::dag_exception 's
                   constructor. 
         */
        virtual const char *what() const throw() override;

      private:
        std::string what_;
      };

    public:
      /**
       * @brief A default constructor for a \ref dag.
       *
       * A default constructor for a \ref dag which initializes its title to
       * the empty string.
       */
      dag();

      /**
       * @brief A constructor for a \ref dag.
       *
       * A constructor for a \ref dag which initializes its title to the
       * \ref title parameter.
       *
       * @param[in] title The title of the dag.
       */
      explicit dag(const std::string &title);

      /**
       * @brief dtor
       */
      ~dag();

      /**
       * @brief The move copy constructor.
       *
       * The move copy constructor moves ownership of the graph data
       * structure from this to \ref other.
       *
       * @param[out] other 
       */
      dag(dag &&other);

      /**
       * @brief Move assignment operator.
       *
       * The move assignment operator moves ownership of the graph data
       * structure from this to \ref other.
       *
       * @param[out] other The \ref dag_graph to move ownership to.
       *
       * @return A reference to (this).
       */
      dag &operator=(dag &&other);

      /**
       * @brief Clones this object into an identical dag.
       * A member function of \ref dag that makes an identical copy of this
       * and returns it.
       *
       * @return A \ref dag that is an identical copy of this. 
       */
      dag clone();

      /**
       * @brief Adds a disconnected \ref dag_vertex into the graph.
       *
       * A member function of \ref dag that adds a \ref dag_vertex to the
       * graph data structure. If the insertion fails then this function
       * returns false otherwise it returns ture.
       *
       * @param[in] v A \ref dag_vertex to be added to the \ref dag.
       *
       * @return true if \ref v is inserted or false other wise. 
       */
      bool add_vertex(dag_vertex &&v);

      /**
       * @brief Finds a \ref dag_vertex in the graph.
       *
       * A member function of \ref dag that finds a \ref v in the graph
       * data structure and returns a \ref std::weak_ptr<dag_vertex> to
       * the original \ref dag_vertex stored in the graph.
       *
       * @param[in] v A \ref dag_vertex used to find the the \ref dag.
       *
       * @return A \ref std::weak_ptr<dag_vertex> to the original
       *         \ref dag_vertex stored in the graph. If \ref v is not found
       *         then an expired \ref std::weak_ptr<dag_vertex> is returned. 
       */
      std::weak_ptr<dag_vertex> find_vertex(const dag_vertex &v);

      /**
       * @brief Finds a \ref dag_vertex in the graph
       *
       * A member function of \ref dag that finds a \ref v in the graph
       * data structure and returns a \ref std::weak_ptr<dag_vertex> to
       * the original \ref dag_vertex stored in the graph based on its
       * \ref uuid..
       *
       * @param[in] u A \ref uuid used to find the the \ref dag.
       *
       * @return A \ref std::weak_ptr<dag_vertex> to the original
       *         \ref dag_vertex stored in the graph. If a \ref dag_vertex
       *         \ref v is not found then an expired
       *         \ref std::weak_ptr<dag_vertex> is returned. 
       */
      std::weak_ptr<dag_vertex> find_vertex_by_uuid(const uuid &u);

      /**
       * @brief Finds all \ref dag_vertex (s) in the graph from a \ref label.
       *
       * A member function of \ref dag that finds all \ref dag_vertex (s) in
       * the graph and returns a \ref std::vector<std::weak_ptr<dag_vertex>>
       * of those \ref dag_vertex (s).
       *
       * @param[in] l A \ref std::string that contains the label of the
       *              \ref dag_vertex (s) that you are interested in.
       *
       * @return A \ref std::vector<std::weak_ptr<dag_vertex>> that contains
       *         a \ref std::weak_ptr to a collection of \ref dag_vertex (s)
       *         contained within the graph. 
       */
      std::vector<std::weak_ptr<dag_vertex>> find_all_verticies_with_label(
        const std::string &l);

      /**
       * @brief Returns true if the \ref dag_vertex \ref v is in the graph.
       *
       * A member function of \ref dag that check if a \ref dag_vertex
       * is contained in the graph.
       *
       * @param[in] v A \ref dag_vertex to check for in an instance of
       *              \ref this.
       *
       * @return true if \ref v is in an instance of \ref this, false if
       *         \ref v is not in an instance of \ref this. 
       */
      bool contains_vertex(const dag_vertex &v);

      /**
       * @brief Returns true if the \ref dag_vertex \ref v is in the graph.
       *
       * A member function of \ref dag that check if a \ref dag_vertex
       * is contained in the graph base on its \ref uuid.
       *
       * @param[in] u A \ref dag_vertex to check for in an instance of
       *              \ref this based on its \ref uuid.
       *
       * @return true if \ref v is in an instance of \ref this, false if
       *         \ref v is not in an instance of \ref this based on its
       *         \ref uuid.
       */
      bool contains_vertex_by_uuid(const uuid &u);

      /**
       * @brief Returns true if the \ref dag_vertex \ref v is in the graph.
       *
       * A member function of \ref dag that check if a \ref dag_vertex
       * is contained in the graph base on its label
       *
       * @param[in] l The label owned by one or more \ref dag_vertex.
       *
       * @return true if \ref v is in an instance of \ref this, false if
       *         \ref v is not in an instance of \ref this based on its
       *         label. 
       */
      bool contains_vertex_by_label(const std::string &l);

      /**
       * @brief Returns true if connecting \ref v1 to \ref v2 makes graph
       *        cyclic.
       *
       * A member function of \ref dag that checks if adding a \ref dag_edge
       * between \ref dag_vertex \ref v1 and \ref dag_vertex \ref v2 would
       * make \ref this a cylical grph.
       *
       * @param[in] v1 A \ref dag_vertex where the edge begins.
       * @param[in] v2 A \ref dag_vertex where the edge ends.
       *
       * @return true if connecting \ref v2 to \ref v1 would make \ref this
                 a cyclic graph. 
       */
      bool connection_would_make_cyclic(const dag_vertex &v1,
        const dag_vertex &v2);

      /**
       * @brief Returns true if connecting v1 to v2 makes graph
       *        cyclic based on the uuid owned by the vertex.
       *
       * A member function of \ref dag that checks if adding a \ref dag_edge
       * between \ref uuid \p u1 and \ref uuid \p u2 would make \ref this a
       * cylical graph based on \ref uuid of v1 and v2.
       *
       * @param[in] u1 A \ref uuid of a \ref dag_vertex where the edge begins.
       * @param[in] u2 A \ref uuid of a \ref dag_vertex where the edge begins.
       *
       * @return true if connecting \ref v2 to \ref v1 would make \ref this
       *         a cyclic graph based on \ref uuids of \ref v1 and \ref v2. 
       */
      bool connection_would_make_cyclic_by_uuid(const uuid &u1,
        const uuid &u2);

      /**
       * @brief Returns true if connecting any v1 to any v2 with a label \p l1
       *        to \p l2 makes graph cyclic.
       *
       * A member function of \ref dag that checks if adding a \ref dag_edge
       * between \ref dag_vertex v1 and \ref dag_vertex v2 would
       * make \ref this a cylical grph based on the labels \p l1 and \p l2.
       *
       * @param[in] l1 A string held by a \ref dag_vertex where the edge
       *               begins.
       * @param[in] l2 A string held by a \ref dag_vertex where the edge ends.
       *
       * @return true if connecting v2 to v1 based on \p l2 and \pl1 would
       *         make \ref this a cyclic graph. 
       */
      bool connection_would_make_cyclic_by_label(const std::string &l1,
        const std::string &l2);

      /**
       * @brief A function to make a directed connection between to \ref
       *        dag_vertex
       *
       * A member function of \ref dag that connects two \ref dag_vertex
       * with a directed \ref dag_edge from \ref v1 to \ref v2.
       *
       * @param[in] v1 The start \ref dag_vertex from where the \ref dag_edge
                       begins.
       * @param[in] v2 The end \ref dag_vertex from where the \ref dag_edge
                       ends.
       *
       * @return true if the \ref dag_vertex were found and connected. 
       */
      bool connect(const dag_vertex &v1, const dag_vertex &v2);
 
      /**
       * @brief A function to make a directed connection between to \ref
       *        dag_vertex 
       *
       * A member function of \ref dag that connects two \ref dag_vertex
       * with a directed \ref dag_edge from v1 to v2 based on their \ref
       * uuid.
       *
       * @param[in] u1 The start \ref uuid of a \ref dag_vertex from where the
       *               \ref dag_edge begins.
       * @param[in] u2 The end \ref uuid of a \ref dag_vertex from where the
       *               \ref dag_edge ends.
       *
       * @return true if the \ref dag_vertex were found and connected.
       */
      bool connect_by_uuid(const uuid &u1, const uuid &u2);

      /**
       * @brief A function to make mutliple connections between
       *        \ref dag_vertex.
       *
       * A member function of \ref dag that connects multiple \ref dag_vertex
       * based on labels. It will create a \ref dag_edge from any
       * \ref dag_vertex with label \ref l1 to any \ref dag_vertex with label
       * \ref l2. It will not make a connection if any \ref dag_edge would
       * cause \ref this to become cyclic.
       *
       * @param[in] l1 The label to be used to find all \ref dag_vertex from
       *               which the directed \ref dag_edge leaves.
       * @param[in] l2 The label to be used to find all \ref dag_vertex to
       *               which the directed \ref dag_edge connects.
       *
       * @return true if all connections are made false if just one failed. 
       */
      bool connect_all_by_label(const std::string l1, const std::string l2);

      /**
       * @brief A function that adds upto two \ref dag_vertex and then
       *        connects.
       *
       * A member function of \ref dag that connects two \ref dag_vertex
       * \ref v1 to \ref v2. Meaning a directed \ref dag_edge is drawn from
       * \ref v1 to \ref v2. Note that if \ref v1 or \ref v2 exist in the code
       * then they will not be added to \ref this. Also if they are already
       * connected a connection will not be made.
       *
       * @param[in] v1 The start \ref dag_vertex from where the \ref dag_edge
       *               begins.
       * @param[in] v2 The end \ref dag_vertex to where the \ref dag_edge is
       *               drawn to.
       *
       * @return true if both \ref dag_vertex are added and a \ref dag_edge
       *         is created between \ref v1 and \ref v2. 
       */
      bool add_and_connect(dag_vertex &&v1, dag_vertex &&v2);

      /**
       * @brief A function that checks if \ref dag_vertex share a \ref
       *        dag_edge.
       *
       * A member function of \ref dag that checks if there exists
       * a \ref dag_edge from \ref v1 to \ref v2.
       *
       * @param[in] v1 The start \ref dag_vertex from where the \ref dag_edge
       *               begins.
       * @param[in] v2 The end \ref dag_vertex to where the \ref dag_edge is
       *               drawn to.
       *
       * @return true if both \ref v1 had a \ref dag_edge that points to
       *         \ref v2. 
       */
      bool are_connected(const dag_vertex &v1, const dag_vertex &v2);

      /**
       * @brief A function that checks if \ref dag_vertex share a
       * \ref dag_edge.
       *
       * A member function of \ref dag that checks if there exists
       * a \ref dag_edge from \ref v1 to \ref v2 based on \ref uuid (s).
       *
       * @param[in] u1 The start \ref dag_vertex from where the \ref dag_edge
       *               begins. \p u1 is owned by 1st the \ref dag_vertex.
       * @param[in] u2 The end \ref dag_vertex to where the \ref dag_edge
       *               ends. \p u1 is owned by 2nd the \ref dag_vertex.
       *
       * @return true if both \ref v1 had a \ref dag_edge that points to
       *         \ref v2 based on \ref uuid (s). 
       */
      bool are_connected_by_uuid(const uuid &u1, const uuid &u2);

      /**
       * @brief A function that checks for a single connection between two
       *        labels
       *
       * A member function of \ref dag that checks if there exists a
       * single connection between \ref dag_vertex (s) that have \ref l1 to
       * \ref l2.
       *
       * @param[in] l1 The start label for a \ref dag_vertex from which the
       *               \ref dag_edge leaves.
       * @param[in] l2 The end label for a \ref dag_vertex from which the
       *               \ref dag_edge goes into.
       *
       * @return true if there exists a \ref dag_edge from a \ref dag_vertex
       *         with a label of \ref l1 to a \ref dag_vertex with label
       *         \ref l2. If there exists no \ref dag_edge then false. 
       */
      bool all_are_connected_by_label(const std::string l1,
        const std::string l2);

      /**
       * @brief A function that calls user function with each \ref dag_vertex
       *
       * A member function of \ref dag that visits each \ref dag_vertex
       * in \ref this in the order they were added. For each \ref dag_vertex
       * a user defined function \ref cb is called with a ref counted
       * instance of the \ref dag_vertex.
       *
       * @param[in] cb A \ref std::function which takes a instance of a
       *               \ref std::shared_ptr<\ref dag_vertex> contained in
       *               \ref this.
       */
      void linear_traversal(
        std::function<void (std::shared_ptr<dag_vertex>)> cb);

      /**
       * @brief A function to get the number of \ref dag_vertex in \ref this.
       *
       * A member function of \ref dag that returns the number of
       * \ref dag_vertex in \ref this.
       *
       * @return A positive integer count of the number of \ref dag_vertex
       *         store in \ref this. 
       */
      std::size_t vertex_count() const;

      /**
       * @brief A function to get the number of \ref dag_edge in \ref this.
       *
       * A member function of \ref dag that returns the number of
       * \ref dag_edge in \ref this.
       *
       * @return A positive integer count of the number of \ref dag_vertex
                 store in \ref this. 
       */
      std::size_t edge_count() const;

      /**
       * @brief A function that returns an optional title of \ref this.
       *
       * A member function of \ref dag that returns the optional title of
       * \ref this.
       *
       * @return A \ref std::string which contains a optional title. 
       */
      const std::string &title() const;

      /**
       * @brief A function that removes a \ref dag_vertex and its
       *        \ref dag_edges.
       *
       * A member function of \ref dag that removes a \ref dag_vertex \ref v
       * and all its \ref dag_edge from an instance of \ref dag.
       *
       * @param[in] v The \ref dag_vertex to find and remove.
       *
       * @return true if \ref v was found and removed. False otherwise. 
       */
      bool remove_vertex(const dag_vertex &v);

      /**
       * @brief A function that removes a \ref dag_vertex and its
       *        \ref dag_edges.
       *
       * A member function of \ref dag that removes a \ref dag_vertex \ref v
       * based on its \ref uuid and all its \ref dag_edge from an instance of
       * \ref dag.
       *
       * @param[in] id The \ref uuid of a \ref dag_vertex to find and remove.
       *
       * @return true if \ref v was found and removed. False otherwise. 
       */
      bool remove_vertex_by_uuid(const uuid &id);

      /**
       * @brief  A function that removes a \ref dag_vertex (s) and their
       *         \ref dag_edges.
       *
       * A member function of \ref dag that remove a set of \ref dag_vertex
       * based on a label. All \ref dag_edge (s) that extend from that set
       * are also removed from the \ref dag.
       *
       * @param[in] label A string used to find any \ref dag_vertex requested
       *                  to be removed from an instance of a \ref dag.
       *
       * @return true if any \ref dag_vertex was found with \ref label and
       *         removed. False otherwise. 
       */
      bool remove_all_vertex_with_label(const std::string &label);

      /**
       * @brief A function that clears a instance of \ref dag.
       *
       * A member function of \ref dag that removes all \ref dag_edge
       * between all \ref dag_vertex in \ref this.
       */
      void reset();

    public:
      friend std::ostream &operator<<(std::ostream &out, const dag &g);
      friend bool operator==(const dag &lhs, const dag &rhs);
      friend bool operator!=(const dag &lhs, const dag &rhs);

    protected:
      std::shared_ptr<dag_vertex> get_vertex_at(std::size_t i);
      void clone_connections(dag_vertex &from, dag_vertex &to);

      dag(const dag &other);
      dag &operator=(const dag &rhs);

    private:
      typedef std::vector<std::shared_ptr<dag_vertex>> dag_graph;
      dag_graph graph_;
      std::string title_;

    private:
      FRIEND_TEST(TestDag, get_vertex_at);
      FRIEND_TEST(TestDag, clone_connections);
      FRIEND_TEST(TestDag, copy_ctor);
      FRIEND_TEST(TestDag, assignment_operator);
    };
  }
}
#endif
