#ifndef TASK_H_INCLUDED
#define TASK_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/task_stage.h"
#include "dag_scheduler/uuid.h"

#include <atomic>
#include <functional>
#include <memory>
#include <sstream>
#include <ostream>
#include <vector>

namespace com
{
  namespace dag_scheduler
  {
    class DLLSPEC_DAGTASKS task
    {
    public:
      /**
       * @brief default ctor
       */
      task();

      /**
       * @brief ctor that assisgns stages to tasks.
       *
       * A constructor for a \ref task that assigns a set of
       * \ref task_stage (s) to a \ref task.
       *
       * @param[in] stages A collection of \ref task_stages to be run when
       *                   task is run. Its purpose is to make interrupting the
       *                   task easier at well defined user boundaries.
       */
      explicit task(std::vector<std::unique_ptr<task_stage>> &stages);

      /**
       * @brief ctor that gives a task a descriptive name and stages.
       *
       * A constructor for a \ref task that assigns a set of
       * \ref task_stage (s) to a \ref task and a descriptive label to the
       * \ref task.
       *
       * @param[in] stages A collection of \ref task_stage (s) to be run when
       *                   this's run member function is called.
       * @param[in] label A descriptive user defined label for (this).
       */
      task(std::vector<std::unique_ptr<task_stage>> &stages,
        const std::string &label);

      /**
       * @brief ctor that gives a task a descriptive name and stages.
       *
       * A constructor for a \ref task that assigns a set of
       * \ref task_stage (s) to a \ref task and a descriptive label to the
       * \ref task.
       *
       * @param[in] stages A collection of \ref task_stage (s) to be run when
       *                   this's run member function is called.
       * @param[in] label A descriptive user defined label for (this).
       * @param[in] complete_callback An optional function to call at the end
       * of a task.
       */
      task(std::vector<std::unique_ptr<task_stage>> &stages,
        const std::string &label,
        std::function<void (bool)> complete_callback);

      /**
       * @brief dtor
       */
      virtual ~task();

      task(const task &other) = delete;

      task &operator=(const task &other) = delete;

      /**
       * @brief move copy ctor.
       *
       * This may only be called if the \ref iterate_stages member function
       * of a \ref task has not been called and still in the process of
       * running. Doing so will result in termination of the graph and
       * the program.
       *
       * @param[out] other The \ref task to move into (this).
       */
      task(task &&other);

      /**
       * @brief move assignment operator.
       *
       * This may only be called if the run member function \ref task::run
       * has not been called and still in the process of running. Doing so
       * will result in termination of the graph.
       *
       * @param[out] other The \ref task to move into (this).
       *
       * @return A reference to (this).
       */
      task &operator=(task &&other);

      /**
       * @brief Getter for the user firendly label that identifies this task.
       *
       * Each \ref task should be easily identifed by users. This is done
       * by allowing users to assign a label to a task. This member function
       * returns that label.
       *
       * @return The label specified by the user.
       */
      const std::string &label() const;

      /**
       * @brief Getter for the uuid that identifies a \ref task.
       *
       * Each \ref task should be easily identifed. The \ref uuid owned by
       * a \ref task does exactly that, and this is how you get it.
       *
       * @return The \ref uuid owned by (this).
       */
      const uuid &get_uuid() const;

      /**
       * @brief The interface for iterating over the stages stored in a
       *        \ref task.
       *
       * A \ref task stores \ref task_stages so that it can be interrupted by
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
        const std::function<bool (task_stage &)> &next_stage);

      /**
       * @brief Function used to kill a \ref task and all its
       *        \ref task_stages.
       *
       * @return true if task was killed successfully false otherwise.
       */
      virtual bool kill();

      /**
       * @brief Function used by user of class of a \ref task to signal
       * it completion. The complete_callback will be called if set
       * by the user calling the correct constructor.
       *
       * @param[in] status The status to send to the call back specified by
       *                   the user..
       */
      virtual void complete(bool status);

    public:
      /**
       * @brief Equality operator.
       *
       * @param[in] lhs The left hand side of the equivalence operator.
       * @param[in] rhs The right hand side of the equivalence operator.
       *
       * @return true if \ref lhs has same uuid as \ref rhs.
       */
      friend bool operator==(const task &lhs, const task &rhs);

      /**
       * @brief Inequality operator
       *
       * @param[in] lhs The left hand side of the equivalence operator.
       * @param[in] rhs The right hand side of the equivalence operator.
       *
       * @return false if \ref lhs has same uuid as \ref rhs.
       */
      friend bool operator!=(const task &lhs, const task &rhs);

      /**
       * @brief A utility function to print a task to a stream.
       *
       * @param[out] out The std::ostream to write \p t to.
       * @param[in] t The \ref task to write to \p out.
       *
       * @return The std::ostream \p out after \p t was written to it.
       */
      friend std::ostream &operator<<(std::ostream &out, const task &t);

      /**
       * @brief A utility function to print a task to a stream.
       *
       * @param[out] out The std::stringstream to write \p t to.
       * @param[in] t The \ref task to write to \p out.
       *
       * @return The std::stringstream \p out after \p t was written to it.
       */
      friend std::stringstream &operator<<(std::stringstream &out,
        const task &t);

    protected:
      std::atomic_bool iterating_;
      std::atomic_bool kill_;
      std::vector<std::unique_ptr<task_stage>> stages_;
      std::string label_;
      std::function<void (bool)> complete_callback_;
      uuid uuid_;
    };
  }
}

#endif
