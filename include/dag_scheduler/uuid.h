#ifndef UUID_H_INCLUDED
#define UUID_H_INCLUDED

#include "declspec.h"

#include <uuid/uuid.h>

#include <ostream>
#include <string>
#include <sstream>

namespace com
{
  namespace dag_scheduler
  {
    /**
     * @brief A class that represents a unique identifier.
     *
     * A class used by other classes to uniquly identify them. This class
     * does support cloning for the case where the client will clone
     * an item like a \ref dag or a \ref dag_vertex for the purposes of
     * mutating it while not impacting the original.
     */
    class DLLSPEC_DAGTASKS UUID
    {
    public:
      /**
       * @brief ctor
       */
      UUID();

      /**
       * @brief dtor
       */
      ~UUID();

      /**
       * @brief move copy ctor
       *
       * @param[out] other The \ref UUID to move into (this).
       */
      UUID(UUID &&other);

      /**
       * @brief move assignment operator.
       *
       * @param[out] rhs The \ref UUID to move into (this).
       *
       * @return A reference to (this) once \p rhs has been moved into it.
       */
      UUID &operator=(UUID &&rhs);

      /**
       * @brief Clones (this) into an identical \ref UUID.
       *
       * At times the client might want to mutate an object that owns
       * a \ref UUID without mutating the original. This member function
       * does a deep copy on the underlying data that represents a \ref UUID.
       *
       * @return A \ref UUID that is an identical copy of this.
       */
      UUID clone();

      /**
       * @brief Used to ensure a UUID is in a valid state.
       *
       * If a move operation is performed on a \ref UUID the underlying data
       * is put into an invalid state for the \ref UUID that was moved into
       * another \ref UUID.
       *
       * @return true if (this) was maved false otherwise.
       */
      bool is_initialized() const;

      /**
       * @brief Get a string representation of a \ref UUID.
       *
       * @return A string representation of the \ref UUID.
       */
      std::string as_string() const;

      /**
       * @brief A utility function used to write a \ref UUID to a stream.
       *
       * @param[out] out The stream to write \p u to.
       * @param[in] u The \ref UUID to write to \p out.
       *
       * @return The stream \p out after \p u was written to it.
       */
      friend std::ostream &operator<<(std::ostream &out, const UUID &u)
      {
        out << u.as_string();
        return out;
      }

      /**
       * @brief A utility function used to write a \ref UUID to a stream.
       *
       * @param[out] out The stream to write \p u to.
       * @param[in] u The \ref UUID to write to \p out.
       *
       * @return The stream \p out after \p u was written to it.
       */
      friend std::stringstream &operator<<(std::stringstream &out,
        const UUID &u)
      {
        out << u.as_string();
        return out;
      }

      /**
       * @brief Equality operator.
       *
       * @param[in] lhs The \ref UUID to the left hand side of the ==.
       * @param[in] rhs The \ref UUID to the right hand side of the ==.
       *
       * @return true if \p lhs and \p rhs are not initialized, or
       *         if the string representation of \p rhs equals that of
       *         \p rhs, false otherwise. 
       */
      friend bool operator==(const UUID &lhs, const UUID &rhs)
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

      /**
       * @brief Inequality operator.
       *
       * @param[in] lhs The \ref UUID to the left hand side of the !=.
       * @param[in] rhs The \ref UUID to the right hand side of the !=.
       *
       * @return false if \p lhs and \p rhs are not initialized, or
       *         if the string representation of \p rhs equals that of
       *         \p rhs, true otherwise. 
       */
      friend bool operator!=(const UUID &lhs, const UUID &rhs)
      {
        return !(lhs == rhs);
      }

    private:
      uuid_t uuid_;

      UUID(const UUID &other);
      UUID &operator=(const UUID &rhs);
    };
  }
}
#endif
