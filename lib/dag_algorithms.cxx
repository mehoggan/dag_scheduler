#include "dag_scheduler/dag_algorithms.h"

#include "dag_scheduler/concurrent_queue.hpp"
#include "dag_scheduler/dag_edge.h"
#include "dag_scheduler/dag_vertex.h"
#include "dag_scheduler/logging.h"

#include <cassert>

namespace com
{
  namespace dag_scheduler
  {
    namespace detail
    {
      std::vector<std::shared_ptr<dag_vertex>>
      dag_vertices_with_no_incomming_edges(dag &g)
      {
        std::vector<std::shared_ptr<dag_vertex>> output;

        g.linear_traversal([&](std::shared_ptr<dag_vertex> v) {
            if (!v->has_incomming_edges()) {
              output.push_back(v);
            }
          }
        );

        return output;
      }
    }

    // TODO (mhoggan): Fix in logging not here.
    std::atomic_bool dag_topological_sort_init(false);
    bool dag_topological_sort(dag &g, std::list<dag_vertex> &sorted_vertices)
    {
      bool ret = true;

      log_tag LOG_TAG(__FUNCTION__);
      if (!dag_topological_sort_init.load()) {
        logging::add_std_cout_logger(LOG_TAG);
        logging::add_std_cerr_logger(LOG_TAG);
        dag_topological_sort_init.store(true);
      }

      sorted_vertices.clear();
      // Sorted nodes.
      std::list<std::shared_ptr<dag_vertex>> L;
      // Set of nodes with no incomming edges.
      auto Qv = detail::dag_vertices_with_no_incomming_edges(g);
      std::list<std::shared_ptr<dag_vertex>> Q(Qv.begin(), Qv.end());

      while (!Q.empty()) {
        std::shared_ptr<dag_vertex> curr = Q.front();
        Q.pop_front();
        L.push_back(curr);

        curr->visit_all_edges([&](const dag_edge &e) {
            dag_edge &e_ref = *const_cast<dag_edge *>(&e);
            std::shared_ptr<dag_vertex> m = e_ref.get_connection().lock();
            e_ref.connect_to(nullptr);
            assert(m && "Removing connection got rid of shared_ptr.");
            if (!(m->has_incomming_edges())) {
              Q.push_back(m);
            }
          }
        );
      }

      ret = (L.size() != g.vertex_count());
      for (std::shared_ptr<dag_vertex> v : L) {
        sorted_vertices.push_back(v->clone());
      }

      if (ret) {
        logging::fatal(LOG_TAG, "Failed to sort vertices.",
          (g.vertex_count() - L.size()), "failed to sort.");
      }

      return ret;
    }

    // TODO (mhoggan): Fix in logging not here.
    std::atomic_bool process_dag_init(false);
    bool process_dag(dag &g)
    {
      bool ret = false;
      log_tag LOG_TAG(__FUNCTION__);
      if (!process_dag_init.load()) {
        logging::add_std_cout_logger(LOG_TAG);
        logging::add_std_cerr_logger(LOG_TAG);
        process_dag_init.store(true);
      }

      dag g_clone = g.clone();

      std::list<dag_vertex> dummy;
      ret = !dag_topological_sort(g_clone, dummy);
      if (ret) {
        assert(dummy.size() == g_clone.vertex_count() &&
          "Topological sort of graph failed and return value was wrong.");
        std::vector<std::shared_ptr<dag_vertex>>
          curr_dag_vertices_with_no_incomming_edges =
            detail::dag_vertices_with_no_incomming_edges(g);

        std::vector<dag_vertex> vertices_to_process;
        while (curr_dag_vertices_with_no_incomming_edges.empty()) {
          std::for_each(curr_dag_vertices_with_no_incomming_edges.begin(),
            curr_dag_vertices_with_no_incomming_edges.end(),
            [&](std::shared_ptr<dag_vertex> v) {
              dag_vertex v_clone = v->clone();
              assert(g_clone.remove_vertex(*(v.get())) &&
                "Failed to remove vertex from graph.");
              vertices_to_process.push_back(std::move(v_clone));
            }
          );
        }

        logging::info(LOG_TAG, "There were", vertices_to_process.size(),
          "pulled from graph to process.");

        // TODO (mhoggan): Add vertices_to_process to scheduler.
        std::for_each(vertices_to_process.begin(), vertices_to_process.end(),
          [&](dag_vertex &v) {
            logging::info(LOG_TAG, "Gonig to process", v.label());
          });

        curr_dag_vertices_with_no_incomming_edges.clear();
        curr_dag_vertices_with_no_incomming_edges =
          detail::dag_vertices_with_no_incomming_edges(g);
      } else {
        logging::fatal(LOG_TAG, g.title(), "was cyclic.");
      }

      return ret;
    }
  }
}
