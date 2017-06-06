#include "u_dagtasks/dag_vertex.h"

#include "u_dagtasks/dag_edge.h"

#include <cassert>
#include <iostream>

namespace uber
{
  namespace u_dagtasks
  {
    dag_vertex::dag_vertex_connection::dag_vertex_connection()
    {}

    dag_vertex::dag_vertex_connection::dag_vertex_connection(
      dag_edge &edge, dag_vertex &vertex) :
      edge_(new dag_edge(edge.clone())),
      vertex_(new dag_vertex(vertex.clone()))
    {}

    const dag_edge &dag_vertex::dag_vertex_connection::edge() const
    {
      return *edge_;
    }

    const dag_vertex &dag_vertex::dag_vertex_connection::vertex() const
    {
      return *vertex_;
    }

    dag_vertex::dag_vertex() :
      current_status_(status::initialized),
      incomming_edge_count_(0)
    {}

    dag_vertex::dag_vertex(const std::string &label) :
      current_status_(status::initialized),
      label_(label),
      incomming_edge_count_(0)
    {}

    dag_vertex::~dag_vertex()
    {
      current_status_ = status::invalid;
      label_.clear();
      clear_edges();
      incomming_edge_count_.store(0);
    }

    dag_vertex::dag_vertex(dag_vertex &&other)
    {
      uuid_ = std::move(other.uuid_);
      label_ = std::move(other.label_);
      current_status_ = other.current_status_;
      edges_ = std::move(other.edges_);
      incomming_edge_count_ = other.incomming_edge_count_.load();

      other.label_.clear();
      other.current_status_ = status::invalid;
      other.incomming_edge_count_.store(0ul);
    }

    dag_vertex &dag_vertex::operator=(dag_vertex &&rhs)
    {
      uuid_ = std::move(rhs.uuid_);
      label_ = std::move(rhs.label_);
      current_status_ = rhs.current_status_;
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

    std::vector<std::shared_ptr<dag_vertex>> dag_vertex::restablish_connections(
      std::vector<dag_vertex_connection> &connections)
    {
      std::vector<std::shared_ptr<dag_vertex>> ret;
      ret.reserve(connections.size());
      for (auto &connection : connections) {
        dag_vertex vertex = *(const_cast<dag_vertex *>(&connection.vertex()));
        std::shared_ptr<dag_vertex> other = std::make_shared<dag_vertex>(
          vertex.clone());
        dag_edge edge = *(const_cast<dag_edge *>(&connection.edge()));
        std::unique_ptr<dag_edge> tmp_edge(new dag_edge(edge.clone()));
        tmp_edge->connect_to(other);
        // We need to preserve the shared_ptr so the edge's weak_ptr does not
        // expire.
        ret.push_back(other);
        edges_.push_back(std::move(tmp_edge));
      }

      return ret;
    }

    std::size_t dag_vertex::edge_count() const
    {
      return edges_.size();
    }

    void dag_vertex::visit_all_edges(
      std::function<void (const dag_edge &)> cb) const
    {
      std::size_t edge_count = edges_.size();
      (void)edge_count;

      std::for_each(edges_.begin(), edges_.end(),
        [&](const std::unique_ptr<dag_edge> &e) {
          cb(*e);
        }
      );
    }

    std::vector<dag_vertex::dag_vertex_connection>
    dag_vertex::clone_all_connections()
    {
      std::vector<dag_vertex_connection> ret;
      ret.reserve(edges_.size());

      for (std::unique_ptr<dag_edge> &e : edges_) {
        dag_edge e_clone = e->clone();
        dag_vertex v_clone = e->get_connection().lock()->clone();
        ret.push_back(dag_vertex_connection(e_clone, v_clone));
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

    bool dag_vertex::has_incomming_edges() const
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
      // We cannot add back the connections since the edge adds a weak_ptr
      // to a dag_vertex we no longer can duplicate. This has to be done
      // outside the class by the code that is cloning the dag_vertex. dag_graph
      // should be the object that orchestrates that.
    }

    dag_vertex &dag_vertex::operator=(const dag_vertex &rhs)
    {
      dag_vertex &t = *(const_cast<dag_vertex *>(&rhs));

      uuid_ = t.uuid_.clone();
      current_status_ = t.current_status_;
      label_ = rhs.label();
      incomming_edge_count_ = rhs.incomming_edge_count_.load();
      // We cannot add back the connections since the edge adds a weak_ptr
      // to a dag_vertex we no longer can duplicate. This has to be done
      // outside the class by the code that is cloning the dag_vertex. dag_graph
      // should be the object that orchestrates that.

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

      std::size_t lhs_edge_count = lhs.edge_count();
      std::size_t rhs_edge_count = rhs.edge_count();
      ret &= (lhs_edge_count == rhs_edge_count);

      dag_vertex &tmp = *const_cast<dag_vertex *>(&lhs);

      // We are not guaranteed edges are in order. So this is O(e^2).
      for (std::unique_ptr<dag_edge> &e : tmp.edges_) {
        auto it = std::find_if(rhs.edges_.begin(), rhs.edges_.end(),
          [&](const std::unique_ptr<dag_edge> &a) {
            return ((*e) == (*a));
          }
        );
        if (it == rhs.edges_.end()) {
          ret &= false;
        }
      }

      return ret;
    }

    bool operator!=(const dag_vertex &lhs, const dag_vertex &rhs)
    {
      return !(lhs == rhs);

    }
  }
}
