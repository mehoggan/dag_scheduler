////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "dag_scheduler/DagAlgorithms.h"

#include <cassert>

#include "dag_scheduler/DagEdge.h"
#include "dag_scheduler/DagVertex.h"
#include "dag_scheduler/Logging.h"

namespace com::dag_scheduler {
std::vector<std::shared_ptr<DAGVertex>> dagVerticesWithNoIncomingEdges(
        DAG& dag) {
    std::vector<std::shared_ptr<DAGVertex>> output;

    dag.linearTraversal([&](std::shared_ptr<DAGVertex> vertex) {
        if (!vertex->hasIncomingEdges()) {
            output.push_back(vertex);
        }
    });

    return output;
}

std::atomic_bool dag_topological_sort_init(false);
bool dagTopologicalSort(DAG& dag, std::list<DAGVertex>& sorted_vertices) {
    bool ret_val = true;

    LogTag log_tag(__FUNCTION__);
    Logging::addStdCoutLogger(log_tag);
    dag_topological_sort_init.store(true);

    sorted_vertices.clear();
    // Sorted nodes.
    std::list<std::shared_ptr<DAGVertex>> sorted_nodes;
    // Set of nodes with no incoming edges.
    std::vector<std::shared_ptr<DAGVertex>> start_nodes =
            dagVerticesWithNoIncomingEdges(dag);
    std::list<std::shared_ptr<DAGVertex>> start_nodes_list(start_nodes.begin(),
                                                           start_nodes.end());

    while (!start_nodes_list.empty()) {
        std::shared_ptr<DAGVertex> curr = start_nodes_list.front();
        start_nodes_list.pop_front();
        sorted_nodes.push_back(curr);

        curr->visitAllEdges([&](const DAGEdge& edge) {
            DAGEdge& e_ref = *const_cast<DAGEdge*>(&edge);
            std::shared_ptr<DAGVertex> vertex_lock =
                    e_ref.getConnection().lock();
            e_ref.connectTo(nullptr);
            assert(vertex_lock &&
                   "Removing connection got rid of shared_ptr.");
            if (!(vertex_lock->hasIncomingEdges())) {
                start_nodes_list.push_back(vertex_lock);
            }
        });
    }

    ret_val = (sorted_nodes.size() != dag.vertexCount());
    for (const std::shared_ptr<DAGVertex>& vertex : sorted_nodes) {
        sorted_vertices.push_back(vertex->clone());
    }

    if (ret_val) {
        Logging::fatal(log_tag,
                       "Failed to sort vertices.",
                       (dag.vertexCount() - sorted_nodes.size()),
                       "failed to sort.");
    }

    return ret_val;
}

// TODO (mehoggan@gmail.com): Verify that this is tested.
bool processDag(DAG& dag,
                processed_order_type& out,
                TaskScheduler& scheduler) {
    bool ret_val = false;
    LogTag log_tag(__FUNCTION__);
    Logging::addStdCoutLogger(log_tag);

    DAG g_clone = dag.clone();
    DAG g_check = dag.clone();

    std::list<DAGVertex> check;
    ret_val = !dagTopologicalSort(g_check, check);
    if (ret_val && check.size() == g_clone.vertexCount()) {
        out.clear();
        std::vector<std::shared_ptr<DAGVertex>>
                curr_dag_vertices_with_no_incoming_edges =
                        dagVerticesWithNoIncomingEdges(g_clone);

        std::vector<DAGVertex> vertices_to_process;
        while (!curr_dag_vertices_with_no_incoming_edges.empty()) {
            std::for_each(
                    curr_dag_vertices_with_no_incoming_edges.begin(),
                    curr_dag_vertices_with_no_incoming_edges.end(),
                    [&](std::shared_ptr<DAGVertex> vertex) {
                        DAGVertex v_clone = vertex->clone();
                        assert(g_clone.removeVertex(*(vertex.get())) &&
                               "Failed to remove vertex from graph.");
                        vertices_to_process.push_back(std::move(v_clone));
                    });

            out.push_back({});
            for (DAGVertex& vertex : vertices_to_process) {
                out.back().push_back(vertex.clone());
            }
            std::for_each(vertices_to_process.begin(),
                          vertices_to_process.end(),
                          [&](DAGVertex& vertex) {
                              scheduler.queueTask(std::move(vertex.getTask()));
                          });
            vertices_to_process.clear();

            curr_dag_vertices_with_no_incoming_edges.clear();
            curr_dag_vertices_with_no_incoming_edges =
                    dagVerticesWithNoIncomingEdges(g_clone);
        }
    } else {
        Logging::fatal(log_tag, dag.title(), "was cyclic.");
    }

    return ret_val;
}
}  // namespace com::dag_scheduler
