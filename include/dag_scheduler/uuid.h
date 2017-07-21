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
    class DLLSPEC_DAGTASKS uuid
    {
    public:
      uuid();
      ~uuid();

      uuid(uuid &&other);
      uuid &operator=(uuid &&rhs);

      uuid clone();

      bool is_initialized() const;
      std::string as_string() const;

      friend std::ostream &operator<<(std::ostream &out, const uuid &u)
      {
        out << u.as_string();
        return out;
      }

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
