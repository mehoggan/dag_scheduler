#include "u_dagtasks/dag_algorithms.h"

#include "u_dagtasks/dag_edge.h"

#include <cassert>

namespace uber
{
  namespace u_dagtasks
  {
    bool dag_topological_sort(dag &g,
      std::list<dag_vertex> &sorted_vertices)
    {
      bool ret = true;

      sorted_vertices.clear();
      // Sorted nodes.
      std::list<std::shared_ptr<dag_vertex>> L;
      // Set of nodes with no incomming edges.
      std::list<std::shared_ptr<dag_vertex>> Q;

      g.linear_traversal([&](std::shared_ptr<dag_vertex> v) {
          if (!v->has_incomming_edges()) {
            Q.push_back(v);
          }
        }
      );

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

      ret &= (L.size() == g.vertex_count());
      for (std::shared_ptr<dag_vertex> v : L) {
        sorted_vertices.push_back(v->clone());
      }

      return ret;
    }
  }
}
