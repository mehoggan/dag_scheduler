#ifndef TASK_STAGE_H_INCLUDED
#define TASK_STAGE_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/uuid.h"

#include <memory>
#include <sstream>
#include <ostream>

namespace com
{
  namespace dag_scheduler
  {
    class DLLSPEC_DAGTASKS task_stage
    {
    public:
      //! Default constructor for a task_stage.
      /*!
        A default constructor for a task_stage.
      */
      task_stage();

      //! A constructor for a task_stage that assigns a user defined label.
      /*!
        A constructor for a task_stage that assigns a user defined label to
        a \ref task_stage.

        \param[in] label The user defined label.
      */
      explicit task_stage(const std::string &label);

      //! Virtual destructor to assit in deletion of derived classes.
      /*!
        Virtual destructor to assit in deletion of derived classes.
      */
      virtual ~task_stage();

      //! Copy constructor for a \ref task_stage
      /*!
        A copy constructor for a \ref task_stage
      */
      task_stage(const task_stage &other) = delete;

      //! Assignment operator for a \ref task_stage
      /*!
        A assignment operator for a \ref task_stage
      */
      task_stage &operator=(const task_stage &other) = delete;

      //! Move copy constructor for a \ref task_stage
      /*!
        A move copy constructor for a \ref task_stage
      */
      task_stage(task_stage &&other);

      //! Move assignment operator for a \ref task_stage
      /*!
        A move assignment operator for a \ref task_stage
      */
      task_stage &operator=(task_stage &&other);

      //! Getter for the user firendly label that identifies this task_stage.
      /*!
        Each \ref task_stage should be easily identifed by users. This is done
        by allowing users to assign a label to a task_stage. This member
        function returns that label.
      */
      const std::string &label() const;

      //! Getter for the uuid that identifies this task_stage.
      /*!
        Each \ref task_stage should be easily identifed. The \ref uuid owned
        by a \ref task_stage does exactly that, and this is how you get it.
      */
      const uuid &get_uuid() const;

    public:
      //! Pure virtual function used to run a \ref task_stage.
      /*!
        Pure virutal function used to run what a derived \task_stage is
        supposed to do.

        \return True if task_stage was started successfully. False otherwise.
      */
      virtual bool run() = 0;

      //! Pure virtual function used to check if task_stage is running.
      /*!
        This is up to the derived class to define what this means, but
        it is recomended that it return true only if \ref run was called.

        \return True if task_stage was run successfully, false otherwise.
      */
      virtual bool is_running() const = 0;

      //! Pure virtual function used to end a \ref task_stage.
      /*!
        Pure virutal function used to end what a derived \task_stage is
        supposed to do.

        \return True if task_stage was ended successfully. False otherwise.
      */
      virtual bool end() = 0;

      //! Pure virtual function used to force users to cleanup their
      //! task_stages.
      /*!
        Because task_stages can be interrupted and users wil have allocated
        resources a task_stage should be responsible for cleaning up after
        itself.
      */
      virtual void cleanup() = 0;

    public:
      //! Equality opertor for a task_stage.
      /*!
        A function used to check if the \ref uuid of one task_stage is the
        same as another.

        \param[in] lhs The left hand side of the equivalence operator.
        \param[in] rhs The right hand side of the equivalence operator.
        \return True if \ref lhs has same uuid and \ref rhs.
      */
      friend bool operator==(const task_stage &lhs, const task_stage &rhs);

      //! Inequality opertor for a task_stage.
      /*!
        A function used to check if the \ref uuid of one task_stage is not the
        same as another.

        \param[in] lhs The left hand side of the inequivalence operator.
        \param[in] rhs The right hand side of the inequivalence operator.
        \return True if \ref lhs has same uuid and \ref rhs.
      */
      friend bool operator!=(const task_stage &lhs, const task_stage &rhs);

      //! A utility function to print a task_stage to a stream.
      /*!
        A utility function used to write a task_stage to a stream.

        \param[in] out The \ref std::ostream to write \ref t to.
        \param[in] t The \ref task_stage to write to \ref out.
        \return The \ref std::ostream \ref out.
      */
      friend std::ostream &operator<<(std::ostream &out, const task_stage &t);

      //! A utility function to print a task_stage to a stream.
      /*!
        A utility function used to write a task_stage to a stream.

        \param[in] out The \ref std::stringstream to write \ref t to.
        \param[in] t The \ref task_stage to write to \ref out.
        \return The \ref std::stringstream \ref out.
      */
      friend std::stringstream &operator<<(std::stringstream &out,
        const task_stage &t);

    protected:
      std::string label_;
      uuid uuid_;
    };
  }
}

#endif
