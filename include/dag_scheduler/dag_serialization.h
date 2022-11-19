#include <exception>
#include <yaml-cpp/yaml.h>

#include "dag_scheduler/dag.h"

namespace com
{
  namespace dag_scheduler
  {
    class YAMLDagDeserializerError : public std::exception
    {
    public:
      explicit YAMLDagDeserializerError(const std::string &what);

      const char *what() const noexcept override;
    private:
      std::string what_;
    };

    class YAMLDagDeserializer : public LoggedClass<YAMLDagDeserializer>
    {
    public:
      YAMLDagDeserializer();

      std::unique_ptr<DAG> make_dag(const YAML::Node &dag_node);
      void make_vertices(const YAML::Node &vertices_node,
        std::unique_ptr<DAG> &dag);
    };
  }
}

namespace YAML
{
  template<>
  struct convert<std::unique_ptr<com::dag_scheduler::DAG>>
  {
    static bool decode(const Node &node,
      std::unique_ptr<com::dag_scheduler::DAG> &rhs)
    {
      (void) node, (void) rhs;
      com::dag_scheduler::YAMLDagDeserializer deserializer;
      rhs = deserializer.make_dag(node);
      return true;
    }
  };
}
