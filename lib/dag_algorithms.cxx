#include "dag_scheduler/dag_algorithms.h"

#include <cassert>

#include "dag_scheduler/dag_edge.h"
#include "dag_scheduler/dag_vertex.h"
#include "dag_scheduler/logging.h"

namespace com {
namespace dag_scheduler {
std::vector<std::shared_ptr<DAGVertex>> dag_vertices_with_no_incomming_edges(
    DAG &g) {
  std::vector<std::shared_ptr<DAGVertex>> output;

  g.linear_traversal([&](std::shared_ptr<DAGVertex> v) {
    if (!v->has_incomming_edges()) {
      output.push_back(v);
    }
  });

  return output;
}

std::atomic_bool dag_topological_sort_init(false);
bool dag_topological_sort(DAG &g, std::list<DAGVertex> &sorted_vertices) {
  bool ret = true;

  LogTag LOG_TAG(__FUNCTION__);
  Logging::add_std_cout_logger(LOG_TAG);
  dag_topological_sort_init.store(true);

  sorted_vertices.clear();
  // Sorted nodes.
  std::list<std::shared_ptr<DAGVertex>> L;
  // Set of nodes with no incomming edges.
  auto Qv = dag_vertices_with_no_incomming_edges(g);
  std::list<std::shared_ptr<DAGVertex>> Q(Qv.begin(), Qv.end());

  while (!Q.empty()) {
    std::shared_ptr<DAGVertex> curr = Q.front();
    Q.pop_front();
    L.push_back(curr);

    curr->visit_all_edges([&](const DAGEdge &e) {
      DAGEdge &e_ref = *const_cast<DAGEdge *>(&e);
      std::shared_ptr<DAGVertex> m = e_ref.get_connection().lock();
      e_ref.connect_to(nullptr);
      assert(m && "Removing connection got rid of shared_ptr.");
      if (!(m->has_incomming_edges())) {
        Q.push_back(m);
      }
    });
  }

  ret = (L.size() != g.vertex_count());
  for (std::shared_ptr<DAGVertex> v : L) {
    sorted_vertices.push_back(v->clone());
  }

  if (ret) {
    Logging::fatal(LOG_TAG, "Failed to sort vertices.",
                   (g.vertex_count() - L.size()), "failed to sort.");
  }

  return ret;
}

bool process_dag(DAG &g, processed_order_type &out, TaskScheduler &scheduler) {
  bool ret = false;
  LogTag LOG_TAG(__FUNCTION__);
  Logging::add_std_cout_logger(LOG_TAG);

  DAG g_clone = g.clone();
  DAG g_check = g.clone();

  std::list<DAGVertex> check;
  ret = !dag_topological_sort(g_check, check);
  if (ret && check.size() == g_clone.vertex_count()) {
    out.clear();
    std::vector<std::shared_ptr<DAGVertex>>
        curr_dag_vertices_with_no_incomming_edges =
            dag_vertices_with_no_incomming_edges(g_clone);

    std::vector<DAGVertex> vertices_to_process;
    while (!curr_dag_vertices_with_no_incomming_edges.empty()) {
      std::for_each(curr_dag_vertices_with_no_incomming_edges.begin(),
                    curr_dag_vertices_with_no_incomming_edges.end(),
                    [&](std::shared_ptr<DAGVertex> v) {
                      DAGVertex v_clone = v->clone();
                      assert(g_clone.remove_vertex(*(v.get())) &&
                             "Failed to remove vertex from graph.");
                      vertices_to_process.push_back(std::move(v_clone));
                    });

      out.push_back({});
      for (DAGVertex &v : vertices_to_process) {
        out.back().push_back(v.clone());
      }
      std::for_each(vertices_to_process.begin(), vertices_to_process.end(),
                    [&](DAGVertex &vertex) {
                      scheduler.queue_task(std::move(vertex.get_task()));
                    });
      vertices_to_process.clear();

      curr_dag_vertices_with_no_incomming_edges.clear();
      curr_dag_vertices_with_no_incomming_edges =
          dag_vertices_with_no_incomming_edges(g_clone);
    }
  } else {
    Logging::fatal(LOG_TAG, g.title(), "was cyclic.");
  }

  return ret;
}
}  // namespace dag_scheduler
}  // namespace com
