#ifndef DAG_ALGORITHMS_H_INCLUDED
#define DAG_ALGORITHMS_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/dag.h"

#include <list>

namespace com
{
  namespace dag_scheduler
  {
    typedef std::vector<std::vector<dag_vertex>> processed_order_type;

    //! Takes a dag and finds all dag_vertices with no incomming edges.
    /*!
      Takes in a dag and returns all \ref dag_vertex with no incomming
      \ref dag_edge (s).

      \param[in] g The \ref dag for which to apply the search.
      \return A \ref std::vector<\ref std::shared_ptr<\ref dag_vertex>> of
              all the \ref dag_vertex (s) which have no incomming edges.
    */
    DLLSPEC_DAGTASKS std::vector<std::shared_ptr<dag_vertex>>
    dag_vertices_with_no_incomming_edges(dag &g);

    //! Takes a dag and returns a list of \ref dag_vertex (s) in "order".
    /*!
      A function that takes in a dag and returns a ordering of a directed
      graph is a linear ordering of its vertices such that for every directed
      \ref dag_edge uv from \ref dag_vertex u to \ref dag_vertex v, u comes
      before v in the ordering

      \param[in] g The \ref dag to sort.
      \param[out] sorted_vertices The topological sorted \ref dag_vertex
                                  from \ref g.
      \return True \ref dag has a cycle, false otherwise.
    */
    DLLSPEC_DAGTASKS bool dag_topological_sort(dag &g,
      std::list<dag_vertex> &sorted_vertices);

    //! Takes a dag and processes in parallel all \ref dag_vertex that can
    //! be grouped by checking for all \ref dag_vertex with no incomming
    //! \ref dag_edge (s). Subsequently it removes them and continues on
    //! until not \ref dag_vertex (s) are left in \ref g.
    /*!
      Takes a dag and process in parallel all \ref dag_vertex that can
      be grouped by checking for all \ref dag_vertex with no incomming
      \ref dag_edge (s). Subsequently it removes them and continues on
      until not \ref dag_vertex (s) are left in \ref g.

      \param[in] g The \ref dag to sort.
      \param[out] out A \ref std::vector<\ref std::vector<\ref dag_vertex>>
                      which is an orderd set of collections that contain
                      a set of \ref dag_vertex which represents batches
                      of \ref dag_vertex (s) processed.
      \return False if \ref g could not have all \ref dag_vertex (s)
              visited.
    */
    DLLSPEC_DAGTASKS bool process_dag(dag &g, processed_order_type &out);
  }
}

#endif
