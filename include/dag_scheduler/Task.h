////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef DAG_SCHEDULER_TASK_H
#define DAG_SCHEDULER_TASK_H

#include <rapidjson/document.h>

#include <atomic>
#include <boost/config.hpp>
#include <functional>
#include <memory>
#include <ostream>
#include <sstream>
#include <vector>

#include "dag_scheduler/LoggedClass.hpp"
#include "dag_scheduler/TaskStage.h"
#include "dag_scheduler/Uuid.h"

#define API extern "C" BOOST_SYMBOL_EXPORT
API void defaultTaskCallback(bool complete) noexcept;

namespace com::dag_scheduler {
class TaskCallbackPlugin;

class Task : public LoggedClass<Task> {
public:
    /**
     * @brief default ctor
     */
    Task();

    /**
     * @brief ctor that assigns stages to Tasks.
     *
     * A constructor for a \ref Task that assigns a set of
     * \ref TaskStage (s) to a \ref Task.
     *
     * @param[in] stages A collection of \ref TaskStages to be run when
     *                   Task is run. Its purpose is to make interrupting
     * the Task easier at well defined user boundaries.
     */
    explicit Task(std::vector<std::unique_ptr<TaskStage>>& stages);

    /**
     * @brief ctor that assigns stages to Tasks.
     *
     * A constructor for a \ref Task that assigns a set of
     * \ref TaskStage (s) to a \ref Task.
     *
     * @param[in] stages A collection of \ref TaskStages to be run when
     *                   Task is run. Its purpose is to make interrupting
     * the Task easier at well defined user boundaries.
     * @param[in] json_initial_inputs A json body that defines an initial set
     *                                of inputs to be provided to all \ref
     *                                TaskStage (s) owned by \ref (*this).
     */
    Task(std::vector<std::unique_ptr<TaskStage>>& stages,
         const rapidjson::Document& json_initial_inputs);

    /**
     * @brief ctor that gives a Task a descriptive name and stages.
     *
     * A constructor for a \ref Task that assigns a set of
     * \ref TaskStage (s) to a \ref Task and a descriptive label to the
     * \ref Task.
     *
     * @param[in] stages A collection of \ref TaskStage (s) to be run when
     *                   this's run member function is called.
     * @param[in] label A descriptive user defined label for (this).
     */
    Task(std::vector<std::unique_ptr<TaskStage>>& stages,
         const std::string& label);

    /**
     * @brief ctor that gives a Task a descriptive name and stages.
     *
     * A constructor for a \ref Task that assigns a set of
     * \ref TaskStage (s) to a \ref Task and a descriptive label to the
     * \ref Task.
     *
     * @param[in] stages A collection of \ref TaskStage (s) to be run when
     *                   this's run member function is called.
     * @param[in] label A descriptive user defined label for (this).
     * @param[in] json_initial_inputs A json body that defines an initial set
     *                                of inputs to be provided to all \ref
     *                                TaskStage (s) owned by \ref (*this).
     */
    Task(std::vector<std::unique_ptr<TaskStage>>& stages,
         const std::string& label,
         const rapidjson::Document& json_initial_inputs);

    /**
     * @brief ctor that gives a Task a descriptive name and stages.
     *
     * A constructor for a \ref Task that assigns a set of
     * \ref TaskStage (s) to a \ref Task and a descriptive label to the
     * \ref Task.
     *
     * @param[in] stages A collection of \ref TaskStage (s) to be run when
     *                   this's run member function is called.
     * @param[in] label A descriptive user defined label for (this).
     * @param[in] complete_callback An optional function to call at the end
     *                              of a Task.
     */
    Task(std::vector<std::unique_ptr<TaskStage>>& stages,
         const std::string& label,
         std::function<void(bool)> complete_callback);

    /**
     * @brief ctor that gives a Task a descriptive name and stages.
     *
     * A constructor for a \ref Task that assigns a set of
     * \ref TaskStage (s) to a \ref Task and a descriptive label to the
     * \ref Task.
     *
     * @param[in] stages A collection of \ref TaskStage (s) to be run when
     *                   this's run member function is called.
     * @param[in] label A descriptive user defined label for (this).
     * @param[in] complete_callback An optional function to call at the end
     *                              of a Task.
     * @param[in] json_initial_inputs A json body that defines an initial set
     *                                of inputs to be provided to all \ref
     *                                TaskStage (s) owned by \ref (*this).
     */
    Task(std::vector<std::unique_ptr<TaskStage>>& stages,
         const std::string& label,
         std::function<void(bool)> complete_callback,
         const rapidjson::Document& json_initial_inputs);

    /**
     * @brief ctor that gives a Task a descriptive name and stages.
     *
     * A constructor for a \ref Task that assigns a set of
     * \ref TaskStage (s) to a \ref Task and a descriptive label to the
     * \ref Task.
     *
     * @param[in] stages A collection of \ref TaskStage (s) to be run when
     *                   this's run member function is called.
     * @param[in] label A descriptive user defined label for (this).
     * @param[in] complete_callback An optional \ref TaskCallbackPlugin that
     *                              is called when as many as \ref TaskStage
     *                              are completed by the \ref Task.
     */
    Task(std::vector<std::unique_ptr<TaskStage>>& stages,
         const std::string& label,
         std::unique_ptr<TaskCallbackPlugin>&& complete_callback_plugin);

    /**
     * @brief ctor that gives a Task a descriptive name and stages.
     *
     * A constructor for a \ref Task that assigns a set of
     * \ref TaskStage (s) to a \ref Task and a descriptive label to the
     * \ref Task.
     *
     * @param[in] stages A collection of \ref TaskStage (s) to be run when
     *                   this's run member function is called.
     * @param[in] label A descriptive user defined label for (this).
     * @param[in] complete_callback An optional \ref TaskCallbackPlugin that
     *                              is called when as many as \ref TaskStage
     *                              are completed by the \ref Task.
     * @param[in] json_initial_inputs A json body that defines an initial set
     *                                of inputs to be provided to all \ref
     *                                TaskStage (s) owned by \ref (*this).
     */
    Task(std::vector<std::unique_ptr<TaskStage>>& stages,
         const std::string& label,
         std::unique_ptr<TaskCallbackPlugin>&& complete_callback_plugin,
         const rapidjson::Document& json_initial_inputs);

    /**
     * @brief ctor that assigns stages to Tasks.
     *
     * A constructor for a \ref Task that assigns a set of
     * \ref TaskStage (s) to a \ref Task.
     *
     * @param[in] stages A collection of \ref TaskStages to be run when
     *                   Task is run. Its purpose is to make interrupting
     * the Task easier at well defined user boundaries.
     * @param[in] json_config The json document to be used for
     * configuration.
     * @param[in] json_initial_inputs A json body that defines an initial set
     *                                of inputs to be provided to all \ref
     *                                TaskStage (s) owned by \ref (*this).
     */
    Task(std::vector<std::unique_ptr<TaskStage>>& stages,
         const rapidjson::Document& json_config,
         const rapidjson::Document& json_json_initial_inputs);

    /**
     * @brief ctor that gives a Task a descriptive name and stages.
     *
     * A constructor for a \ref Task that assigns a set of
     * \ref TaskStage (s) to a \ref Task and a descriptive label to the
     * \ref Task.
     *
     * @param[in] stages A collection of \ref TaskStage (s) to be run when
     *                   this's run member function is called.
     * @param[in] label A descriptive user defined label for (this).
     * @param[in] json_config The json document to be used for
     * configuration.
     * @param[in] json_initial_inputs A json body that defines an initial set
     *                                of inputs to be provided to all \ref
     *                                TaskStage (s) owned by \ref (*this).
     */
    Task(std::vector<std::unique_ptr<TaskStage>>& stages,
         const std::string& label,
         const rapidjson::Document& json_config,
         const rapidjson::Document& json_json_initial_inputs);

    /**
     * @brief ctor that gives a Task a descriptive name and stages.
     *
     * A constructor for a \ref Task that assigns a set of
     * \ref TaskStage (s) to a \ref Task and a descriptive label to the
     * \ref Task.
     *
     * @param[in] stages A collection of \ref TaskStage (s) to be run when
     *                   this's run member function is called.
     * @param[in] label A descriptive user defined label for (this).
     * @param[in] complete_callback An optional function to call at the end
     * of a Task.
     * @param[in] json_config The json document to be used for
     * configuration.
     * @param[in] json_initial_inputs A json body that defines an initial set
     *                                of inputs to be provided to all \ref
     *                                TaskStage (s) owned by \ref (*this).
     */
    Task(std::vector<std::unique_ptr<TaskStage>>& stages,
         const std::string& label,
         std::function<void(bool)> complete_callback,
         const rapidjson::Document& json_config,
         const rapidjson::Document& json_json_initial_inputs);

    /**
     * @brief ctor that gives a Task a descriptive name and stages.
     *
     * A constructor for a \ref Task that assigns a set of
     * \ref TaskStage (s) to a \ref Task and a descriptive label to the
     * \ref Task.
     *
     * @param[in] stages A collection of \ref TaskStage (s) to be run when
     *                   this's run member function is called.
     * @param[in] label A descriptive user defined label for (this).
     * @param[in] complete_callback An optional \ref TaskCallbackPlugin that
     *                              is called when as many as \ref TaskStage
     *                              are completed by the \ref Task.
     * @param[in] json_config The json document to be used for
     * configuration.
     * @param[in] json_initial_inputs A json body that defines an initial set
     *                                of inputs to be provided to all \ref
     *                                TaskStage (s) owned by \ref (*this).
     */
    Task(std::vector<std::unique_ptr<TaskStage>>& stages,
         const std::string& label,
         std::unique_ptr<TaskCallbackPlugin>&& complete_callback_plugin,
         const rapidjson::Document& json_config,
         const rapidjson::Document& json_json_initial_inputs);

    /**
     * @brief dtor
     */
    ~Task() override;

    Task(const Task& other) = delete;

    Task& operator=(const Task& other) = delete;

    /**
     * @brief move copy ctor.
     *
     * This may only be called if the \ref iterate_stages member function
     * of a \ref Task has not been called and still in the process of
     * running. Doing so will result in termination of the graph and
     * the program.
     *
     * @param[out] other The \ref Task to move into (this).
     */
    Task(Task&& other);

    /**
     * @brief move assignment operator.
     *
     * This may only be called if the run member function \ref Task::run
     * has not been called and still in the process of running. Doing so
     * will result in termination of the graph.
     *
     * @param[out] other The \ref Task to move into (this).
     *
     * @return A reference to (this).
     */
    Task& operator=(Task&& other);

    /**
     * @brief Getter for the user friendly label that identifies this Task.
     *
     * Each \ref Task should be easily identified by users. This is done
     * by allowing users to assign a label to a Task. This member function
     * returns that label.
     *
     * @return The label specified by the user.
     */
    const std::string& label() const;

    /**
     * @brief Getter for the uuid that identifies a \ref Task.
     *
     * Each \ref Task should be easily identified. The \ref uuid owned by
     * a \ref Task does exactly that, and this is how you get it.
     *
     * @return The \ref uuid owned by (this).
     */
    const UUID& getUUID() const;

    /**
     * @brief The interface for iterating over the stages stored in a
     *        \ref Task.
     *
     * A \ref Task stores \ref TaskStages so that it can be interrupted by
     * clients on well defined boundaries. Terminating threads comes with
     * overhead of cleaning up memory in TLS which to-date has no well
     * defined process defined in the C++ programming language. This is
     * a proposed solution for dealing with thread cleanup.
     *
     * @param[in] next_stage A callback for the next stage to run.
     *
     * @return
     */
    bool iterateStages(const std::function<bool(TaskStage&)>& next_stage);

    /**
     * @brief Function used to kill a \ref Task and all its
     *        \ref TaskStages.
     *
     * @return true if Task was killed successfully false otherwise.
     */
    virtual bool kill();

    /**
     * @brief Function used by user of class of a \ref Task to signal
     * it completion. The complete_callback will be called if set
     * by the user calling the correct constructor.
     *
     * @param[in] status The status to send to the call back specified by
     *                   the user..
     */
    virtual void complete(bool status);

    /**
     * @brief Function used by user of class of a \ref Task to check if
     * a callback was set at construction time.
     *
     * @return true if \ref (*this) has a callback set.
     */
    virtual bool callbackIsSet() const;

    /**
     * @brief Plugin used by user of class of a \ref Task to check if
     * a \ref TaskCallbackPlugin was set at construction time.
     *
     * @return true if \ref (*this) has a callback plugin set.
     */
    virtual bool callbackPluginIsSet() const;

    /**
     * @brief A getter for the json configuration passed into the \ref ctor.
     *
     * A member function of \ref Vertex that returns a const reference of
     * the \ref json_config passed into the \ref ctor.
     *
     * @return A const reference to the member \ref json_config_.
     */
    virtual const rapidjson::Document& jsonConfig() const;

    /**
     * @brief A helper method to visualize the configuration owned by \ref
     * this.
     *
     * A member function of \ref Vertex that assists in visualizing the
     * configuration document passed in at construction time.
     *
     * @param[out] out_str A \ref std::string that represents the \ref
     *                     rapidjson::Document owned by this.
     */
    virtual void jsonConfigStr(std::string& out_str) const;

    /**
     * @brief A helper method to visualize the initial inputs owned by \ref
     * this.
     *
     * A member function of \ref Vertex that assists in visualizing the
     * initial inputs document passed in at construction time.
     *
     * @param[out] out_str A \ref std::string that represents the \ref
     *                     rapidjson::Document owned by this.
     */
    virtual void jsonInitialInputsStr(std::string& out_str) const;

    /**
     * @brief A clone method to acquire a copy of the internal task.
     *
     * A clone method to acquire a copy of the internal task.
     *
     * @return A \ref std::unique_ptr<Task> that is a clone of the \ref
     * (*this).
     */
    virtual std::unique_ptr<Task> clone() const;

public:
    /**
     * @brief Equality operator.
     *
     * @param[in] lhs The left hand side of the equivalence operator.
     * @param[in] rhs The right hand side of the equivalence operator.
     *
     * @return true if \ref lhs has same uuid as \ref rhs.
     */
    friend bool operator==(const Task& lhs, const Task& rhs);

    /**
     * @brief Inequality operator
     *
     * @param[in] lhs The left hand side of the equivalence operator.
     * @param[in] rhs The right hand side of the equivalence operator.
     *
     * @return false if \ref lhs has same uuid as \ref rhs.
     */
    friend bool operator!=(const Task& lhs, const Task& rhs);

    /**
     * @brief A utility function to print a Task to a stream.
     *
     * @param[out] out The std::ostream to write \p t to.
     * @param[in] t The \ref Task to write to \p out.
     *
     * @return The std::ostream \p out after \p t was written to it.
     */
    friend std::ostream& operator<<(std::ostream& out, const Task& task);

    /**
     * @brief A utility function to print a Task to a stream.
     *
     * @param[out] out The std::stringstream to write \p t to.
     * @param[in] t The \ref Task to write to \p out.
     *
     * @return The std::stringstream \p out after \p t was written to it.
     */
    friend std::stringstream& operator<<(std::stringstream& out,
                                         const Task& task);

protected:
    void setJsonConfig(const rapidjson::Document& json_config);

    void setJsonInitialInputs(const rapidjson::Document& json_initial_inputs);

private:
    void updateUUID(const UUID& uuid);

protected:
    std::atomic_bool iterating_;
    std::atomic_bool kill_;
    std::vector<std::unique_ptr<TaskStage>> stages_;
    std::string label_;
    std::function<void(bool)> complete_callback_;
    std::unique_ptr<TaskCallbackPlugin> complete_callback_plugin_;
    UUID uuid_;
    std::unique_ptr<rapidjson::Document> json_config_;
    std::unique_ptr<rapidjson::Document> json_initial_inputs_;
};
}  // namespace com::dag_scheduler

#endif
