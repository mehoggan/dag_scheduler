#include <exception>
#include <yaml-cpp/yaml.h>

#include "dag_scheduler/dag.h"

namespace com
{
  namespace dag_scheduler
  {
    /**
     * @brief A class used to deserialize YAML into a DAG.
     *
     * The definition in YAML for a DAG would be of the form:
     * DAG:
     *   Title: <optional string>
     *     Vertices:
     *       - Vertex:
     *         Name: <optional string>
     *         UUID: <valid uuid4 string>
     *         Task:
     *           Name: <optional string>
     *           Callback: <optional>
     *             LibraryName: <string>
     *             MethodName: <string>
     *           Stages:
     *             - Stage:
     *               Name: <optional string>
     *               Type: <namespaced C++ class>
     *             ...
     *       ...
     *       Connections:
     *         - Connection:
     *           From: <valid uuid4 string from vertex>
     *           To: <valid uuid4 string from vertex>
     *         ...
     */
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
      const static std::string DAG_KEY;
      const static std::string VERTICES_KEY;
      const static std::string TASK_KEY;
      const static std::string STAGES_KEY;

      const static std::string TITLE_KEY;
      const static std::string NAME_KEY;
      const static std::string UUID_KEY;
      const static std::string CALLBACK_KEY;
      const static std::string LIBRARY_NAME_KEY;
      const static std::string METHOD_NAME_KEY;

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

    public:
      YAMLDagDeserializer();

      std::unique_ptr<DAG> make_dag(const YAML::Node &dag_node) const;

    private:
      void make_vertices(const YAML::Node &vertices_node,
        std::unique_ptr<DAG> &dag) const;
      void make_task(const YAML::Node &task_node,
        std::unique_ptr<Task> &task) const;
      void throw_wrong_type(const UpTo &upto, const std::string &error) const;
      std::function<void (bool)> make_task_callback(
        const YAML::Node &callback_node) const;

    public:
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
