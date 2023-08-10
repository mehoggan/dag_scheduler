#ifndef TASK_H_INCLUDED
#define TASK_H_INCLUDED

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/task_stage.h"
#include "dag_scheduler/uuid.h"

#include <boost/config.hpp>

#include <atomic>
#include <functional>
#include <memory>
#include <sstream>
#include <ostream>
#include <vector>


#define API extern "C" BOOST_SYMBOL_EXPORT
API void default_task_callback(bool) noexcept;

namespace com
{
  namespace dag_scheduler
  {
    class TaskCallbackPlugin;

    class Task :
      public LoggedClass<Task>
    {
    public:
      /**
       * @brief default ctor
       */
      Task();

      /**
       * @brief ctor that assisgns stages to Tasks.
       *
       * A constructor for a \ref Task that assigns a set of
       * \ref TaskStage (s) to a \ref Task.
       *
       * @param[in] stages A collection of \ref TaskStages to be run when
       *                   Task is run. Its purpose is to make interrupting the
       *                   Task easier at well defined user boundaries.
       */
      explicit Task(std::vector<std::unique_ptr<TaskStage>> &stages);

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
      Task(std::vector<std::unique_ptr<TaskStage>> &stages,
        const std::string &label);

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
       */
      Task(std::vector<std::unique_ptr<TaskStage>> &stages,
        const std::string &label,
        std::function<void (bool)> complete_callback);

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
      Task(std::vector<std::unique_ptr<TaskStage>> &stages,
        const std::string &label,
        std::unique_ptr<TaskCallbackPlugin> &&complete_callback_plugin);

      /**
       * @brief dtor
       */
      virtual ~Task();

      Task(const Task &other) = delete;

      Task &operator=(const Task &other) = delete;

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
      Task(Task &&other);

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
      Task &operator=(Task &&other);

      /**
       * @brief Getter for the user firendly label that identifies this Task.
       *
       * Each \ref Task should be easily identifed by users. This is done
       * by allowing users to assign a label to a Task. This member function
       * returns that label.
       *
       * @return The label specified by the user.
       */
      const std::string &label() const;

      /**
       * @brief Getter for the uuid that identifies a \ref Task.
       *
       * Each \ref Task should be easily identifed. The \ref uuid owned by
       * a \ref Task does exactly that, and this is how you get it.
       *
       * @return The \ref uuid owned by (this).
       */
      const UUID &get_uuid() const;

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
      bool iterate_stages(
        const std::function<bool (TaskStage &)> &next_stage);

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
      virtual bool callback_is_set() const;

      /**
       * @brief Plugin used by user of class of a \ref Task to check if
       * a \ref TaskCallbackPlugin was set at construction time.
       *
       * @return true if \ref (*this) has a callback plugin set.
       */
      virtual bool callback_plugin_is_set() const;

    public:
      /**
       * @brief Equality operator.
       *
       * @param[in] lhs The left hand side of the equivalence operator.
       * @param[in] rhs The right hand side of the equivalence operator.
       *
       * @return true if \ref lhs has same uuid as \ref rhs.
       */
      friend bool operator==(const Task &lhs, const Task &rhs);

      /**
       * @brief Inequality operator
       *
       * @param[in] lhs The left hand side of the equivalence operator.
       * @param[in] rhs The right hand side of the equivalence operator.
       *
       * @return false if \ref lhs has same uuid as \ref rhs.
       */
      friend bool operator!=(const Task &lhs, const Task &rhs);

      /**
       * @brief A utility function to print a Task to a stream.
       *
       * @param[out] out The std::ostream to write \p t to.
       * @param[in] t The \ref Task to write to \p out.
       *
       * @return The std::ostream \p out after \p t was written to it.
       */
      friend std::ostream &operator<<(std::ostream &out, const Task &t);

      /**
       * @brief A utility function to print a Task to a stream.
       *
       * @param[out] out The std::stringstream to write \p t to.
       * @param[in] t The \ref Task to write to \p out.
       *
       * @return The std::stringstream \p out after \p t was written to it.
       */
      friend std::stringstream &operator<<(std::stringstream &out,
        const Task &t);

    protected:
      std::atomic_bool iterating_;
      std::atomic_bool kill_;
      std::vector<std::unique_ptr<TaskStage>> stages_;
      std::string label_;
      std::function<void (bool)> complete_callback_;
      std::unique_ptr<TaskCallbackPlugin> complete_callback_plugin_;
      UUID uuid_;
    };
  }
}

#endif
