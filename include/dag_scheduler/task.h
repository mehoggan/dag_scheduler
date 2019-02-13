#ifndef TASK_H_INCLUDED
#define TASK_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/task_stage.h"
#include "dag_scheduler/uuid.h"

#include <atomic>
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
      //! Default constructor for a task.
      /*!
        A default constructor for a task.
      */
      task();

      //! A constructor for a task that assigns a user defined label.
      /*!
        A constructor for a task that assigns a user defined label to
        a \ref task.

        \param[in] label The user defined label.
      */
      explicit task(std::vector<std::unique_ptr<task_stage>> &stages);

      //! A constructor for a task that assigns a user defined label.
      /*!
        A constructor for a task that assigns a user defined label to
        a \ref task.

        \param[in] stages The stages to be run in this task.
        \param[in] label The user defined label.
      */
      task(std::vector<std::unique_ptr<task_stage>> &stages,
        const std::string &label);

      //! Virtual destructor to assit in deletion of derived classes.
      /*!
        Virtual destructor to assit in deletion of derived classes.
      */
      virtual ~task();

      //! Copy constructor for a \ref task
      /*!
        A copy constructor for a \ref task
      */
      task(const task &other) = delete;

      //! Assignment operator for a \ref task
      /*!
        A assignment operator for a \ref task
      */
      task &operator=(const task &other) = delete;

      //! Move copy constructor for a \ref task
      /*!
        A move copy constructor for a \ref task
      */
      task(task &&other);

      //! Move assignment operator for a \ref task
      /*!
        A move assignment operator for a \ref task
      */
      task &operator=(task &&other);

      //! Getter for the user firendly label that identifies this task.
      /*!
        Each \ref task should be easily identifed by users. This is done
        by allowing users to assign a label to a task. This member function
        returns that label.
      */
      const std::string &label() const;

      //! Getter for the uuid that identifies this task.
      /*!
        Each \ref task should be easily identifed. The \ref uuid owned by
        a \ref task does exactly that, and this is how you get it.
      */
      const uuid &get_uuid() const;

      //! Getter for stages
      /*!
        Every task has a set of stages so that it can be interrupted between
        stages. Having long running tasks can make it very difficult to
        kill them. It would lead to long running joins awaiting for threads to
        complete execution of task related code.

        \param[in] next_stage A callback for the next stage to run.
      */
      bool iterate_stages(
        const std::function<bool (task_stage &)> &next_stage);

      //! Function used to kill a \ref task and all its \ref task_stages.
      /*!
        Function used to kill what a derived \task is supposed
        to do.

        \return True if task was killed successfully. False otherwise.
      */
      virtual bool kill();

    public:
      //! Equality opertor for a task.
      /*!
        A function used to check if the \ref uuid of one task is the same
        as another.

        \param[in] lhs The left hand side of the equivalence operator.
        \param[in] rhs The right hand side of the equivalence operator.
        \return True if \ref lhs has same uuid and \ref rhs.
      */
      friend bool operator==(const task &lhs, const task &rhs);

      //! Inequality opertor for a task.
      /*!
        A function used to check if the \ref uuid of one task is not the same
        as another.

        \param[in] lhs The left hand side of the inequivalence operator.
        \param[in] rhs The right hand side of the inequivalence operator.
        \return True if \ref lhs has same uuid and \ref rhs.
      */
      friend bool operator!=(const task &lhs, const task &rhs);

      //! A utility function to print a task to a stream.
      /*!
        A utility function used to write a task to a stream.

        \param[in] out The \ref std::ostream to write \ref t to.
        \param[in] t The \ref task to write to \ref out.
        \return The \ref std::ostream \ref out.
      */
      friend std::ostream &operator<<(std::ostream &out, const task &t);

      //! A utility function to print a task to a stream.
      /*!
        A utility function used to write a task to a stream.

        \param[in] out The \ref std::stringstream to write \ref t to.
        \param[in] t The \ref task to write to \ref out.
        \return The \ref std::stringstream \ref out.
      */
      friend std::stringstream &operator<<(std::stringstream &out,
        const task &t);

    protected:
      std::atomic_bool iterating_;
      std::atomic_bool kill_;
      std::vector<std::unique_ptr<task_stage>> stages_;
      std::string label_;
      uuid uuid_;
    };
  }
}

#endif
