#include "dag_scheduler/dag_serialization.h"

#include <boost/dll/import.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/dll/shared_library_load_mode.hpp>
#include <boost/dll.hpp>
#include <boost/function.hpp>
#include <boost/system/system_error.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>

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
    const std::string YAMLDagDeserializer::CALLBACK_KEY = "Callback";
    const std::string YAMLDagDeserializer::LIBRARY_NAME_KEY = "LibraryName";
    const std::string YAMLDagDeserializer::SYMBOL_NAME_KEY = "SymbolName";
    const std::string YAMLDagDeserializer::CALLBACK_TYPE_KEY = "Type";

    YAMLDagDeserializerError::YAMLDagDeserializerError(
      const std::string &what) :
      std::exception(),
      what_(what)
    {}

    const char *YAMLDagDeserializerError::what() const noexcept
    {
      return what_.c_str();
    }

    YAMLDagDeserializerNonSupportedCallbackType::
      YAMLDagDeserializerNonSupportedCallbackType(
        const std::string &what) :
        std::exception(),
        what_(what)
    {}

    const char
    *YAMLDagDeserializerNonSupportedCallbackType::what() const noexcept
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
          std::string("        Callback: <optional>\n") +
          std::string("            LibraryName: <string>\n") +
          std::string("            SymbolName: <string>\n") +
          std::string("            Type: <enum {Plugin, Function}>\n") +
          std::string("        ...");
      } else {
        ret = std::string("      Task:\n") +
          std::string("        Name: <optional string>\n") +
          std::string("        Callback: <optional>\n") +
          std::string("            LibraryName: <string>\n") +
          std::string("            SymbolName: <string>\n") +
          std::string("            Type: <enum {Plugin, Function}>\n") +
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

    YAMLDagDeserializer::CallbackType
    YAMLDagDeserializer::callback_type_from_string(const std::string &enum_str)
    {
      std::string to_lower = enum_str;
      std::transform(to_lower.begin(), to_lower.end(), to_lower.begin(),
        [](unsigned char c) -> char
        {
          return static_cast<char>(std::tolower(static_cast<int>(c)));
        }
      );
      YAMLDagDeserializer::CallbackType ret = CallbackType::DNE;
      if (to_lower == "function") {
        ret = CallbackType::FUNCTION;
      } else if (to_lower == "plugin") {
        ret = CallbackType::PLUGIN;
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
      if (task_node[NAME_KEY]) {
        task_name = task_node[NAME_KEY].as<std::string>();
      }

      std::vector<std::unique_ptr<TaskStage>> stages;
      if (task_node[STAGES_KEY]) {
        Logging::info(LOG_TAG, "Going to process stages in",
          task_node[STAGES_KEY], "...");
        load_stages(task_node[STAGES_KEY], stages);
      } else {
        Logging::warn(LOG_TAG, "Just created a task from", task_node[TASK_KEY],
          "with no stages");
      }

      if (task_node[CALLBACK_KEY]) {
        make_task_callback(task_name, stages, task_node[CALLBACK_KEY], task);
      } else {
        task = std::make_unique<Task>(stages, task_name);
      }
    }

    void YAMLDagDeserializer::throw_wrong_type(const UpTo &upto,
      const std::string &error) const
    {
      std::string up_to_example = YAMLDagDeserializer::sample_dag_output(upto);
      auto error_str = error + std::string(" As in:\n") + up_to_example;
      throw YAMLDagWrongTypeError(error_str);
    }

    std::stringstream &operator<<(std::stringstream &ss,
      std::vector<std::string> sym)
    {
      std::copy(sym.begin(), sym.end(),
        std::ostream_iterator<std::string>(ss, ","));
      return ss;
    }

    void YAMLDagDeserializer::make_task_callback(
      const std::string &task_name,
      std::vector<std::unique_ptr<TaskStage>> &stages,
      const YAML::Node &callback_node,
      std::unique_ptr<Task> &task) const
    {
      std::function<void (bool)> task_callback;
      Logging::info(LOG_TAG, "Going to create task callback from",
        callback_node, "...");
      auto library_name = callback_node[LIBRARY_NAME_KEY].as<std::string>();
      boost::dll::shared_library dynamic_library;
       
      try { 
        dynamic_library = boost::dll::shared_library(library_name);
      } catch (boost::system::system_error &system_error) {
        std::stringstream error_builder;
        error_builder << "Could not load library from ";
        error_builder << library_name << " with " << system_error.what();
        Logging::error(LOG_TAG, error_builder.str());
        throw YAMLDagDeserializerError(error_builder.str());
      }

      if (callback_node[CALLBACK_TYPE_KEY]) {
        CallbackType type = YAMLDagDeserializer::callback_type_from_string(
         callback_node[CALLBACK_TYPE_KEY].as<std::string>());
        if (type == CallbackType::FUNCTION) {
          task_callback = make_task_function_callback(
            dynamic_library,
            library_name,
            callback_node[SYMBOL_NAME_KEY].as<std::string>());
          task = std::make_unique<Task>(stages, task_name, task_callback);
        } else if (type == CallbackType::PLUGIN) {
          std::unique_ptr<TaskCallbackPlugin> task_callback_plugin;
          make_task_function_callback_plugin(
            dynamic_library,
            library_name,
            callback_node[SYMBOL_NAME_KEY].as<std::string>(),
            task_callback_plugin);
          task = std::make_unique<Task>(
            stages, task_name, std::move(task_callback_plugin));
        } else {
          throw YAMLDagDeserializerNonSupportedCallbackType(
            std::string("Unspported callback type specified. Currently ") +
            std::string("only \"FUNCTION\" or \"PLUGIN\" are accepted. ") +
            std::string("Note these values are case insensitive."));
        }
      } else {
        std::stringstream error_builder;
        error_builder << std::string("If a callback is to be used in:\n");
        error_builder << callback_node[CALLBACK_KEY];
        error_builder << std::string(" it must contain a Callback Type.");
        Logging::error(LOG_TAG, error_builder.str());
        throw YAMLDagDeserializerError(error_builder.str());
      }
    }

    std::function<void (bool)>
    YAMLDagDeserializer::make_task_function_callback(
      const boost::dll::shared_library &library,
      const std::string &library_name,
      const std::string &symbol_name) const
    {
      std::string cb_symbols;
      std::function<void (bool)> ret;
      bool symbol_present = verify_symbol_present(
        library, library_name, symbol_name, cb_symbols);
      if (symbol_present) {
        Logging::info(LOG_TAG, "Found", symbol_name, "in", library_name);
        ret = library.get_alias<void (bool)>(symbol_name);
      } else {
        throw YAMLDagDeserializerError("Failed to load " + symbol_name +
          " from " + library_name + ". It could not be found in available " +
          " symbols of " + cb_symbols);
      }
      return ret;
    }
 
    void YAMLDagDeserializer::make_task_function_callback_plugin(
      const boost::dll::shared_library &library,
      const std::string &library_name,
      const std::string &symbol_name,
      std::unique_ptr<TaskCallbackPlugin> &ret) const
    {
      std::string cb_symbols;
      bool symbol_present = verify_symbol_present(
        library, library_name, symbol_name, cb_symbols);
      if (symbol_present) {
        Logging::info(LOG_TAG, "Found", symbol_name, "in", library_name);
        boost::shared_ptr<TaskCallbackPlugin> callback_plugin =
          boost::dll::import_alias<TaskCallbackPlugin>(
            library_name,
            symbol_name,
            boost::dll::load_mode::append_decorations);
        ret = std::make_unique<TaskCallbackPlugin>(*(callback_plugin.get()));
      } else {
        throw YAMLDagDeserializerError("Failed to load " + symbol_name +
          " from " + library_name + ". It could not be found in available " +
          " symbols of " + cb_symbols);
      }
    }

    bool YAMLDagDeserializer::verify_symbol_present(
      const boost::dll::shared_library &library,
      const std::string &library_name,
      const std::string &symbol_name,
      std::string &cb_symbols) const
    {
      Logging::info(LOG_TAG, "Going to load", symbol_name, "from",
        library_name, "and library load status is",
        (library.is_loaded() ? "true" : "false"));

      std::vector<std::string> task_cb_funcs;
      try {
        boost::dll::library_info inf(library_name);
        task_cb_funcs = inf.symbols("TaskCb");
      } catch (const std::runtime_error &rt_error) {
        throw YAMLDagDeserializerError("Could not load symbol info from " +
          library_name + " with " + rt_error.what());
      } catch (const std::exception &error) {
        throw YAMLDagDeserializerError("Could not load symbol info from " +
          library_name + " with " + error.what());
      }

      std::vector<std::string>::iterator find_if_it = std::find_if(
        task_cb_funcs.begin(), task_cb_funcs.end(),
        [&symbol_name](const std::string &exported)
        {
          return exported == symbol_name;
        });

      std::stringstream cb_symbols_ss;
      cb_symbols_ss << task_cb_funcs;
      cb_symbols = cb_symbols_ss.str();
      Logging::info(LOG_TAG, "Going to load", symbol_name, "from",
        library_name, "which contains the following symbols",
        cb_symbols, "and library load status is",
        (library.is_loaded() ? "true" : "false"));

      return find_if_it != task_cb_funcs.end();
    }

    void YAMLDagDeserializer::load_stages(const YAML::Node &stages_node,
      std::vector<std::unique_ptr<TaskStage>> &out_stages) const
    {
      auto stages_vector = stages_node.as<std::vector<YAML::Node>>();
      std::copy(stages_vector.begin(), stages_vector.end(),
        std::ostream_iterator<YAML::Node>(std::cout, ", "));

      std::for_each(stages_vector.begin(), stages_vector.end(),
        [&out_stages](const YAML::Node &)
        {
          // auto next_stage = std::make_unique<TestTa>();
          out_stages.push_back(nullptr);
        });
    }
  }
}
