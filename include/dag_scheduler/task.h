#ifndef TASK_H_INCLUDED
#define TASK_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/uuid.h"

#include <memory>
#include <sstream>
#include <ostream>

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
      explicit task(const std::string &label);

      //! Virtual destructor to assit in deletion of derived classes.
      /*!
        Virtual destructor to assit in deletion of derived classes.
      */
      virtual ~task();

      //! Copy constructor for a \ref task
      /*!
        A copy constructor for a \ref task
      */
      task(const task &other);

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

    public:
      //! Pure virtual function used to run a \ref task.
      /*!
        Pure virutal function used to run what a derived \task is supposed
        to do.

        \return True if task was started successfully. False otherwise.
      */
      virtual bool run() = 0;

      //! Pure virtual function used to check if task is running.
      /*!
        This is up to the derived class to define what this means, but
        it is recomended that it return true only if \ref run was called.

        \return True if task was run successfully, false otherwise.
      */
      virtual bool is_running() const = 0;

      //! Pure virtual function used to run a \ref task.
      /*!
        Pure virutal function used to kill what a derived \task is supposed
        to do.

        \return True if task was killed successfully. False otherwise.
      */
      virtual bool kill() = 0;

      //! Pure virtual function used to force users to cleanup their tasks.
      /*!
        Because tasks can be interrupted and users wil have allocated
        resources a task should be responsible for cleaning up after
        itself.
      */
      virtual void cleanup() = 0;

      //! Pure virtual function that clones a derived class.
      /*!
        Pure virtual function of \ref task that makes an identical copy of
        this and returns it. However, it resets the connection which can only
        be made from an object that knows about the connection.

        \return A \ref task that is an identical copy of this.
      */
      virtual std::unique_ptr<task> clone() = 0;

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
      std::string label_;
      uuid uuid_;
    };
  }
}

#endif
