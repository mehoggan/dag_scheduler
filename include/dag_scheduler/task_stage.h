#ifndef TASK_STAGE_H_INCLUDED
#define TASK_STAGE_H_INCLUDED

#include "dag_scheduler/uuid.h"

#include <memory>
#include <ostream>
#include <sstream>

namespace com {
  namespace dag_scheduler {
    class TaskStage {
    public:
      /**
       * @brief default ctor
       */
      TaskStage();

      /**
       * @brief A constructor for a TaskStage that assigns a user defined
       *        label.
       *
       * The \p label is used only to describe what the task is for. It
       * does not impact what the task does or how it operates.
       *
       * @param[in] label The user defined label.
       */
      explicit TaskStage(const std::string &label);

      /**
       * @brief dtor
       */
      virtual ~TaskStage();

      TaskStage(const TaskStage &other) = delete;

      TaskStage &operator=(const TaskStage &other) = delete;

      /**
       * @brief move copy ctor.
       *
       * This may only be called if the \ref run member function
       * of a \ref task has not been called and still in the process of
       * running. Doing so will result in termination of the graph and
       * the program.
       *
       * @param[out] other The \ref TaskStage to move int (this).
       */
      TaskStage(TaskStage &&other);

      /**
       * @brief move assignment operator.
       *
       * This may only be called if the \ref run member function
       * of a \ref task has not been called and still in the process of
       * running. Doing so will result in termination of the graph and
       * the program.
       *
       * @param[out] other The \ref TaskStage to move into this.
       *
       * @return A reference to (this).
       */
      TaskStage &operator=(TaskStage &&other);

      /**
       * @brief Getter for the user friendly label that identifies a
       *        \ref TaskStage.
       *
       * Each \ref TaskStage should be easily identifed by users. This is
       * done by allowing users to assign a label to a TaskStage. This member
       * function returns that label.
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
      const UUID &get_uuid() const;

    public:
      /**
       * @brief A pure virtual function used to run a \ref TaskStage.
       *
       * A pure virtual function used to run what a derived \ref TaskStage
       * is supposed to do.
       *
       * @return true if \ref TaskStage was started successfully. false
       *         otherwise.
       */
      virtual bool run() = 0;

      /**
       * @brief A pure virtual function used to check if task_stag is running.
       *
       * It is up to the derived class to define what this means, but it is
       * recomended that it return only true if \ref run was called and
       * the stage is still activly running.
       *
       * @return true if \ref TaskStage was run successfully, false
       *         otherwise.
       */
      virtual bool is_running() const = 0;

      /**
       * @brief A pure virtual function used to end a \ref TaskStage 's
       *        current path of execution. It is up to a running
       *        \ref TaskStage to check this flag and ensure that it
       *        it ends what is running in a thread safe maner.
       *
       * @return true if \ref TaskStage was ended successfully. flase
       *         otherwise.
       */
      virtual bool end() = 0;

      /**
       * @brief A pure virtual function used to force users to cleanup any
       *        memory that was alloctaed during a call to
       *        \ref TaskStage::run.
       */
      virtual void cleanup() = 0;

      /**
       * @brief A utility method for cloning a \ref TaskStage.
       *
       * A utility method for cloning a TaskStage.
       *
       * @return A cloned \ref std::unique_ptr<Stage> of \ref (*this).
       */
      virtual std::unique_ptr<TaskStage> clone() const = 0;

    public:
      /**
       * @brief Equality operator for a \ref TaskStage.
       *
       * Two \ref TaskStage (s) are considerede equal if and only if their
       * \ref uuid (s) are the equal.
       *
       * @param[in] lhs The left hand side of the equivalence operator.
       * @param[in] rhs The right hand side of the equivalence operator.
       *
       * @return true if \p lhs has the same \ref uuid as \p rhs.
       */
      friend bool operator==(const TaskStage &lhs, const TaskStage &rhs);

      /**
       * @brief Inequality operator for a \ref TaskStage.
       *
       * @param[in] lhs The left hand side of the inequivalence operator.
       * @param[in] rhs The right hand side of the inequivalence operator.
       *
       * @return false if \p lhs has the same \ref uuid as \p rhs.
       */
      friend bool operator!=(const TaskStage &lhs, const TaskStage &rhs);

      /**
       * @brief A utility function to print a \ref TaskStage to a stream.
       *
       * A utility function to used to write the members of a \ref TaskStage
       * to a stream.
       *
       * @param[out] out The std::ostream to write \p t to.
       * @param[in] t The \ref TaskStage to write to \p out.
       *
       * @return The stream after \p t has been written to it.
       */
      friend std::ostream &operator<<(std::ostream &out, const TaskStage &t);

      /**
       * @brief A utility function to print a \ref TaskStage to a stream.
       *
       * A utility function to used to write the members of a \ref TaskStage
       * to a stream.
       *
       * @param[out] out The std::ostream to write \p t to.
       * @param[in] t The \ref TaskStage to write to \p out.
       *
       * @return The stream after \p t has been written to it.
       */
      friend std::stringstream &
      operator<<(std::stringstream &out, const TaskStage &t);

    protected:
      TaskStage(const std::string &label, const UUID &uuid);

    protected:
      std::string label_;
      UUID uuid_;
    };
  } // namespace dag_scheduler
} // namespace com

#endif
