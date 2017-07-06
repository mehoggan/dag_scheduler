#ifndef DAG_ALGORITHMS_H_INCLUDED
#define DAG_ALGORITHMS_H_INCLUDED

#include "declspec.h"

#include "dagtasks/dag.h"

#include <list>

namespace com
{
  namespace dagtasks
  {
    DLLSPEC_DAGTASKS bool dag_topological_sort(dag &g,
      std::list<dag_vertex> &sorted_vertices);
  }
}

#endif
