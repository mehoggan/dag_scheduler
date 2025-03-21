#ifndef PRINT_STAGE_H_INCLUDED
#define PRINT_STAGE_H_INCLUDED

#include <atomic>
#include <memory>
#include <ostream>
#include <sstream>

#include "dag_scheduler/task_stage.h"
#include "dag_scheduler/uuid.h"

namespace com {
namespace stages_lib {
class PrintStage : public com::dag_scheduler::TaskStage {
public:
    /**
     * @brief default ctor
     */
    PrintStage();

    /**
     * @brief A constructor for a PrintStage that assigns a user defined
     *        label.
     *
     * The \p label is used only to describe what the task is for. It
     * does not impact what the task does or how it operates.
     *
     * @param[in] label The user defined label.
     */
    explicit PrintStage(const std::string& label);

    /**
     * @brief A constructor for a PrintStage that assigns a user defined
     *        label and a UUID.
     *
     * The \p label is used only to describe what the task is for. It
     * does not impact what the task does or how it operates. \p uuid is
     * a user defined \ref UUID which was added primarily to facilitate
     * clone.
     *
     * @param[in] label The user defined label.
     * @param[in] uuid The user defined \ref UUID.
     */
    PrintStage(const std::string& label, const dag_scheduler::UUID& uuid);

    /**
     * @brief dtor
     */
    virtual ~PrintStage();

    PrintStage(const PrintStage& other) = delete;

    PrintStage& operator=(const PrintStage& other) = delete;

    /**
     * @brief move copy ctor.
     *
     * This may only be called if the \ref run member function
     * of a \ref task has not been called and still in the process of
     * running. Doing so will result in termination of the graph and
     * the program.
     *
     * @param[out] other The \ref PrintStage to move int (this).
     */
    PrintStage(PrintStage&& other);

    /**
     * @brief move assignment operator.
     *
     * This may only be called if the \ref run member function
     * of a \ref task has not been called and still in the process of
     * running. Doing so will result in termination of the graph and
     * the program.
     *
     * @param[out] other The \ref PrintStage to move into this.
     *
     * @return A reference to (this).
     */
    PrintStage& operator=(PrintStage&& other);

public:
    /**
     * @brief A function used to run a \ref PrintStage.
     *
     * A function used to run what a derived \ref PrintStage
     * is supposed to do.
     *
     * @return true if \ref PrintStage was started successfully. false
     *         otherwise.
     */
    virtual bool run();

    /**
     * @brief A function used to check if task_stag is running.
     *
     * It is up to the derived class to define what this means, but it is
     * recomended that it return only true if \ref run was called and
     * the stage is still activly running.
     *
     * @return true if \ref PrintStage was run successfully, false
     *         otherwise.
     */
    virtual bool is_running() const;

    /**
     * @brief A function used to end a \ref PrintStage 's
     *        current path of execution. It is up to a running
     *        \ref PrintStage to check this flag and ensure that it
     *        it ends what is running in a thread safe maner.
     *
     * @return true if \ref PrintStage was ended successfully. flase
     *         otherwise.
     */
    virtual bool end();

    /**
     * @brief A function used to force users to cleanup any
     *        memory that was alloctaed during a call to
     *        \ref PrintStage::run.
     */
    virtual void cleanup();

    /**
     * @brief A utility method for cloning a \ref TaskStage.
     *
     * A utility method for cloning a TaskStage.
     *
     * @return A cloned \ref std::unique_ptr<Stage> of \ref (*this).
     */
    virtual std::unique_ptr<TaskStage> clone() const;

public:
    /**
     * @brief Equality operator for a \ref PrintStage.
     *
     * Two \ref PrintStage (s) are considerede equal if and only if their
     * \ref uuid (s) are the equal.
     *
     * @param[in] lhs The left hand side of the equivalence operator.
     * @param[in] rhs The right hand side of the equivalence operator.
     *
     * @return true if \p lhs has the same \ref uuid as \p rhs.
     */
    friend bool operator==(const PrintStage& lhs, const PrintStage& rhs);

    /**
     * @brief Inequality operator for a \ref PrintStage.
     *
     * @param[in] lhs The left hand side of the inequivalence operator.
     * @param[in] rhs The right hand side of the inequivalence operator.
     *
     * @return false if \p lhs has the same \ref uuid as \p rhs.
     */
    friend bool operator!=(const PrintStage& lhs, const PrintStage& rhs);

    /**
     * @brief A utility function to print a \ref PrintStage to a stream.
     *
     * A utility function to used to write the members of a \ref PrintStage
     * to a stream.
     *
     * @param[out] out The std::ostream to write \p t to.
     * @param[in] t The \ref PrintStage to write to \p out.
     *
     * @return The stream after \p t has been written to it.
     */
    friend std::ostream& operator<<(std::ostream& out, const PrintStage& t);

    /**
     * @brief A utility function to print a \ref PrintStage to a stream.
     *
     * A utility function to used to write the members of a \ref PrintStage
     * to a stream.
     *
     * @param[out] out The std::ostream to write \p t to.
     * @param[in] t The \ref PrintStage to write to \p out.
     *
     * @return The stream after \p t has been written to it.
     */
    friend std::stringstream& operator<<(std::stringstream& out,
                                         const PrintStage& t);

public:
    /**
     * @brief A trait method needed by \ref TaskStage as a factory method.
     *
     * A trait method needed by \ref TaskStage as a factory method. When
     * a YAML file defining a \ref DAG is deserialized this method is called
     * to create the appropriate instance. It is used along with \ref
     * BOOST_DLL_ALIAS_SECTIONED to define an alias used to load the method
     * from the \ref Stages section of a dynamic library. See:
     *
     * <source root>/stages_lib/lib/print_stage.cxx
     *
     * for an example on how to use \ref BOOST_DLL_ALIAS_SECTIONED.
     *
     * @param[in] name The name to be assigned to the stage.
     *
     * @return A \ref std::unique_ptr<TaskStage> inserted into a \ref Task's
     *         collection of stages to be executed within a \ref DAGVertex.
     */
    static std::unique_ptr<com::dag_scheduler::TaskStage> make_stage(
            const std::string& name);

private:
    std::atomic_bool running_;
};
}  // namespace stages_lib
}  // namespace com
#endif
