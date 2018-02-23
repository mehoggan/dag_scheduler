#ifndef UUID_H_INCLUDED
#define UUID_H_INCLUDED

#include "declspec.h"

#include <uuid/uuid.h>

#include <ostream>
#include <string>

namespace com
{
  namespace dag_scheduler
  {
    //! A class that represents a unique identifier.
    /*!
      A class that represents a directed acyclic graph (dag) and operations
      that can be performed on it. You can add, find, check containment a
      \ref dag_vertex. Additionally you can connect two \ref dag_vertex.
      Every connection checks to preserve that acyclic portion of the graph.
      If any connection would make the graph acyclic then an exception is
      thrown and the connection is not made.
    */
    class DLLSPEC_DAGTASKS uuid
    {
    public:
      //! A constructor for a \ref uuid.
      /*!
        A constructor for a \ref uuid.
      */
      uuid();

      //! A destructor for a \ref uuid.
      /*!
        A destructor for a \ref uuid.
      */
      ~uuid();

      //! The move copy constructor.
      /*!
        The move copy constructor moves ownership of the uuid data
        structure from this to \ref other.

        \param[out] other The \ref uuid to move ownership to.
      */
      uuid(uuid &&other);

      //! Move assignment operator.
      /*!
        The move assignment operator moves ownership of the uuid data
        structure from this to \ref other.
      */
      uuid &operator=(uuid &&rhs);

      //! Clones this object into an identical uuid
      /*!
        A member function of \ref uuid that makes an identical copy of this
        and returns it.

        \return A \ref uuid that is an identical copy of this.
      */
      uuid clone();

      //! In the case a move of a \ref uuid this function will return false.
      /*!
        A function to be used to check if \ref this was moved to another
        \ref uuid.

        \return True if \ref this was moved, false otherwise.
      */
      bool is_initialized() const;

      //! Get a string representation of the \ref uuid.
      /*!
        A member fucntion of \ref uuid that returns a string representation.

        \return A string representation of the \ref uuid.
      */
      std::string as_string() const;

      //! A utility function for streaming out a uuid.
      /*!
          A utility function for streaming out a uuid.

          \param[in] out The stream to write \ref u to.
          \param[in] u The \ref uuid to write out to \ref out.
          \return The \ref out param written to.
      */
      friend std::ostream &operator<<(std::ostream &out, const uuid &u)
      {
        out << u.as_string();
        return out;
      }

      //! A utility function for checking equality of two \ref uuid s.
      /*!
          A utility function for checking equality of two \ref uuid.

          \param[in] lhs The \ref uuid to the left hand side of the == op.
          \param[in] rhs The \ref uuid to the right hand side of the == op.
          \return True if \ref lhs and \ref rhs are not initialized, or
                  if the string representation of \ref rhs equals that of
                  \ref rhs. False otherwise.
      */
      friend bool operator==(const uuid &lhs, const uuid &rhs)
      {
        bool ret = true;

        if (!lhs.is_initialized() && !rhs.is_initialized()) {
          ret = true;
        } else if (!lhs.is_initialized() || !rhs.is_initialized()) {
          ret = false;
        } else {
          // Returns 0 on true.
          ret = (lhs.as_string() == rhs.as_string());
        }

        return ret;
      }

      //! A utility function for checking inequality of two \ref uuid s.
      /*!
          A utility function for checking inequality of two \ref uuid.

          \param[in] lhs The \ref uuid to the left hand side of the == op.
          \param[in] rhs The \ref uuid to the right hand side of the == op.
          \return False if \ref lhs and \ref rhs are not initialized, or
                  if the string representation of \ref rhs equals that of
                  \ref rhs. True otherwise.
      */
      friend bool operator!=(const uuid &lhs, const uuid &rhs)
      {
        return !(lhs == rhs);
      }

    private:
      uuid_t uuid_;

      uuid(const uuid &other);
      uuid &operator=(const uuid &rhs);
    };
  }
}
#endif
