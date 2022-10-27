#ifndef DAG_H_INCLUDED
#define DAG_H_INCLUDED

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

// NOTE: Pragmas added for YCM.
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wshadow"
#endif
#include <boost/beast.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace com
{
  namespace dag_scheduler
  {
    class DAGEdge;
    class DAGVertex;

    /**
     * @brief A \class DAG that represents a directed acyclic graph.
     *
     * A class that represents a directed acyclic graph (dag) and operations
     * that can be performed on it. You can add, find, check containment a
     * \ref DAGVertex. Additionally you can connect two \ref DAGVertex.
     * Every connection checks to preserve that acyclic portion of the graph.
     * If any connection would make the graph acyclic then an exception is
     * thrown and the connection is not made.
     */
    class DAG : public LoggedClass<DAG>
    {
    public:
      /**
       * @brief An class used to indicate an exception in an operation on a
       *        dag.
       *
       * A class used to indicate an exception in an operation on a dag. One
       * such an example would be if the client tried to connect two
       * \ref DAGVertex s that would make the graph have a cycle.
       */
      class DAGException : public std::runtime_error
      {
      public:
        /**
         * @brief ctor
         *
         * @param[in] message A constructor for a \ref dag::dag_exception.
         */
        explicit DAGException(const char *message);

        /**
         * @brief Override for \ref std::runtime_error 's what.
         *
         * @return The error message specified in \ref DAG::DAGException 's
                   constructor. 
         */
        virtual const char *what() const throw() override;

      private:
        std::string what_;
      };

    public:
      /**
       * @brief A default constructor for a \ref DAG.
       *
       * A default constructor for a \ref DAG which initializes its title to
       * the empty string.
       */
      DAG();

      /**
       * @brief A constructor for a \ref DAG.
       *
       * A constructor for a \ref DAG which initializes its title to the
       * \ref title parameter.
       *
       * @param[in] title The title of the DAG.
       */
      explicit DAG(const std::string &title);

      /**
       * @brief dtor
       */
      ~DAG();

      /**
       * @brief The move copy constructor.
       *
       * The move copy constructor moves ownership of the graph data
       * structure from this to \ref other.
       *
       * @param[out] other 
       */
      DAG(DAG &&other);

      /**
       * @brief Move assignment operator.
       *
       * The move assignment operator moves ownership of the graph data
       * structure from this to \ref other.
       *
       * @param[out] other The \ref DAG to move ownership to.
       *
       * @return A reference to (this).
       */
      DAG &operator=(DAG &&other);

      /**
       * @brief Clones this object into an identical DAG.
       * A member function of \ref DAG that makes an identical copy of this
       * and returns it.
       *
       * @return A \ref DAG that is an identical copy of this. 
       */
      DAG clone();

      /**
       * @brief Adds a disconnected \ref DAGVertex into the graph.
       *
       * A member function of \ref DAG that adds a \ref DAGVertex to the
       * graph data structure. If the insertion fails then this function
       * returns false otherwise it returns ture.
       *
       * @param[in] v A \ref DAGVertex to be added to the \ref DAG.
       *
       * @return true if \ref v is inserted or false other wise. 
       */
      bool add_vertex(DAGVertex &&v);

      /**
       * @brief Finds a \ref DAGVertex in the graph.
       *
       * A member function of \ref DAG that finds a \ref v in the graph
       * data structure and returns a \ref std::weak_ptr<DAGVertex> to
       * the original \ref DAGVertex stored in the graph.
       *
       * @param[in] v A \ref DAGVertex used to find the the \ref dag.
       *
       * @return A \ref std::weak_ptr<DAGVertex> to the original
       *         \ref DAGVertex stored in the graph. If \ref v is not found
       *         then an expired \ref std::weak_ptr<DAGVertex> is returned. 
       */
      std::weak_ptr<DAGVertex> find_vertex(const DAGVertex &v);

      /**
       * @brief Finds a \ref DAGVertex in the graph
       *
       * A member function of \ref DAG that finds a \ref v in the graph
       * data structure and returns a \ref std::weak_ptr<DAGVertex> to
       * the original \ref DAGVertex stored in the graph based on its
       * \ref UUID..
       *
       * @param[in] u A \ref UUID used to find the the \ref DAG.
       *
       * @return A \ref std::weak_ptr<DAGVertex> to the original
       *         \ref DAGVertex stored in the graph. If a \ref DAGVertex
       *         \ref v is not found then an expired
       *         \ref std::weak_ptr<DAGVertex> is returned. 
       */
      std::weak_ptr<DAGVertex> find_vertex_by_uuid(const UUID &u);

      /**
       * @brief Finds all \ref DAGVertex (s) in the graph from a \ref label.
       *
       * A member function of \ref DAG that finds all \ref DAGVertex (s) in
       * the graph and returns a \ref std::vector<std::weak_ptr<DAGVertex>>
       * of those \ref DAGVertex (s).
       *
       * @param[in] l A \ref std::string that contains the label of the
       *              \ref DAGVertex (s) that you are interested in.
       *
       * @return A \ref std::vector<std::weak_ptr<DAGVertex>> that contains
       *         a \ref std::weak_ptr to a collection of \ref DAGVertex (s)
       *         contained within the graph. 
       */
      std::vector<std::weak_ptr<DAGVertex>> find_all_verticies_with_label(
        const std::string &l);

      /**
       * @brief Returns true if the \ref DAGVertex \ref v is in the graph.
       *
       * A member function of \ref DAG that check if a \ref DAGVertex
       * is contained in the graph.
       *
       * @param[in] v A \ref DAGVertex to check for in an instance of
       *              \ref this.
       *
       * @return true if \ref v is in an instance of \ref this, false if
       *         \ref v is not in an instance of \ref this. 
       */
      bool contains_vertex(const DAGVertex &v);

      /**
       * @brief Returns true if the \ref DAGVertex \ref v is in the graph.
       *
       * A member function of \ref DAG that check if a \ref DAGVertex
       * is contained in the graph base on its \ref UUID.
       *
       * @param[in] u A \ref DAGVertex to check for in an instance of
       *              \ref this based on its \ref UUID.
       *
       * @return true if \ref v is in an instance of \ref this, false if
       *         \ref v is not in an instance of \ref this based on its
       *         \ref UUID.
       */
      bool contains_vertex_by_uuid(const UUID &u);

      /**
       * @brief Returns true if the \ref DAGVertex \ref v is in the graph.
       *
       * A member function of \ref DAG that check if a \ref DAGVertex
       * is contained in the graph base on its label
       *
       * @param[in] l The label owned by one or more \ref DAGVertex.
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
       * A member function of \ref DAG that checks if adding a \ref DAGEdge
       * between \ref DAGVertex \ref v1 and \ref DAGVertex \ref v2 would
       * make \ref this a cylical grph.
       *
       * @param[in] v1 A \ref DAGVertex where the edge begins.
       * @param[in] v2 A \ref DAGVertex where the edge ends.
       *
       * @return true if connecting \ref v2 to \ref v1 would make \ref this
                 a cyclic graph. 
       */
      bool connection_would_make_cyclic(const DAGVertex &v1,
        const DAGVertex &v2);

      /**
       * @brief Returns true if connecting v1 to v2 makes graph
       *        cyclic based on the UUID owned by the vertex.
       *
       * A member function of \ref DAG that checks if adding a \ref DAGEdge
       * between \ref UUID \p u1 and \ref UUID \p u2 would make \ref this a
       * cylical graph based on \ref UUID of v1 and v2.
       *
       * @param[in] u1 A \ref UUID of a \ref DAGVertex where the edge begins.
       * @param[in] u2 A \ref UUID of a \ref DAGVertex where the edge begins.
       *
       * @return true if connecting \ref v2 to \ref v1 would make \ref this
       *         a cyclic graph based on \ref UUIDs of \ref v1 and \ref v2. 
       */
      bool connection_would_make_cyclic_by_uuid(const UUID &u1,
        const UUID &u2);

      /**
       * @brief Returns true if connecting any v1 to any v2 with a label \p l1
       *        to \p l2 makes graph cyclic.
       *
       * A member function of \ref DAG that checks if adding a \ref DAGEdge
       * between \ref DAGVertex v1 and \ref DAGVertex v2 would
       * make \ref this a cylical grph based on the labels \p l1 and \p l2.
       *
       * @param[in] l1 A string held by a \ref DAGVertex where the edge
       *               begins.
       * @param[in] l2 A string held by a \ref DAGVertex where the edge ends.
       *
       * @return true if connecting v2 to v1 based on \p l2 and \pl1 would
       *         make \ref this a cyclic graph. 
       */
      bool connection_would_make_cyclic_by_label(const std::string &l1,
        const std::string &l2);

      /**
       * @brief A function to make a directed connection between to \ref
       *        DAGVertex
       *
       * A member function of \ref DAG that connects two \ref DAGVertex
       * with a directed \ref DAGEdge from \ref v1 to \ref v2.
       *
       * @param[in] v1 The start \ref DAGVertex from where the \ref DAGEdge
                       begins.
       * @param[in] v2 The end \ref DAGVertex from where the \ref DAGEdge
                       ends.
       *
       * @return true if the \ref DAGVertex were found and connected. 
       */
      bool connect(const DAGVertex &v1, const DAGVertex &v2);
 
      /**
       * @brief A function to make a directed connection between to \ref
       *        DAGVertex 
       *
       * A member function of \ref DAG that connects two \ref DAGVertex
       * with a directed \ref DAGEdge from v1 to v2 based on their \ref
       * UUID.
       *
       * @param[in] u1 The start \ref UUID of a \ref DAGVertex from where the
       *               \ref DAGEdge begins.
       * @param[in] u2 The end \ref UUID of a \ref DAGVertex from where the
       *               \ref DAGEdge ends.
       *
       * @return true if the \ref DAGVertex were found and connected.
       */
      bool connect_by_uuid(const UUID &u1, const UUID &u2);

      /**
       * @brief A function to make mutliple connections between
       *        \ref DAGVertex.
       *
       * A member function of \ref DAG that connects multiple \ref DAGVertex
       * based on labels. It will create a \ref DAGEdge from any
       * \ref DAGVertex with label \ref l1 to any \ref DAGVertex with label
       * \ref l2. It will not make a connection if any \ref DAGEdge would
       * cause \ref this to become cyclic.
       *
       * @param[in] l1 The label to be used to find all \ref DAGVertex from
       *               which the directed \ref DAGEdge leaves.
       * @param[in] l2 The label to be used to find all \ref DAGVertex to
       *               which the directed \ref DAGEdge connects.
       *
       * @return true if all connections are made false if just one failed. 
       */
      bool connect_all_by_label(const std::string l1, const std::string l2);

      /**
       * @brief A function that adds upto two \ref DAGVertex and then
       *        connects.
       *
       * A member function of \ref DAG that connects two \ref DAGVertex
       * \ref v1 to \ref v2. Meaning a directed \ref DAGEdge is drawn from
       * \ref v1 to \ref v2. Note that if \ref v1 or \ref v2 exist in the code
       * then they will not be added to \ref this. Also if they are already
       * connected a connection will not be made.
       *
       * @param[in] v1 The start \ref DAGVertex from where the \ref DAGEdge
       *               begins.
       * @param[in] v2 The end \ref DAGVertex to where the \ref DAGEdge is
       *               drawn to.
       *
       * @return true if both \ref DAGVertex are added and a \ref DAGEdge
       *         is created between \ref v1 and \ref v2. 
       */
      bool add_and_connect(DAGVertex &&v1, DAGVertex &&v2);

      /**
       * @brief A function that checks if \ref DAGVertex share a \ref
       *        DAGEdge.
       *
       * A member function of \ref DAG that checks if there exists
       * a \ref DAGEdge from \ref v1 to \ref v2.
       *
       * @param[in] v1 The start \ref DAGVertex from where the \ref DAGEdge
       *               begins.
       * @param[in] v2 The end \ref DAGVertex to where the \ref DAGEdge is
       *               drawn to.
       *
       * @return true if both \ref v1 had a \ref DAGEdge that points to
       *         \ref v2. 
       */
      bool are_connected(const DAGVertex &v1, const DAGVertex &v2);

      /**
       * @brief A function that checks if \ref DAGVertex share a
       * \ref DAGEdge.
       *
       * A member function of \ref DAG that checks if there exists
       * a \ref DAGEdge from \ref v1 to \ref v2 based on \ref UUID (s).
       *
       * @param[in] u1 The start \ref DAGVertex from where the \ref DAGEdge
       *               begins. \p u1 is owned by 1st the \ref DAGVertex.
       * @param[in] u2 The end \ref DAGVertex to where the \ref DAGEdge
       *               ends. \p u1 is owned by 2nd the \ref DAGVertex.
       *
       * @return true if both \ref v1 had a \ref DAGEdge that points to
       *         \ref v2 based on \ref UUID (s). 
       */
      bool are_connected_by_uuid(const UUID &u1, const UUID &u2);

      /**
       * @brief A function that checks for a single connection between two
       *        labels
       *
       * A member function of \ref DAG that checks if there exists a
       * single connection between \ref DAGVertex (s) that have \ref l1 to
       * \ref l2.
       *
       * @param[in] l1 The start label for a \ref DAGVertex from which the
       *               \ref DAGEdge leaves.
       * @param[in] l2 The end label for a \ref DAGVertex from which the
       *               \ref DAGEdge goes into.
       *
       * @return true if there exists a \ref DAGEdge from a \ref DAGVertex
       *         with a label of \ref l1 to a \ref DAGVertex with label
       *         \ref l2. If there exists no \ref DAGEdge then false. 
       */
      bool all_are_connected_by_label(const std::string l1,
        const std::string l2);

      /**
       * @brief A function that calls user function with each \ref DAGVertex
       *
       * A member function of \ref DAG that visits each \ref DAGVertex
       * in \ref this in the order they were added. For each \ref DAGVertex
       * a user defined function \ref cb is called with a ref counted
       * instance of the \ref DAGVertex.
       *
       * @param[in] cb A \ref std::function which takes a instance of a
       *               \ref std::shared_ptr<\ref DAGVertex> contained in
       *               \ref this.
       */
      void linear_traversal(
        std::function<void (std::shared_ptr<DAGVertex>)> cb);

      /**
       * @brief A function to get the number of \ref DAGVertex in \ref this.
       *
       * A member function of \ref DAG that returns the number of
       * \ref DAGVertex in \ref this.
       *
       * @return A positive integer count of the number of \ref DAGVertex
       *         store in \ref this. 
       */
      std::size_t vertex_count() const;

      /**
       * @brief A function to get the number of \ref DAGEdge in \ref this.
       *
       * A member function of \ref DAG that returns the number of
       * \ref DAGEdge in \ref this.
       *
       * @return A positive integer count of the number of \ref DAGVertex
                 store in \ref this. 
       */
      std::size_t edge_count() const;

      /**
       * @brief A function that returns an optional title of \ref this.
       *
       * A member function of \ref DAG that returns the optional title of
       * \ref this.
       *
       * @return A \ref std::string which contains a optional title. 
       */
      const std::string &title() const;

      /**
       * @brief A function that removes a \ref DAGVertex and its
       *        \ref DAGEdges.
       *
       * A member function of \ref DAG that removes a \ref DAGVertex \ref v
       * and all its \ref DAGEdge from an instance of \ref dag.
       *
       * @param[in] v The \ref DAGVertex to find and remove.
       *
       * @return true if \ref v was found and removed. False otherwise. 
       */
      bool remove_vertex(const DAGVertex &v);

      /**
       * @brief A function that removes a \ref DAGVertex and its
       *        \ref DAGEdges.
       *
       * A member function of \ref DAG that removes a \ref DAGVertex \ref v
       * based on its \ref UUID and all its \ref DAGEdge from an instance of
       * \ref dag.
       *
       * @param[in] id The \ref UUID of a \ref DAGVertex to find and remove.
       *
       * @return true if \ref v was found and removed. False otherwise. 
       */
      bool remove_vertex_by_uuid(const UUID &id);

      /**
       * @brief  A function that removes a \ref DAGVertex (s) and their
       *         \ref DAGEdges.
       *
       * A member function of \ref DAG that remove a set of \ref DAGVertex
       * based on a label. All \ref DAGEdge (s) that extend from that set
       * are also removed from the \ref dag.
       *
       * @param[in] label A string used to find any \ref DAGVertex requested
       *                  to be removed from an instance of a \ref dag.
       *
       * @return true if any \ref DAGVertex was found with \ref label and
       *         removed. False otherwise. 
       */
      bool remove_all_vertex_with_label(const std::string &label);

      /**
       * @brief A function that clears a instance of \ref dag.
       *
       * A member function of \ref DAG that removes all \ref DAGEdge
       * between all \ref DAGVertex in \ref this.
       */
      void reset();

    public:
      friend std::ostream &operator<<(std::ostream &out, const DAG &g);
      friend bool operator==(const DAG &lhs, const DAG &rhs);
      friend bool operator!=(const DAG &lhs, const DAG &rhs);

    protected:
      std::shared_ptr<DAGVertex> get_vertex_at(std::size_t i);
      void clone_connections(DAGVertex &from, DAGVertex &to);

      DAG(const DAG &other);
      DAG &operator=(const DAG &rhs);

    private:
      typedef std::vector<std::shared_ptr<DAGVertex>> dag_graph;
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
