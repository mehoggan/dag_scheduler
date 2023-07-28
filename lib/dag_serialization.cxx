#include "dag_scheduler/dag_serialization.h"

#include <iostream>
#include <memory>

namespace com
{
  namespace dag_scheduler
  {
    const std::string YAMLDagDeserializer::DAG_KEY = "DAG";
    const std::string YAMLDagDeserializer::VERTICES_KEY = "Vertices";
    const std::string YAMLDagDeserializer::TASK_KEY = "Task";
    const std::string YAMLDagDeserializer::STAGES_KEY = "Stages";

    const std::string YAMLDagDeserializer::TITLE_KEY = "Title";
    const std::string YAMLDagDeserializer::NAME_KEY = "Name";
    const std::string YAMLDagDeserializer::UUID_KEY = "UUID";

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
      const YAML::Node &dag_node) const
    {
      Logging::info(LOG_TAG, "Going to build DAG from\n", dag_node);

      std::unique_ptr<DAG> ret;
      if (dag_node[DAG_KEY]) {
        if (not dag_node[DAG_KEY].IsMap()) {
          auto error = std::string("\"DAG\" the root elment must be a ") +
            std::string("YAML map.");
          throw_wrong_type(UpTo::DAG, error);
        } // Not sure if this will ever get triggered. But better to
          // communicate more than less to users.

        const YAML::Node &dag_definition_node = dag_node[DAG_KEY];
        std::string title;
        if (dag_definition_node[TITLE_KEY]) {
          title = dag_definition_node[TITLE_KEY].as<std::string>();
          ret = std::make_unique<DAG>(title);
        } else {
          Logging::warn(LOG_TAG, "No title specified is this intended?\n",
            "Sample input would look like:\n",
            full_sample_output());
          ret = std::make_unique<DAG>();
        }

        Logging::info(LOG_TAG, "Going to build DAG with title --", title,
          "-- from the following definition", dag_definition_node);

        if (dag_definition_node[VERTICES_KEY]) {
          const YAML::Node &vertex_node = dag_definition_node[VERTICES_KEY];
          make_vertices(vertex_node, ret);
        } else {
          Logging::warn(LOG_TAG, "No\"", VERTICES_KEY, "\" map found in",
            n2s(dag_node),
            "Sample input would look like:\n",
            full_sample_output());
        }
      } else {
        std::string sample = YAMLDagDeserializer::sample_dag_output(UpTo::DAG);
        throw YAMLDagDeserializerError(
          "Root element of \"DAG\" not found in\n" + n2s(dag_node) +
          "\nFor example, as in\n" + sample);
      }

      return ret;
    }

    void YAMLDagDeserializer::make_vertices(const YAML::Node &vertices_node,
      std::unique_ptr<DAG> &dag) const
    {
      (void) dag;
      if (vertices_node.IsSequence()) {
        const auto &vertices = vertices_node.as<std::vector<YAML::Node>>();
        Logging::info(LOG_TAG, "Going to process vertices", vertices_node,
          "...");
        if (vertices_node.size() >= 0) {
          for (const YAML::Node &vertex_node : vertices) {
            Logging::info(LOG_TAG, "Processing", vertex_node, "...");
            std::string name;
            if (vertex_node[NAME_KEY]) {
              name = vertex_node[NAME_KEY].as<std::string>();
            }

            auto uuid_str = vertex_node[UUID_KEY].as<std::string>();
            UUID uuid = UUID(uuid_str);
            std::unique_ptr<Task> task;
            if (vertex_node[TASK_KEY]) {
              make_task(vertex_node[TASK_KEY], task);
            } else {
              Logging::warn(LOG_TAG, "A Vertex without a Task was provided",
                "in", vertex_node);
            }
            DAGVertex next_vertex(name, std::move(task), std::move(uuid));
            dag->add_vertex(std::move(next_vertex));
          }
        }
      } else {
        auto error = std::string("\"Vertices\" must be a YAML Sequence.");
        throw_wrong_type(UpTo::VERTEX, error);
      }
    }

    void YAMLDagDeserializer::make_task(const YAML::Node &task_node,
      std::unique_ptr<Task> &task) const
    {
      std::string task_name;
      std::vector<std::unique_ptr<TaskStage>> stages;
      if (task_node[NAME_KEY]) {
        task_name = task_node[NAME_KEY].as<std::string>();
      }
      if (task_node[STAGES_KEY]) {
        Logging::info(LOG_TAG, "Going to process stages in", task_node, "...");
        // TODO (mehoggan): Write code to actually process stages.
      } else {
        Logging::warn(LOG_TAG, "Just created a task from", task_node[TASK_KEY],
          "with no stages");
      }
      task = std::make_unique<Task>(stages, task_name);
    }

    void YAMLDagDeserializer::throw_wrong_type(const UpTo &upto,
      const std::string &error) const
    {
      std::string up_to_example = YAMLDagDeserializer::sample_dag_output(upto);
      auto error_str = error + std::string(" As in:\n") + up_to_example;
      throw YAMLDagWrongTypeError(error_str);
    }
  }
}
