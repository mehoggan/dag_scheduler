////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef DAG_SCHEDULER_DAGALGORITHMS_H
#define DAG_SCHEDULER_DAGALGORITHMS_H

#include <list>

#include "dag_scheduler/Dag.h"
#include "dag_scheduler/TaskScheduler.h"

namespace com::dag_scheduler {
typedef std::vector<std::vector<DAGVertex>> processed_order_type;

/**
 * @brief Takes a dag and finds all dag_vertices with no incoming edges.
 *
 * Takes in a dag and returns all \ref DAGVertex with no incoming
 * \ref dag_edge (s).
 *
 * @param[in] dag The \ref dag for which to apply the search.
 *
 * @return A \ref std::vector<\ref std::shared_ptr<\ref DAGVertex>> of
 *         all the \ref DAGVertex (s) which have no incoming edges.
 */
std::vector<std::shared_ptr<DAGVertex>> dagVerticesWithNoIncomingEdges(
        DAG& dag);

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
bool dagTopologicalSort(DAG& dag, std::list<DAGVertex>& sorted_vertices);

/**
 * @brief Takes a dag and processes in parallel all \ref DAGVertex that
 *        can be grouped by checking for all \ref DAGVertex with no
 *        incoming dag_edge (s). Subsequently it removes them and
 *        continues on until not \ref DAGVertex (s) are left in \ref g.
 *
 * Takes a dag and process in parallel all \ref DAGVertex that can
 * be grouped by checking for all \ref DAGVertex with no incoming
 * \ref dag_edge (s). Subsequently it removes them and continues on
 * until not \ref DAGVertex (s) are left in \ref g.
 *
 * @param[in] dag The \ref dag to sort.
 * @param[out] out A \ref std::vector<\ref std::vector<\ref DAGVertex>>
 *                 which is an ordered set of collections that contain
 *                 a set of \ref DAGVertex which represents batches
 *                 of \ref DAGVertex (s) processed.
 *
 * @return False if \ref g could not have all \ref DAGVertex (s)
 *         visited.
 */
bool processDAG(DAG& dag, processed_order_type& out, TaskScheduler& scheduler);
}  // namespace com::dag_scheduler

#endif
