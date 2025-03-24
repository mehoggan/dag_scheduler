////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef DAG_SERIALIZATION_H_INCLUDED
#define DAG_SERIALIZATION_H_INCLUDED

#include <stdexcept>

#include <yaml-cpp/yaml.h>

#include <boost/dll.hpp>
#include <exception>

#include "dag_scheduler/Dag.h"
#include "dag_scheduler/DynamicLibraryRegistry.h"
#include "dag_scheduler/TaskCallbackPlugin.h"
#include "dag_scheduler/TaskStage.h"

namespace com::dag_scheduler {
/**
 * @brief A class used to deserialize YAML into a DAG.
 *
 * The definition in YAML for a DAG would be of the form:
 * DAG:
 *   Title: <optional string>
 *   Configuration: <optional YAML>
 *     <valid YAML>
 *   Vertices:
 *     - Vertex:
 *       Name: <optional string>
 *       UUID: <valid uuid4 string>
 *       Task:
 *         Name: <optional string>
 *         Callback: <optional>
 *           LibraryName: <string>
 *           SymbolName: <string>
 *           Type: <enum {Plugin, Function}>
 *         Stages:
 *           - Name: <optional string>
 *             LibraryName: <string>
 *             SymbolName: <string>
 *           ...
 *         TODO (matthew.hoggan)): Make sure these are tested.
 *         InitialInputs: <optional YAML>
 *           <valid YAML>
 *         Configuration: <optional YAML>
 *           <valid YAML>
 *       ...
 *     ...
 *     Connections:
 *       - Connection:
 *         From: <valid uuid4 string from vertex>
 *         To: <valid uuid4 string from vertex>
 *         ...
 */
class YAMLDagDeserializerError : public std::exception {
public:
    explicit YAMLDagDeserializerError(const std::string& what);

    const char* what() const noexcept override;

private:
    std::string what_;
};

class YAMLDagDeserializerNonSupportedCallbackType : public std::exception {
public:
    explicit YAMLDagDeserializerNonSupportedCallbackType(
            const std::string& what);

    const char* what() const noexcept override;

private:
    std::string what_;
};

class YAMLDagWrongTypeError : public YAMLDagDeserializerError {
public:
    explicit YAMLDagWrongTypeError(const std::string& what);
};

class YAMLDagDeserializer : public LoggedClass<YAMLDagDeserializer> {
public:
    static const char DAG_KEY[];
    static const char VERTICES_KEY[];
    static const char TASK_KEY[];
    static const char STAGES_KEY[];
    static const char CONFIGURATION_KEY[];
    static const char INITIAL_INPUTS_KEY[];

    static const char TITLE_KEY[];
    static const char NAME_KEY[];
    static const char UUID_KEY[];
    static const char CALLBACK_KEY[];
    static const char LIBRARY_NAME_KEY[];
    static const char SYMBOL_NAME_KEY[];
    static const char CALLBACK_TYPE_KEY[];

public:
    enum class UpTo {
        CONNECTIONS,
        EMPTY,
        DAG,
        VERTICES,
        VERTEX,
        TASK,
        STAGES,
        STAGE
    };

    enum class CallbackType { FUNCTION, PLUGIN, DNE };

public:
    /**
     * @brief ctor
     *
     * A default constructor fo a \ref YAMLDagDeserializer which does nothing.
     */
    YAMLDagDeserializer();

    /**
     * @brief A factory method that creates a \ref DAG.
     *
     * A factory method that creates a \ref DAG.
     *
     * @param[in] dag_node A \ref YAML::Node to start the deserialization
     * from.
     *
     * @return A \ref std::unique_ptr<DAG> that contains the deserialized \ref
     *         DAG.
     */
    std::unique_ptr<DAG> make_dag(const YAML::Node& dag_node) const;

private:
    void make_vertices(const YAML::Node& vertices_node,
                       std::unique_ptr<DAG>& dag) const;
    void make_task(const YAML::Node& task_node,
                   std::unique_ptr<Task>& task) const;
    void throw_wrong_type(const UpTo& upto, const std::string& error) const;
    void make_task_callback(
            const std::string& task_name,
            std::vector<std::unique_ptr<TaskStage>>& stages,
            const YAML::Node& callback_node,
            std::unique_ptr<Task>& task,
            const rapidjson::Document& json_config,
            const rapidjson::Document& json_initial_inputs) const;
    std::function<void(bool)> make_task_function_callback(
            const DynamicLibraryRegistry::RegistryItem& shared_library,
            const std::string& symbol_name) const;
    void make_task_function_callback_plugin(
            const DynamicLibraryRegistry::RegistryItem& shared_library,
            const std::string& symbol_name,
            std::unique_ptr<TaskCallbackPlugin>& ret) const;
    bool verify_symbol_present(
            const DynamicLibraryRegistry::RegistryItem& shared_library,
            const std::string& symbol_name,
            const std::string& section_name,
            std::string& cb_symbols) const;
    void load_stages(const YAML::Node& stages_node,
                     std::vector<std::unique_ptr<TaskStage>>& out_stages) const;
    std::unique_ptr<TaskStage> dynamically_load_stage(
            const DynamicLibraryRegistry::RegistryItem& shared_library,
            const std::string& symbol_name,
            const std::string& stage_name) const;

public:
    static std::string sample_dag_output(const UpTo& upto);
    static std::string full_sample_output();
    static CallbackType callback_type_from_string(const std::string& enum_str);

private:
    static void stage_str(std::string& ret);
    static void stages_str(std::string& ret);
    static void task_str(std::string& ret);
    static void vertex_str(std::string& ret);
    static void vertices_str(std::string& ret);
    static void connections_str(std::string& ret);
    static void dag_str(std::string& ret);
};
}  // namespace com::dag_scheduler

namespace YAML {
template <typename T = char (&)[]> struct convert {
    static bool decode(const Node& node, char (&rhs)[]) {
        (void)node;
        (void)rhs;
        throw std::logic_error("This has not been implemented.");
    }
};
}  // namespace YAML

namespace YAML {
template <> struct convert<std::unique_ptr<com::dag_scheduler::DAG>> {
    static bool decode(const Node& node,
                       std::unique_ptr<com::dag_scheduler::DAG>& rhs) {
        com::dag_scheduler::YAMLDagDeserializer deserializer;
        rhs = deserializer.make_dag(node);
        return true;
    }
};
}  // namespace YAML
#endif
