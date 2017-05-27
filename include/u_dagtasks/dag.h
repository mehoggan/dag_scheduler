#ifndef DAG_H_INCLUDED
#define DAG_H_INCLUDED

#include "declspec.h"

#include "u_dagtasks/dag_vertex.h"
#include "u_dagtasks/uuid.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <unordered_map>
#include <vector>

namespace uber
{
  namespace u_dagtasks
  {
    class dag_edge;
    class dag_vertex;

    class DLLSPEC_U_DAGTASKS dag
    {
    public:
        class dag_exception : public std::runtime_error
        {
        public:
            explicit dag_exception(const char *message);

            virtual const char *what() const throw();
        };

    public:
      dag();
      ~dag();

      dag(dag &&other);
      dag &operator=(dag &&other);

      dag clone();

      bool add_vertex(dag_vertex &v);

      std::weak_ptr<dag_vertex> find_vertex(const dag_vertex &v);
      std::weak_ptr<dag_vertex> find_vertex_by_uuid(const uuid &u);
      std::vector<std::weak_ptr<dag_vertex>> find_all_verticies_with_label(
        const std::string &l);

      bool contains_vertex(const dag_vertex &v);
      bool contains_vertex_by_uuid(const uuid &u);
      bool contains_vertex_by_label(const std::string &l);

      bool connection_would_make_cyclic(const dag_vertex &v1,
        const dag_vertex &v2);
      bool connection_would_make_cyclic_by_uuid(const uuid &u1,
        const uuid &u2);
      bool connection_would_make_cyclic_by_label(const std::string &l1,
        const std::string &l2);

      bool connect(const dag_vertex &v1, const dag_vertex &v2);
      bool connect_by_uuid(const uuid &u1, const uuid &u2);
      bool connect_all_by_label(const std::string l1, const std::string l2);

      bool add_and_connect(dag_vertex &v1, dag_vertex &v2);

      bool are_connected(const dag_vertex &v1, const dag_vertex &v2);
      bool are_connected_by_uuid(const uuid &u1, const uuid &u2);
      bool all_are_connected_by_label(const std::string l1,
        const std::string l2);

      void linear_traversal(
        std::function<void (std::shared_ptr<dag_vertex>)> cb);

      std::size_t vertex_count() const;
      std::size_t edge_count() const;

    private:
      dag(const dag &other);
      dag &operator=(const dag &rhs);

      typedef std::vector<std::shared_ptr<dag_vertex>> dag_graph;
      dag_graph graph_;
      // TODO: Add this in.
      // std::unordered_map<std::string, std::shared_ptr<dag_vertex>> lu_;
    };
  }
}
#endif
