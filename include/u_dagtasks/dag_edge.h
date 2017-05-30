#ifndef DAG_EDGE_H_INCLUDED
#define DAG_EDGE_H_INCLUDED

#include "declspec.h"

#include "u_dagtasks/dag_vertex.h"
#include "u_dagtasks/uuid.h"

#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace uber
{
  namespace u_dagtasks
  {
    class DLLSPEC_U_DAGTASKS dag_edge
    {
    private:
      friend class dag_vertex;

    public:
      // TODO: Use dag class to manage status.
      enum class status
      {
        initialized,
        traversed,
        non_traverable
      };

    public:
      dag_edge();
      virtual ~dag_edge();

      dag_edge(dag_edge &&other);
      dag_edge &operator=(dag_edge &&rhs);

      dag_edge clone();

      std::weak_ptr<dag_vertex> get_connection();
      bool connect_to(std::shared_ptr<dag_vertex> v);
      bool is_a_connection_to(const dag_vertex &v) const;

      const uuid &get_uuid() const;
      const status &current_status() const;
      std::string current_status_as_string() const;

    protected:
      dag_edge(const dag_edge &other);
      dag_edge &operator=(const dag_edge &rhs);

    private:
      class uuid uuid_;
      status current_status_;
      std::weak_ptr<dag_vertex> connection_;

    public:
      friend std::ostream &operator<<(std::ostream &out, const dag_edge &e);
      friend bool operator==(const dag_edge &lhs, const dag_edge &rhs);
      friend bool operator!=(const dag_edge &lhs, const dag_edge &rhs);
    };
  }
}
#endif
