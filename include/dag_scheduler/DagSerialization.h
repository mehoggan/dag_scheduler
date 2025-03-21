#ifndef DAG_SERIALIZATION_H_INCLUDED
#define DAG_SERIALIZATION_H_INCLUDED

#include <yaml-cpp/yaml.h>

#include <boost/dll.hpp>
#include <exception>

#include "dag_scheduler/dag.h"
#include "dag_scheduler/dynamic_library_registry.h"
#include "dag_scheduler/task_callback_plugin.h"
#include "dag_scheduler/task_stage.h"

namespace com {
namespace dag_scheduler {
/**
 * @brief A class used to deserialize YAML into a DAG.
 *
 * The definition in YAML for a DAG would be of the form:
 * DAG:
 *   Title: <optional string>
 *   Configuation: <optional YAML>
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
 *         TODO (mehoggan): Make sure these are tested.
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
    const static std::string DAG_KEY;
    const static std::string VERTICES_KEY;
    const static std::string TASK_KEY;
    const static std::string STAGES_KEY;
    const static std::string CONFIGURATION_KEY;
    const static std::string INITIAL_INPUTS_KEY;

    const static std::string TITLE_KEY;
    const static std::string NAME_KEY;
    const static std::string UUID_KEY;
    const static std::string CALLBACK_KEY;
    const static std::string LIBRARY_NAME_KEY;
    const static std::string SYMBOL_NAME_KEY;
    const static std::string CALLBACK_TYPE_KEY;

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
    static void verticies_str(std::string& ret);
    static void connections_str(std::string& ret);
    static void dag_str(std::string& ret);
};
}  // namespace dag_scheduler
}  // namespace com

namespace YAML {
template <> struct convert<std::unique_ptr<com::dag_scheduler::DAG>> {
    static bool decode(const Node& node,
                       std::unique_ptr<com::dag_scheduler::DAG>& rhs) {
        (void)node, (void)rhs;
        com::dag_scheduler::YAMLDagDeserializer deserializer;
        rhs = deserializer.make_dag(node);
        return true;
    }
};
}  // namespace YAML
#endif
