#include "dag_scheduler/dag_serialization.h"

#include <iostream>

namespace com
{
  namespace dag_scheduler
  {
    YAMLDagDeserializerError::YAMLDagDeserializerError(
      const std::string &what) :
      std::exception(),
      what_(what)
    {}

    const char *YAMLDagDeserializerError::what() const noexcept
    {
      return what_.c_str();
    }

    YAMLDagWrongTypeError::YAMLDagWrongTypeError(
      const std::string &what) :
      YAMLDagDeserializerError(what)
    {}


    YAMLDagDeserializer::YAMLDagDeserializer() :
      LoggedClass<YAMLDagDeserializer>(*this)
    {}

    std::string n2s(const YAML::Node &node)
    {
      std::stringstream ss;
      ss << node;
      return ss.str();
    }

    void YAMLDagDeserializer::stage_str(std::string &ret)
    {
      ret = std::string("          - Stage:\n") +
        std::string("            Name: <optional string>\n") +
        std::string("            Type: <namespaced C++ class>\n") +
        std::string("          ...\n") +
        std::string("    ...\n");
    }

    void YAMLDagDeserializer::stages_str(std::string &ret)
    {
      if (ret.empty()) {
        ret = std::string("        Stages:\n") +
          std::string("          ...");
      } else {
        ret = std::string("        Stages:\n") + ret;
      }
    }

    void YAMLDagDeserializer::task_str(std::string &ret)
    {
      if (ret.empty()) {
        ret = std::string("      Task:\n") +
          std::string("        Name: <optional string>\n") +
          std::string("        ...");
      } else {
        ret = std::string("      Task:\n") +
          std::string("        Name: <optional string>\n") +
          ret;
      }
    }

    void YAMLDagDeserializer::vertex_str(std::string &ret)
    {
      if (ret.empty()) {
        ret = std::string("    - Vertex:\n") +
          std::string("      Name: <optional string>\n") +
          std::string("      UUID: <valid uuid4 string>\n") +
          std::string("        ...");
      } else {
        ret = std::string("    - Vertex:\n") +
          std::string("      Name: <optional string>\n") +
          std::string("      UUID: <valid uuid4 string>\n") +
          ret;
      }
    }

    void YAMLDagDeserializer::verticies_str(std::string &ret)
    {
      if (ret.empty()) {
        ret = std::string("  Vertices:\n") +
          std::string("    ...");
      } else {
        ret = std::string("  Vertices:\n") + ret;
      }
    }

    void YAMLDagDeserializer::connections_str(std::string &ret)
    {
      ret = std::string("  Connections:\n") +
        std::string("    - Connection:\n") +
        std::string("      From: <valid uuid4 string from vertex>\n") +
        std::string("      To: <valid uuid4 string from vertex>\n") +
        std::string("    ...");
    }

    void YAMLDagDeserializer::dag_str(std::string &ret)
    {
      if (ret.empty()) {
        ret = std::string("DAG:\n") +
          std::string("  Title: <optional string>\n") +
          std::string("  ...");
      } else {
        ret = std::string("DAG:\n") +
          std::string("  Title: <optional string>\n") +
          ret;
      }
    }

    std::string YAMLDagDeserializer::sample_dag_output(const UpTo &upto)
    {
      std::string ret;

      switch (upto) {
      case UpTo::STAGE: stage_str(ret);
      case UpTo::STAGES: stages_str(ret);
      case UpTo::TASK: task_str(ret);
      case UpTo::VERTEX: vertex_str(ret);
      case UpTo::VERTICES: verticies_str(ret);
      case UpTo::DAG: dag_str(ret);
      case UpTo::EMPTY: {}
      case UpTo::CONNECTIONS: {
        // Here we do nothing, because we will just append it to the bottom
        // of whatever depth in the YAML tree the user has specified to
        // visualize the YAML definition for a DAG.
      }
      }

      if (upto == UpTo::CONNECTIONS) {
        connections_str(ret);
      }

      if (ret.back() != '\n') {
        ret += "\n";
      }

      return ret;
    }

    std::string YAMLDagDeserializer::full_sample_output()
    {
      std::string stage_output = sample_dag_output(UpTo::STAGE);
      std::string connections_output = sample_dag_output(UpTo::CONNECTIONS);
      return stage_output + connections_output;
    }

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
             throw YAMLDagWrongTypeError(
               "Verticies was not declared correctly in\n" +
               n2s(dag_definition_node) + "\nLooking for\n" +
               "DAG:\n  Vertices:\n    Vertex:\n      ...");
           }
        } else {
          Logging::warn(LOG_TAG, "No \"Vertices\": map found in",
            n2s(dag_node));
        }
      } else {
        std::string sample = YAMLDagDeserializer::sample_dag_output(
          YAMLDagDeserializer::UpTo::DAG);
        throw YAMLDagDeserializerError(
          "Root element of \"DAG\" not found in\n" + n2s(dag_node) +
          "\nFor example, as in\n" + sample);
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
