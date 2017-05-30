#include "u_dagtasks/dag_vertex.h"

#include "u_dagtasks/dag_edge.h"

#include <algorithm>
#include <cassert>
#include <iostream>

namespace uber
{
  namespace u_dagtasks
  {
    dag_vertex::dag_vertex(const std::string &label) :
      current_status_(status::initialized),
      label_(label),
      incomming_edge_count_(0)
    {}

    dag_vertex::~dag_vertex()
    {
      current_status_ = status::invalid;
      incomming_edge_count_.store(0);
    }

    dag_vertex::dag_vertex(dag_vertex &&other)
    {
      uuid_ = std::move(other.uuid_);
      current_status_ = other.current_status_;
      label_ = std::move(other.label_);
      edges_ = std::move(other.edges_);
      incomming_edge_count_ = other.incomming_edge_count_.load();

      other.label_.clear();
      other.current_status_ = status::invalid;
      other.incomming_edge_count_.store(0ul);
    }

    dag_vertex &dag_vertex::operator=(dag_vertex &&rhs)
    {
      uuid_ = std::move(rhs.uuid_);
      current_status_ = rhs.current_status_;
      label_ = std::move(rhs.label_);
      edges_ = std::move(rhs.edges_);
      incomming_edge_count_ = rhs.incomming_edge_count_.load();

      rhs.label_.clear();
      rhs.current_status_ = status::invalid;
      rhs.incomming_edge_count_.store(0ul);

      return (*this);
    }

    dag_vertex dag_vertex::clone()
    {
      return (*this);
    }

    void dag_vertex::visit_all_edges(
      std::function<void (const dag_edge &)> cb) const
    {
      std::for_each(edges_.begin(), edges_.end(),
        [&](const std::unique_ptr<dag_edge> &e) {
          cb(*e);
        }
      );
    }

    const dag_edge &dag_vertex::get_edge(std::size_t index) const
    {
      assert(index < edge_count());

      return (*edges_[index]);
    }

    bool dag_vertex::contains_connection_to(const dag_vertex &other)
    {
      bool ret = false;

      for (const auto &ptrref : edges_) {
        if (ptrref->is_a_connection_to(other)) {
          ret = true;
          break;
        }
      }

      return ret;
    }

    bool dag_vertex::connect(std::shared_ptr<dag_vertex> other)
    {
      bool ret = false;

      if (!contains_connection_to(*other)) {
        std::unique_ptr<dag_edge> e(new dag_edge);
        e->connect_to(other);
        edges_.push_back(std::move(e));
        ret = true;
        assert(!edges_.empty());
        assert(edges_.back()->is_a_connection_to(*other));
      }

      return ret;
    }

    const uuid &dag_vertex::get_uuid() const
    {
      return uuid_;
    }

    const dag_vertex::status &dag_vertex::current_status() const
    {
      return current_status_;
    }

    std::string dag_vertex::current_status_as_string() const
    {
      std::string ret;

      switch(current_status_) {
      case status::initialized: {
        ret = "initialized";
      }
        break;
      case status::scheduled: {
        ret = "scheduled";
      }
        break;
      case status::running: {
        ret = "running";
      }
        break;
      case status::passed: {
        ret = "passed";
      }
        break;
      case status::failed: {
        ret = "failed";
      }
        break;
      case status::invalid: {
        ret = "invalid";
      }
        break;
      }

      return ret;
    }

    const std::string &dag_vertex::label() const
    {
      return label_;
    }

    std::size_t dag_vertex::edge_count() const
    {
      return edges_.size();
    }

    bool dag_vertex::has_incomming_edge() const
    {
      return (incomming_edge_count_ > 0);
    }

    std::size_t dag_vertex::incomming_edge_count() const
    {
      return incomming_edge_count_;
    }

    void dag_vertex::add_incomming_edge()
    {
      ++incomming_edge_count_;
    }

    void dag_vertex::sub_incomming_edge()
    {
      --incomming_edge_count_;
    }

    void dag_vertex::clear_edges()
    {
      edges_.clear();
    }

    dag_vertex::dag_vertex(const dag_vertex &other) :
      uuid_(const_cast<dag_vertex *>(&other)->uuid_.clone()),
      current_status_(other.current_status_),
      label_(other.label()),
      incomming_edge_count_(other.incomming_edge_count_.load())
    {
      dag_vertex &t = *(const_cast<dag_vertex *>(&other));

      std::for_each(t.edges_.begin(), t.edges_.end(),
        [&](std::unique_ptr<dag_edge> &o) {
          std::unique_ptr<dag_edge> tmp(new dag_edge(o->clone()));
          edges_.push_back(std::move(tmp));
          edges_.back()->connect_to(std::make_shared<dag_vertex>(
            o->get_connection().lock()->clone()));
      });
    }

    dag_vertex &dag_vertex::operator=(const dag_vertex &rhs)
    {
      dag_vertex &r = *(const_cast<dag_vertex *>(&rhs));

      uuid_ = const_cast<dag_vertex *>(&rhs)->uuid_.clone();
      current_status_ = rhs.current_status_;
      label_ = rhs.label();
      std::for_each(r.edges_.begin(), r.edges_.end(),
        [&](std::unique_ptr<dag_edge> &o) {
          std::unique_ptr<dag_edge> tmp(new dag_edge(o->clone()));
          edges_.push_back(std::move(tmp));
          edges_.back()->connect_to(std::make_shared<dag_vertex>(
            o->get_connection().lock()->clone()));
      });

      incomming_edge_count_ = rhs.incomming_edge_count_.load();

      return (*this);
    }

    std::ostream &operator<<(std::ostream &out, const dag_vertex &v)
    {
      out << "uuid_ = " << v.uuid_ << " current_status_ = "
        << v.current_status_as_string() << " label = " << v.label_
        << std::endl << "edges(" << v.edge_count() << "): ";
      v.visit_all_edges([&](const dag_edge &e) {
          out << "\t" << e << std::endl;
        }
      );

      return out;
    }

    bool operator==(const dag_vertex &lhs, const dag_vertex &rhs)
    {
      bool ret = true;

      auto lhs_str = lhs.uuid_.as_string();
      auto rhs_str = rhs.uuid_.as_string();

      ret &= (lhs_str == rhs_str);
      ret &= (lhs.label() == rhs.label());
      ret &= (lhs.edge_count() == rhs.edge_count());

      dag_vertex &tmp = *const_cast<dag_vertex *>(&lhs);

      // We are not guaranteed edges are in order. So this is O(e^2).
      for (std::unique_ptr<dag_edge> &e : tmp.edges_) {
        auto it = std::find_if(rhs.edges_.begin(), rhs.edges_.end(),
          [&](const std::unique_ptr<dag_edge> &a) {
            return ((*e) == (*a));
          }
        );
        if (it == lhs.edges_.end()) {
          ret &= false;
        }
      }

      return ret;
    }

    bool operator!=(const dag_vertex &lhs, const dag_vertex &rhs)
    {
      return !(lhs.uuid_.as_string() == rhs.uuid_.as_string());

    }
  }
}
