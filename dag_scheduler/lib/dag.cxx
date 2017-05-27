#include "u_dagtasks/dag.h"

#include "u_dagtasks/dag_edge.h"

#include <cassert>
#include <list>
#include <iostream>
#include <sstream>

// TODO: Test this entire file.

namespace uber
{
  namespace u_dagtasks
  {
    dag::dag_exception::dag_exception(const char *message) :
      std::runtime_error(message)
    {}

    const char *dag::dag_exception::what() const throw()
    {
      return std::exception::what();
    }

    dag::dag()
    {}

    dag::~dag()
    {}

    dag::dag(dag &&other) :
      graph_(std::move(other.graph_))
    {}

    dag &dag::operator=(dag &&other)
    {
      graph_ = std::move(other.graph_);
      return (*this);
    }

    dag dag::clone()
    {
      return (*this);
    }

    bool dag::add_vertex(dag_vertex &v)
    {
      bool ret = false;

      if (!contains_vertex_by_label(v.label())) {
        auto graph_vertex = std::make_shared<dag_vertex>(std::move(v));
        graph_.push_back(graph_vertex);
        ret = true;
      }

      return ret;
    }

    std::weak_ptr<dag_vertex> dag::find_vertex(const dag_vertex &v)
    {
      std::weak_ptr<dag_vertex> ret;

      auto it = std::find_if(graph_.begin(), graph_.end(),
        [&](std::shared_ptr<dag_vertex> vi) {
          const dag_vertex &rhs = (*(vi.get()));
          return (v == rhs);
        }
      );

      if (it != graph_.end()) {
        ret = (*it);
      }

      return ret;
    }

    std::weak_ptr<dag_vertex> dag::find_vertex_by_uuid(const uuid &u)
    {
      std::weak_ptr<dag_vertex> ret;

      auto it = std::find_if(graph_.begin(), graph_.end(),
        [&](std::shared_ptr<dag_vertex> vi) {
          const dag_vertex &rhs = (*(vi.get()));
          return (u == rhs.uuid());
        }
      );

      if (it != graph_.end()) {
        ret = (*it);
      }

      return ret;
    }

    std::vector<std::weak_ptr<dag_vertex>> dag::find_all_verticies_with_label(
      const std::string &l)
    {
      std::vector<std::weak_ptr<dag_vertex>> ret;

      for (auto it = graph_.begin(); it != graph_.end(); ++it) {
        if ((*it)->label() == l) {
          std::weak_ptr<dag_vertex> tmp = (*it);
          ret.push_back(tmp);
        }
      }

      return ret;
    }

    bool dag::contains_vertex(const dag_vertex &v)
    {
      bool ret = false;

      if (!find_vertex(v).expired()) {
        ret = true;
      }

      return ret;
    }

    bool dag::contains_vertex_by_uuid(const uuid &u)
    {
      bool ret = false;

      if (!find_vertex_by_uuid(u).expired()) {
        ret = true;
      }

      return ret;
    }

    bool dag::contains_vertex_by_label(const std::string &l)
    {
      bool ret = false;

      if (!find_all_verticies_with_label(l).empty()) {
        ret = true;
      }

      return ret;
    }

    bool dag::connection_would_make_cyclic(const dag_vertex &v1,
      const dag_vertex &v2)
    {
      bool ret = false;

      std::list<std::shared_ptr<dag_vertex>> sorted_vertices;
      std::list<std::shared_ptr<dag_vertex>> entry_vertices;

      dag dag_clone = clone();

      dag_vertex v1_clone = (*const_cast<dag_vertex *>(&v1)).clone();
      dag_vertex v2_clone = (*const_cast<dag_vertex *>(&v2)).clone();

      dag_clone.add_vertex(v1_clone);
      dag_clone.add_vertex(v2_clone);

      auto v1_ptr = dag_clone.find_vertex(v1);
      auto v2_ptr = dag_clone.find_vertex(v2);

      assert(!v1_ptr.expired() && "Failed to find vertex 1.");
      assert(!v2_ptr.expired() && "Failed to find vertex 2.");
      v1_ptr.lock()->connect(v2_ptr.lock());

      dag_clone.linear_traversal([&](std::shared_ptr<dag_vertex> v) {
          if (!v->has_incomming_edge()) {
            entry_vertices.push_back(v);
          }
        }
      );

      while (!entry_vertices.empty()) {
        std::shared_ptr<dag_vertex> n = entry_vertices.front();
        sorted_vertices.push_back(n);
        entry_vertices.pop_front();

        n->visit_all_edges([&](const dag_edge &e) {
            dag_edge &e_tmp = *const_cast<dag_edge *>(&e);
            std::weak_ptr<dag_vertex> connection = e_tmp.get_connection();

            if (connection.lock()->incomming_edge_count() == 1) {
              entry_vertices.push_back(connection.lock());
            }

            // Will reduce incomming connections.
            e_tmp.connect_to(nullptr);
          }
        );

        n->clear_edges();
      }

      for (auto v : dag_clone.graph_) {
        if (v->edge_count() != 0) {
          ret = true;
          break;
        }
      }

      return ret;
    }

    bool dag::connection_would_make_cyclic_by_uuid(const uuid &u1,
      const uuid &u2)
    {
      bool ret = false;

      return ret;
    }

    bool dag::connection_would_make_cyclic_by_label(const std::string &l1,
      const std::string &l2)
    {
      bool ret = false;

      return ret;
    }

    bool dag::connect(const dag_vertex &v1, const dag_vertex &v2)
    {
      bool ret = false;

      std::weak_ptr<dag_vertex> v1_tmp = find_vertex(v1);
      std::weak_ptr<dag_vertex> v2_tmp = find_vertex(v2);

      if (!v1_tmp.expired() && !v2_tmp.expired()) {
        auto v1_it = std::find(graph_.begin(), graph_.end(), v1_tmp.lock());
        auto v2_it = std::find(graph_.begin(), graph_.end(), v2_tmp.lock());

        if (v1_it != graph_.end() && v2_it != graph_.end()) {
          /*
          if (!connection_would_make_cyclic(*(v1_it->get()),
            *(v2_it->get()))) {
            v1_it->get()->connect(*v2_it);
            ret = true;
          } else {
            std::stringstream error_str;
            error_str << "Connecting " << (*(v1_tmp.lock())) << " to "
              << (*(v2_tmp.lock())) << " would cause a cycle.";
            throw dag_exception(error_str.str().c_str());
          }
          */
        }
      }

      return ret;
    }

    bool dag::connect_by_uuid(const uuid &u1, const uuid &u2)
    {
      bool ret = false;

      std::weak_ptr<dag_vertex> v1_tmp = find_vertex_by_uuid(u1);
      std::weak_ptr<dag_vertex> v2_tmp = find_vertex_by_uuid(u2);

      if (!v1_tmp.expired() && !v2_tmp.expired()) {
        auto v1_it = std::find(graph_.begin(), graph_.end(), v1_tmp.lock());
        auto v2_it = std::find(graph_.begin(), graph_.end(), v2_tmp.lock());

        if (v1_it != graph_.end() && v2_it != graph_.end()) {
          if (!connection_would_make_cyclic(*(v1_it->get()),
            *(v2_it->get()))) {
            v1_it->get()->connect(*v2_it);
            ret = true;
          } else {
            std::stringstream error_str;
            error_str << "Connecting " << (*(v1_tmp.lock())) << " to "
              << (*(v2_tmp.lock())) << " would cause a cycle.";
            throw dag_exception(error_str.str().c_str());
          }
        }
      }

      return ret;
    }

    bool dag::connect_all_by_label(const std::string l1,
      const std::string l2)
    {
      bool ret = false;

      std::vector<std::weak_ptr<dag_vertex>> v1;
      v1 = find_all_verticies_with_label(l1);
      std::vector<std::weak_ptr<dag_vertex>> v2;
      v2 = find_all_verticies_with_label(l2);

      for (auto v : v1) {
        for (auto u : v2) {
          if (!connection_would_make_cyclic(*(v.lock()), *(u.lock()))) {
            v.lock()->connect(u.lock());
            ret = true;
          } else {
            std::stringstream error_str;
            error_str << "Connecting " << (*(v.lock())) << " to "
              << (*(u.lock())) << " would cause a cycle.";
            throw dag_exception(error_str.str().c_str());
          }
        }
      }

      return ret;
    }

    bool dag::add_and_connect(dag_vertex &v1, dag_vertex &v2)
    {
      bool ret = false;

      std::string l1 = v1.label();
      std::string l2 = v2.label();

      add_vertex(v1);
      add_vertex(v2);

      if (!connection_would_make_cyclic(v1, v2)) {
        ret = connect_all_by_label(l1, l2);
      }

      return ret;
    }

    bool dag::are_connected(const dag_vertex &v1, const dag_vertex &v2)
    {
      bool ret = false;

      std::weak_ptr<dag_vertex> v1_tmp = find_vertex(v1);
      std::weak_ptr<dag_vertex> v2_tmp = find_vertex(v2);

      dag_vertex &v2_ref = *(v2_tmp.lock().get());
      std::uintptr_t v2_addr = reinterpret_cast<std::uintptr_t>(&v2_ref);
      v1_tmp.lock()->visit_all_edges([&](const dag_edge &e) {
          dag_edge &e_tmp = *const_cast<dag_edge *>(&e);
          dag_vertex &c_ref = *(e_tmp.get_connection().lock().get());
          std::uintptr_t c_addr = reinterpret_cast<std::uintptr_t>(&c_ref);
          if (c_addr == v2_addr) {
            ret = true;
          }
        }
      );

      return ret;
    }

    bool dag::are_connected_by_uuid(const uuid &u1, const uuid &u2)
    {
      bool ret = false;

      std::weak_ptr<dag_vertex> v1_tmp = find_vertex_by_uuid(u1);
      std::weak_ptr<dag_vertex> v2_tmp = find_vertex_by_uuid(u2);

      ret = are_connected(*(v1_tmp.lock().get()), *(v2_tmp.lock().get()));

      return ret;
    }

    bool dag::all_are_connected_by_label(const std::string l1,
      const std::string l2)
    {
      bool ret = true;

      std::vector<std::weak_ptr<dag_vertex>> v1;
      v1 = find_all_verticies_with_label(l1);
      std::vector<std::weak_ptr<dag_vertex>> v2;
      v2 = find_all_verticies_with_label(l2);

      for (auto vfrom : v1) {
        for (auto vto : v2) {
          ret &= are_connected(*(vfrom.lock()), *(vto.lock()));
        }
      }

      return ret;
    }

    void dag::linear_traversal(
      std::function<void (std::shared_ptr<dag_vertex>)> cb)
    {
      std::for_each(graph_.begin(), graph_.end(), cb);
    }

    std::size_t dag::vertex_count() const
    {
      return graph_.size();
    }

    std::size_t dag::edge_count() const
    {
      std::size_t ret = 0;

      (const_cast<dag *>(this))->linear_traversal(
        [&] (std::shared_ptr<dag_vertex> v) {
          ret += v->edge_count();
        }
      );

      return ret;
    }

    dag::dag(const dag &other)
    {
      dag *o = (const_cast<dag *>(&other));
      o->linear_traversal([&](std::shared_ptr<dag_vertex> v) {
          dag_vertex tmp = v->clone();
          add_vertex(tmp);
        }
      );
    }

    dag &dag::operator=(const dag &rhs)
    {
      dag &o = *(const_cast<dag *>(&rhs));
      o.linear_traversal([&](std::shared_ptr<dag_vertex> v) {
          dag_vertex tmp = v->clone();
          add_vertex(tmp);
        }
      );

      return (*this);
    }
  }
}
