#ifndef DAG_ALGORITHMS_H_INCLUDED
#define DAG_ALGORITHMS_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/dag.h"

#include <list>

namespace com
{
  namespace dag_scheduler
  {
    DLLSPEC_DAGTASKS bool dag_topological_sort(dag &g,
      std::list<dag_vertex> &sorted_vertices);
  }
}

#endif
