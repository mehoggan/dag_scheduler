#ifndef DAG_VERTEX_H_INCLUDED
#define DAG_VERTEX_H_INCLUDED

#include "declspec.h"

#include "u_dagtasks/uuid.h"

#include <atomic>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace uber
{
  namespace u_dagtasks
  {
    class dag_edge;
    class dag;

    class DLLSPEC_U_DAGTASKS dag_vertex
    {
    private:
      friend class dag_edge;
      friend class dag;

    public:
      // TODO: Use dag class to manage status.
      enum class status
      {
        initialized,
        scheduled,
        running,
        passed,
        failed,
        invalid
      };

    public:
      explicit dag_vertex(const std::string &label);
      virtual ~dag_vertex();

      dag_vertex(dag_vertex &&other);
      dag_vertex &operator=(dag_vertex &&rhs);

      dag_vertex clone();

      void visit_all_edges(std::function<void (const dag_edge &)> cb) const;
      const dag_edge &get_edge(std::size_t index) const;

      bool contains_connection_to(const dag_vertex &other);
      bool connect(std::shared_ptr<dag_vertex> other);

      const uuid &get_uuid() const;
      const status &current_status() const;
      std::string current_status_as_string() const;
      const std::string &label() const;
      std::size_t edge_count() const;

      bool has_incomming_edge() const;
      std::size_t incomming_edge_count() const;

      friend std::ostream &operator<<(std::ostream &out,
        const dag_vertex &v);
      friend bool operator==(const dag_vertex &lhs, const dag_vertex &rhs);
      friend bool operator!=(const dag_vertex &lhs, const dag_vertex &rhs);

    protected:
      void add_incomming_edge();
      void sub_incomming_edge();
      void clear_edges();

    private:
      dag_vertex(const dag_vertex &other);
      dag_vertex &operator=(const dag_vertex &rhs);

      class uuid uuid_;
      status current_status_;
      std::string label_;
      std::vector<std::unique_ptr<dag_edge>> edges_;
      std::atomic<std::size_t> incomming_edge_count_;
    };
  }
}
#endif
