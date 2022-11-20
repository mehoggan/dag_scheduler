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

    class YAMLDagWrongTypeError : public YAMLDagDeserializerError
    {
    public:
      explicit YAMLDagWrongTypeError(const std::string &what);
    };

    class YAMLDagDeserializer : public LoggedClass<YAMLDagDeserializer>
    {
    public:
      YAMLDagDeserializer();

      std::unique_ptr<DAG> make_dag(const YAML::Node &dag_node);
      void make_vertices(const YAML::Node &vertices_node,
        std::unique_ptr<DAG> &dag);

    public:
      enum class UpTo
      {
        CONNECTIONS,
        EMPTY,
        DAG,
        VERTICES,
        VERTEX,
        TASK,
        STAGES,
        STAGE
      };

      static std::string sample_dag_output(const UpTo &upto);
      static std::string full_sample_output();

    private:
      static void stage_str(std::string &ret);
      static void stages_str(std::string &ret);
      static void task_str(std::string &ret);
      static void vertex_str(std::string &ret);
      static void verticies_str(std::string &ret);
      static void connections_str(std::string &ret);
      static void dag_str(std::string &ret);
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
