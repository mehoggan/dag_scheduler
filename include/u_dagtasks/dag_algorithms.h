#ifndef DAG_ALGORITHMS_H_INCLUDED
#define DAG_ALGORITHMS_H_INCLUDED

#include "declspec.h"

#include "u_dagtasks/dag.h"

#include <list>

namespace uber
{
  namespace u_dagtasks
  {
    bool dag_topological_sort(dag &g,
      std::list<dag_vertex> &sorted_vertices);
  }
}

#endif
