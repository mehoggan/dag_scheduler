#ifndef DAG_H_INCLUDED
#define DAG_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/dag_vertex.h"
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

    //! A class that represents a directed acyclic graph (dag).
    /*!
      A class that represents a directed acyclic graph (dag) and operations
      that can be performed on it. You can add, find, check containment a
      \ref dag_vertex. Additionally you can connect two \ref dag_vertex.
      Every connection checks to preserve that acyclic portion of the graph.
      If any connection would make the graph acyclic then an exception is
      thrown and the connection is not made.
    */
    class DLLSPEC_DAGTASKS dag
    {
    private:
      FRIEND_TEST(TestUDag, get_vertex_at);
      FRIEND_TEST(TestUDag, clone_connections);
      FRIEND_TEST(TestUDag, copy_ctor);
      FRIEND_TEST(TestUDag, assignment_operator);

    public:
      //! An class used to indicate an exception in an operation on a dag.
      /*!
        A class used to indicate an exception in an operation on a dag. One
        such an example would be if the client tried to connect two
        \ref dag_vertex s that would make the graph have a cycle.
      */
      class dag_exception : public std::runtime_error
      {
      public:
        //! A constructor for a \ref dag::dag_exception.
        /*!
          A constructor for a \ref dag::dag_exception.

          \param[in] message The message used to indicate the error.
        */
        explicit dag_exception(const char *message);

        //! Override for \ref std::runtime_error 's what.
        /*!
          A member function that returns the error message to client code.

          \return The error message specified in \ref dag::dag_exception 's
                  constructor.
        */
        virtual const char *what() const throw();

      private:
        std::string what_;
      };

    public:
      //! A default constructor for a \ref dag.
      /*!
        A default constructor for a \ref dag which initializes its title to
        the empty string.
      */
      dag();

      //! A constructor for a \ref dag.
      /*!
        A constructor for a \ref dag which initializes its title to the
        \ref title parameter.

        \param[in] title The title of the dag.
      */
      explicit dag(const std::string &title);

      //! A destructor for a \ref dag.
      /*!
        A destructor for a \ref dag which empties the contents of it and
        free's any allocated memory.
      */
      ~dag();

      //! The move copy constructor.
      /*!
        The move copy constructor moves ownership of the graph data
        structure from this to \ref other.

        \param[out] other The \ref dag_graph to move ownership to.
      */
      dag(dag &&other);

      //! Move assignment operator.
      /*!
        The move assignment operator moves ownership of the graph data
        structure from this to \ref other.
      */
      dag &operator=(dag &&other);

      //! Clones this object into an identical dag.
      /*!
        A member function of \ref dag that makes an identical copy of this
        and returns it.

        \return A \ref dag that is an identical copy of this.
      */
      dag clone();

      //! Adds a disconnected \ref dag_vertex into the graph.
      /*!
        A member function of \ref dag that adds a \ref dag_vertex to the
        graph data structure. If the insertion fails then this function
        returns false otherwise it returns ture.

        \param[in] v A \ref dag_vertex to be added to the \ref dag.
        \return True if \ref v is inserted or false other wise.
      */
      bool add_vertex(dag_vertex &&v);

      //! Finds a \ref dag_vertex in the graph
      /*!
        A member function of \ref dag that finds a \ref v in the graph
        data structure and returns a \ref std::weak_ptr<dag_vertex> to
        the original \ref dag_vertex stored in the graph.

        \param[in] v A \ref dag_vertex used to find the the \ref dag.
        \return A \ref std::weak_ptr<dag_vertex> to the original
                \ref dag_vertex stored in the graph. If \ref v is not found
                then an expired \ref std::weak_ptr<dag_vertex> is returned.
      */
      std::weak_ptr<dag_vertex> find_vertex(const dag_vertex &v);

      //! Finds a \ref dag_vertex in the graph
      /*!
        A member function of \ref dag that finds a \ref v in the graph
        data structure and returns a \ref std::weak_ptr<dag_vertex> to
        the original \ref dag_vertex stored in the graph based on its
        \ref uuid..

        \param[in] u A \ref uuid used to find the the \ref dag.
        \return A \ref std::weak_ptr<dag_vertex> to the original
                \ref dag_vertex stored in the graph. If a \ref dag_vertex
                \ref v is not found then an expired
                \ref std::weak_ptr<dag_vertex> is returned.
      */
      std::weak_ptr<dag_vertex> find_vertex_by_uuid(const uuid &u);

      //! Finds all \ref dag_vertex (s) in the graph from a \ref label.
      /*!
        A member function of \ref dag that finds all \ref dag_vertex (s) in
        the graph and returns a \ref std::vector<std::weak_ptr<dag_vertex>>
        of those \ref dag_vertex (s).

        \param[in] l A \ref std::string that contains the label of the
                     \ref dag_vertex (s) that you are interested in.
        \return A \ref std::vector<std::weak_ptr<dag_vertex>> that contains
                a \ref std::weak_ptr to a collection of \ref dag_vertex (s)
                contained within the graph.
      */
      std::vector<std::weak_ptr<dag_vertex>> find_all_verticies_with_label(
        const std::string &l);

      //! Returns true if the \ref dag_vertex \ref v is in the graph.
      /*
        A member function of \ref dag
      */
      bool contains_vertex(const dag_vertex &v);

      //!
      /*
      */
      bool contains_vertex_by_uuid(const uuid &u);

      //!
      /*
      */
      bool contains_vertex_by_label(const std::string &l);

      //!
      /*
      */
      bool connection_would_make_cyclic(const dag_vertex &v1,
        const dag_vertex &v2);

      //!
      /*
      */
      bool connection_would_make_cyclic_by_uuid(const uuid &u1,
        const uuid &u2);

      //!
      /*
      */
      bool connection_would_make_cyclic_by_label(const std::string &l1,
        const std::string &l2);

      //!
      /*
      */
      bool connect(const dag_vertex &v1, const dag_vertex &v2);

      //!
      /*
      */
      bool connect_by_uuid(const uuid &u1, const uuid &u2);

      //!
      /*
      */
      bool connect_all_by_label(const std::string l1, const std::string l2);

      //!
      /*
      */
      bool add_and_connect(dag_vertex &&v1, dag_vertex &&v2);

      //!
      /*
      */
      bool are_connected(const dag_vertex &v1, const dag_vertex &v2);

      //!
      /*
      */
      bool are_connected_by_uuid(const uuid &u1, const uuid &u2);

      //!
      /*
      */
      bool all_are_connected_by_label(const std::string l1,
        const std::string l2);

      //!
      /*
      */
      void linear_traversal(
        std::function<void (std::shared_ptr<dag_vertex>)> cb);

      //!
      /*
      */
      std::size_t vertex_count() const;

      //!
      /*
      */
      std::size_t edge_count() const;

      //!
      /*
      */
      const std::string &title() const;

      //!
      /*
      */
      void reset();

    protected:
      std::shared_ptr<dag_vertex> get_vertex_at(std::size_t i);
      void clone_connections(dag_vertex &from, dag_vertex &to);

      dag(const dag &other);
      dag &operator=(const dag &rhs);

    private:
      typedef std::vector<std::shared_ptr<dag_vertex>> dag_graph;
      dag_graph graph_;
      std::string title_;

    public:
      friend std::ostream &operator<<(std::ostream &out, const dag &g);
      friend bool operator==(const dag &lhs, const dag &rhs);
      friend bool operator!=(const dag &lhs, const dag &rhs);
    };
  }
}
#endif
