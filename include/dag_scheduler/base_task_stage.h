////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef INCLUDE_DAG_SCHEDULER_BASE_TASK_STAGE_H_
#define INCLUDE_DAG_SCHEDULER_BASE_TASK_STAGE_H_

#include <ostream>
#include <sstream>
#include <string>

#include "dag_scheduler/uuid.h"

namespace com {
namespace dag_scheduler {
class BaseTaskStage {
 public:
  /**
   * @brief default ctor
   */
  BaseTaskStage();

  /**
   * @brief A constructor for a BaseTaskStage that assigns a user defined
   *        label.
   *
   * The \p label is used only to describe what the task is for. It
   * does not impact what the task does or how it operates.
   *
   * @param[in] label The user defined label.
   */
  explicit BaseTaskStage(const std::string &label);

  /**
   * @brief dtor
   */
  virtual ~BaseTaskStage();

  BaseTaskStage(const BaseTaskStage &other) = delete;

  BaseTaskStage &operator=(const BaseTaskStage &other) = delete;

  /**
   * @brief move copy ctor.
   *
   * This may only be called if the \ref run member function
   * of a \ref task has not been called and still in the process of
   * running. Doing so will result in termination of the graph and
   * the program.
   *
   * @param[out] other The \ref BaseTaskStage to move int (this).
   */
  BaseTaskStage(BaseTaskStage &&other);

  /**
   * @brief move assignment operator.
   *
   * This may only be called if the \ref run member function
   * of a \ref task has not been called and still in the process of
   * running. Doing so will result in termination of the graph and
   * the program.
   *
   * @param[out] other The \ref BaseTaskStage to move into this.
   *
   * @return A reference to (this).
   */
  BaseTaskStage &operator=(BaseTaskStage &&other);

  /**
   * @brief Getter for the user friendly label that identifies a
   *        \ref BaseTaskStage.
   *
   * Each \ref BaseTaskStage should be easily identified by users. This is
   * done by allowing users to assign a label to a BaseTaskStage. This
   * member function returns that label.
   *
   * @return The label specified by the user.
   */
  const std::string &label() const;

  /**
   * @brief Getter for the uuid that identifies a \ref task.
   *
   * Each \ref task should be easily identified. The \ref uuid owned by
   * a \ref task does exactly that, and this is how you get it.
   *
   * @return The \ref uuid owned by (this).
   */
  const UUID &get_uuid() const;

 public:
  /**
   * @brief A virtual function used to run a \ref BaseTaskStage.
   *
   * A virtual function used to run what a derived \ref BaseTaskStage
   * is supposed to do.
   *
   * @return true if \ref BaseTaskStage was started successfully. false
   *         otherwise.
   */
  virtual bool run();

  /**
   * @brief A virtual function used to check if task_stag is running.
   *
   * It is up to the derived class to define what this means, but it is
   * recommended that it return only true if \ref run was called and
   * the stage is still actively running.
   *
   * @return true if \ref BaseTaskStage was run successfully, false
   *         otherwise.
   */
  virtual bool is_running() const;

  /**
   * @brief A virtual function used to end a \ref BaseTaskStage 's
   *        current path of execution. It is up to a running
   *        \ref BaseTaskStage to check this flag and ensure that it
   *        it ends what is running in a thread safe manner.
   *
   * @return true if \ref BaseTaskStage was ended successfully. false
   *         otherwise.
   */
  virtual bool end();

  /**
   * @brief A virtual function used to force users to cleanup any
   *        memory that was allocated during a call to
   *        \ref BaseTaskStage::run.
   */
  virtual void cleanup();

 public:
  /**
   * @brief Equality operator for a \ref BaseTaskStage.
   *
   * Two \ref BaseTaskStage (s) are considered equal if and only if their
   * \ref uuid (s) are the equal.
   *
   * @param[in] lhs The left hand side of the equivalence operator.
   * @param[in] rhs The right hand side of the equivalence operator.
   *
   * @return true if \p lhs has the same \ref uuid as \p rhs.
   */
  friend bool operator==(const BaseTaskStage &lhs, const BaseTaskStage &rhs);

  /**
   * @brief Inequality operator for a \ref BaseTaskStage.
   *
   * @param[in] lhs The left hand side of the not equal operator.
   * @param[in] rhs The right hand side of the not equal operator.
   *
   * @return false if \p lhs has the same \ref uuid as \p rhs.
   */
  friend bool operator!=(const BaseTaskStage &lhs, const BaseTaskStage &rhs);

  /**
   * @brief A utility function to print a \ref BaseTaskStage to a stream.
   *
   * A utility function to used to write the members of a
   * \ref BaseTaskStage to a stream.
   *
   * @param[out] out The std::ostream to write \p t to.
   * @param[in] t The \ref BaseTaskStage to write to \p out.
   *
   * @return The stream after \p t has been written to it.
   */
  friend std::ostream &operator<<(std::ostream &out, const BaseTaskStage &t);

  /**
   * @brief A utility function to print a \ref BaseTaskStage to a stream.
   *
   * A utility function to used to write the members of a
   * \ref BaseTaskStage to a stream.
   *
   * @param[out] out The std::ostream to write \p t to.
   * @param[in] t The \ref BaseTaskStage to write to \p out.
   *
   * @return The stream after \p t has been written to it.
   */
  friend std::stringstream &operator<<(std::stringstream &out,
                                       const BaseTaskStage &t);

 protected:
  std::string label_;
  UUID uuid_;
};
}  // namespace dag_scheduler
}  // namespace com

#endif  // INCLUDE_DAG_SCHEDULER_BASE_TASK_STAGE_H_
