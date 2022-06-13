#ifndef DAG_ALGORITHMS_H_INCLUDED
#define DAG_ALGORITHMS_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/dag.h"
#include "dag_scheduler/task_scheduler.h"

#include <list>

namespace com
{
  namespace dag_scheduler
  {
    typedef std::vector<std::vector<DAGVertex>> processed_order_type;

    /**
     * @brief Takes a dag and finds all dag_vertices with no incomming edges.
     *
     * Takes in a dag and returns all \ref DAGVertex with no incomming
     * \ref dag_edge (s).
     *
     * @param[in] g The \ref dag for which to apply the search.
     *
     * @return A \ref std::vector<\ref std::shared_ptr<\ref DAGVertex>> of
     *         all the \ref DAGVertex (s) which have no incomming edges. 
     */
    DLLSPEC_DAGTASKS std::vector<std::shared_ptr<DAGVertex>>
    dag_vertices_with_no_incomming_edges(DAG &g);

    /**
     * @brief Takes a dag and returns a list of \ref DAGVertex (s) in
     *        "order".
     *
     * A function that takes in a dag and returns a ordering of a directed
     * graph is a linear ordering of its vertices such that for every directed
     * \ref dag_edge uv from \ref DAGVertex u to \ref DAGVertex v, u comes
     * before v in the ordering
     *
     * @param[in] g The \ref dag to sort.
     * @param[out] sorted_vertices The topological sorted \ref DAGVertex
     *                             from \ref g.
     *
     * @return true \ref dag has a cycle, false otherwise. 
     */
    DLLSPEC_DAGTASKS bool dag_topological_sort(DAG &g,
      std::list<DAGVertex> &sorted_vertices);

    /**
     * @brief Takes a dag and processes in parallel all \ref DAGVertex that
     *        can be grouped by checking for all \ref DAGVertex with no
     *        incomming dag_edge (s). Subsequently it removes them and
     *        continues on until not \ref DAGVertex (s) are left in \ref g.
     *
     * Takes a dag and process in parallel all \ref DAGVertex that can
     * be grouped by checking for all \ref DAGVertex with no incomming
     * \ref dag_edge (s). Subsequently it removes them and continues on
     * until not \ref DAGVertex (s) are left in \ref g.
     *
     * @param[in] g The \ref dag to sort.
     * @param[out] out A \ref std::vector<\ref std::vector<\ref DAGVertex>>
     *                 which is an orderd set of collections that contain
     *                 a set of \ref DAGVertex which represents batches
     *                 of \ref DAGVertex (s) processed.
     *
     * @return False if \ref g could not have all \ref DAGVertex (s)
     *         visited. 
     */
    DLLSPEC_DAGTASKS bool process_dag(DAG &g, processed_order_type &out,
      TaskScheduler &scheduler);
  }
}

#endif
