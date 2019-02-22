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
    class DLLSPEC_DAGTASKS uuid
    {
    public:
      /**
       * @brief ctor
       */
      uuid();

      /**
       * @brief dtor
       */
      ~uuid();

      /**
       * @brief move copy ctor
       *
       * @param[out] other The \ref uuid to move into (this).
       */
      uuid(uuid &&other);

      /**
       * @brief move assignment operator.
       *
       * @param[out] rhs The \ref uuid to move into (this).
       *
       * @return A reference to (this) once \p rhs has been moved into it.
       */
      uuid &operator=(uuid &&rhs);

      /**
       * @brief Clones (this) into an identical \ref uuid.
       *
       * At times the client might want to mutate an object that owns
       * a \ref uuid without mutating the original. This member function
       * does a deep copy on the underlying data that represents a \ref uuid.
       *
       * @return A \ref uuid that is an identical copy of this.
       */
      uuid clone();

      /**
       * @brief Used to ensure a uuid is in a valid state.
       *
       * If a move operation is performed on a \ref uuid the underlying data
       * is put into an invalid state for the \ref uuid that was moved into
       * another \ref uuid.
       *
       * @return true if (this) was maved false otherwise.
       */
      bool is_initialized() const;

      /**
       * @brief Get a string representation of a \ref uuid.
       *
       * @return A string representation of the \ref uuid.
       */
      std::string as_string() const;

      /**
       * @brief A utility function used to write a \ref uuid to a stream.
       *
       * @param[out] out The stream to write \p u to.
       * @param[in] u The \ref uuid to write to \p out.
       *
       * @return The stream \p out after \p u was written to it.
       */
      friend std::ostream &operator<<(std::ostream &out, const uuid &u)
      {
        out << u.as_string();
        return out;
      }

      /**
       * @brief A utility function used to write a \ref uuid to a stream.
       *
       * @param[out] out The stream to write \p u to.
       * @param[in] u The \ref uuid to write to \p out.
       *
       * @return The stream \p out after \p u was written to it.
       */
      friend std::stringstream &operator<<(std::stringstream &out,
        const uuid &u)
      {
        out << u.as_string();
        return out;
      }

      /**
       * @brief Equality operator.
       *
       * @param[in] lhs The \ref uuid to the left hand side of the ==.
       * @param[in] rhs The \ref uuid to the right hand side of the ==.
       *
       * @return true if \p lhs and \p rhs are not initialized, or
       *         if the string representation of \p rhs equals that of
       *         \p rhs, false otherwise. 
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

      /**
       * @brief Inequality operator.
       *
       * @param[in] lhs The \ref uuid to the left hand side of the !=.
       * @param[in] rhs The \ref uuid to the right hand side of the !=.
       *
       * @return false if \p lhs and \p rhs are not initialized, or
       *         if the string representation of \p rhs equals that of
       *         \p rhs, true otherwise. 
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
