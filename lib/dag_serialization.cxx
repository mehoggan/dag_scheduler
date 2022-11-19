#include "dag_scheduler/dag_serialization.h"

namespace com
{
  namespace dag_scheduler
  {
    YAMLDagDeserializerError::YAMLDagDeserializerError(
      const std::string &what) :
      what_(what)
    {}

    const char *YAMLDagDeserializerError::what() const noexcept
    {
      return what_.c_str();
    }

    YAMLDagDeserializer::YAMLDagDeserializer() :
      LoggedClass<YAMLDagDeserializer>(*this)
    {}

    std::unique_ptr<DAG> YAMLDagDeserializer::make_dag(
      const YAML::Node &dag_node)
    {
      Logging::info(LOG_TAG, "Going to build DAG from\n", dag_node);

      std::unique_ptr<DAG> ret;
      if (dag_node["DAG"]) {
        const YAML::Node dag_definition_node = dag_node["DAG"];
        if (dag_definition_node["Title"]) {
          auto title = dag_definition_node["Title"].as<std::string>();
          Logging::info(LOG_TAG, "Going to build DAG with title", title,
            "from the following definition", dag_definition_node);
          ret = std::make_unique<DAG>(title);
        } else {
          Logging::warn(LOG_TAG, "No title specified is this intended?");
          ret = std::make_unique<DAG>();
        }

        if (dag_definition_node["Veritices"]) {
           if (dag_definition_node.IsMap()) {
           } else {
           }
        } else {
        std::stringstream ss;
        ss << dag_node;
          Logging::warn(LOG_TAG, "No \"Vertices\": map found in", dag_node);
        }
      } else {
        std::stringstream ss;
        ss << dag_node;
        throw YAMLDagDeserializerError(
          "Root element of \"DAG\" not found in\n" + ss.str());
      }

      return ret;
    }

    void YAMLDagDeserializer::make_vertices(const YAML::Node &vertices_node,
      std::unique_ptr<DAG> &dag)
    {
      (void) vertices_node;
      (void) dag;
    }
  }
}
