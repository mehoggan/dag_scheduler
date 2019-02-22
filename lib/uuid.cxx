#include "dag_scheduler/uuid.h"

#include <iostream>

namespace com
{
  namespace dag_scheduler
  {
    uuid::uuid()
    {
      uuid_generate(uuid_);
    }

    uuid::~uuid()
    {
      uuid_clear(uuid_);
    }

    uuid uuid::clone()
    {
      return (*this);
    }

    uuid::uuid(uuid &&other)
    {
      uuid_copy(uuid_, other.uuid_);
      other.~uuid();
    }

    uuid &uuid::operator=(uuid &&rhs)
    {
      uuid_copy(uuid_, rhs.uuid_);
      rhs.~uuid();

      return (*this);
    }

    bool uuid::is_initialized() const
    {
      return !uuid_is_null(uuid_);
    }

    std::string uuid::as_string() const
    {
      std::string ret;
      if (is_initialized()) {
        char uuid_str[37];
        uuid_unparse_lower(uuid_, uuid_str);
        ret = std::string(uuid_str);
      }

      return ret;
    }

    uuid::uuid(const uuid &other)
    {
      uuid_copy(uuid_, other.uuid_);
    }

    uuid &uuid::operator=(const uuid &rhs)
    {
      uuid_copy(uuid_, rhs.uuid_);

      return (*this);
    }
  }
}
