#include "u_dagtasks/dag_edge.h"

#include <iostream>

namespace uber
{
  namespace u_dagtasks
  {
    dag_edge::dag_edge() :
      current_status_(status::initialized)
    {}

    dag_edge::~dag_edge()
    {
      current_status_ = status::non_traverable;
      connection_.reset();
    }

    dag_edge::dag_edge(dag_edge &&other)
    {
      uuid_ = std::move(other.uuid_);
      current_status_ = other.current_status_;
      connection_ = std::move(other.connection_);
      other.current_status_ = status::non_traverable;
    }

    dag_edge &dag_edge::operator=(dag_edge &&rhs)
    {
      uuid_ = std::move(rhs.uuid_);
      current_status_ = rhs.current_status_;
      connection_ = std::move(rhs.connection_);
      rhs.current_status_ = status::non_traverable;

      return (*this);
    }

    dag_edge dag_edge::clone()
    {
      return (*this);
    }

    std::weak_ptr<dag_vertex> dag_edge::get_connection()
    {
      return connection_;
    }

    bool dag_edge::connect_to(std::shared_ptr<dag_vertex> v)
    {
      bool ret = true;

      if (!connection_.expired()) {
        connection_.lock()->sub_incomming_edge();
        ret = false;
      }

      connection_ = v;
      if (!connection_.expired()) {
        connection_.lock()->add_incomming_edge();
      }

      return ret;
    }

    bool dag_edge::is_a_connection_to(const dag_vertex &v) const
    {
      bool ret = false;

      if (!connection_.expired() && (v == (*(connection_.lock())))) {
        ret = true;
      }

      return ret;
    }

    const uuid &dag_edge::get_uuid() const
    {
      return uuid_;
    }

    const dag_edge::status &dag_edge::current_status() const
    {
      return current_status_;
    }

    std::string dag_edge::current_status_as_string() const
    {
      std::string ret;

      switch(current_status_) {
      case status::initialized: {
        ret = "initialized";
      }
        break;
      case status::traversed: {
        ret = "traversed";
      }
        break;
      case status::non_traverable: {
        ret = "non_traverable";
      }
        break;
      }

      return ret;
    }

    dag_edge::dag_edge(const dag_edge &other) :
      uuid_(const_cast<dag_edge *>(&other)->uuid_.clone()),
      current_status_(other.current_status()),
      connection_(/*We cannot connect because we do NOT own.*/)
    {}

    dag_edge &dag_edge::operator=(const dag_edge &rhs)
    {
      uuid_ = const_cast<dag_edge *>(&rhs)->uuid_.clone();
      current_status_ = rhs.current_status();
      connection_.reset(/*We cannot connect because we do NOT own.*/);

      return (*this);
    }

    std::ostream &operator<<(std::ostream &out, const std::nullptr_t n)
    {
      out << "nullptr";

      return out;
    }

    std::ostream &operator<<(std::ostream &out, const dag_edge &e)
    {
      out << "uuid_ = " << e.uuid_ << " current_status_ = "
        << "(" << e.current_status_as_string() << ") connection = "
        << (*(e.connection_.lock().get()));

      return out;
    }

    bool operator==(const dag_edge &lhs, const dag_edge &rhs)
    {
      bool ret = true;

      ret &= (lhs.uuid_.as_string() == rhs.uuid_.as_string());
      ret &= (lhs.connection_.lock().use_count() ==
        rhs.connection_.lock().use_count());
      ret &= (lhs.current_status_ == rhs.current_status_);

      if (lhs.connection_.lock() && rhs.connection_.lock()) {
        ret &= ((*(lhs.connection_.lock().get()) ==
          (*(rhs.connection_.lock().get()))));
      } else {
        ret &= (!lhs.connection_.lock().get() &&
          !rhs.connection_.lock().get());
      }

      return ret;
    }

    bool operator!=(const dag_edge &lhs, const dag_edge &rhs)
    {
      bool ret = !(lhs == rhs);

      return ret;
    }
  }
}
