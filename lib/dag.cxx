#include "u_dagtasks/dag.h"

#include "u_dagtasks/dag_algorithms.h"
#include "u_dagtasks/dag_edge.h"

#include <algorithm>
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
      std::runtime_error(message),
      what_(message)
    {}

    const char *dag::dag_exception::what() const throw()
    {
      return what_.c_str();
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

    bool dag::add_vertex(dag_vertex &&v)
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
          return (u == rhs.get_uuid());
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

      std::list<dag_vertex> sorted_vertices;
      dag graph_clone = clone();

      dag_vertex v1_clone = *const_cast<dag_vertex *>(&v1);
      dag_vertex v2_clone = *const_cast<dag_vertex *>(&v2);
      graph_clone.add_vertex(std::move(v1_clone));
      graph_clone.add_vertex(std::move(v2_clone));
      std::weak_ptr<dag_vertex> v1_found = graph_clone.find_vertex(v1_clone);
      std::weak_ptr<dag_vertex> v2_found = graph_clone.find_vertex(v2_clone);
      // Bypass dag interface to prevent infinite recursion.
      v1_found.lock()->connect(v2_found.lock());

      assert((!v1_found.expired() && !v2_found.expired()) &&
        "There must be a regression in add_vertex.");

      ret = dag_topological_sort(graph_clone, sorted_vertices);

      return ret;
    }

    bool dag::connection_would_make_cyclic_by_uuid(const uuid &u1,
      const uuid &u2)
    {
      bool ret = false;

      std::weak_ptr<dag_vertex> v1_tmp = find_vertex_by_uuid(u1);
      std::weak_ptr<dag_vertex> v2_tmp = find_vertex_by_uuid(u2);

      ret = connection_would_make_cyclic(*(v1_tmp.lock()), *(v2_tmp.lock()));

      return ret;
    }

    bool dag::connection_would_make_cyclic_by_label(const std::string &l1,
      const std::string &l2)
    {
      bool ret = false;

      std::vector<std::weak_ptr<dag_vertex>> v1;
      v1 = find_all_verticies_with_label(l1);
      std::vector<std::weak_ptr<dag_vertex>> v2;
      v2 = find_all_verticies_with_label(l2);

      for (auto v : v1) {
        for (auto u : v2) {
          ret &= connection_would_make_cyclic(*(v.lock()), *(u.lock()));
        }
      }

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
          if (!connection_would_make_cyclic(*(v1_it->get()),
            *(v2_it->get()))) {
            v1_it->get()->connect(*v2_it);
            ret = true;
          } else {
            std::stringstream error_str;
            error_str << "Connecting " << std::endl << (*(v1_tmp.lock()))
              << std::endl << "to " << std::endl << (*(v2_tmp.lock()))
              << std::endl << "would cause a cycle.";
            throw dag_exception(error_str.str().c_str());
          }
        }
      }

      return ret;
    }

    bool dag::connect_by_uuid(const uuid &u1, const uuid &u2)
    {
      bool ret = false;

      std::weak_ptr<dag_vertex> v1_tmp = find_vertex_by_uuid(u1);
      std::weak_ptr<dag_vertex> v2_tmp = find_vertex_by_uuid(u2);

      ret = connect(*(v1_tmp.lock()), *(v2_tmp.lock()));

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
          ret &= connect(*(v.lock()), *(u.lock()));
        }
      }

      return ret;
    }

    bool dag::add_and_connect(dag_vertex &&v1, dag_vertex &&v2)
    {
      bool ret = false;

      std::string l1 = v1.label();
      std::string l2 = v2.label();

      add_vertex(std::move(v1));
      add_vertex(std::move(v2));

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

    void dag::reset()
    {
      graph_.clear();
    }

    dag::dag(const dag &other)
    {
      dag *o = (const_cast<dag *>(&other));
      o->linear_traversal([&](std::shared_ptr<dag_vertex> v) {
          dag_vertex tmp = v->clone();
          add_vertex(std::move(tmp));
        }
      );
    }

    dag &dag::operator=(const dag &rhs)
    {
      dag &o = *(const_cast<dag *>(&rhs));
      o.linear_traversal([&](std::shared_ptr<dag_vertex> v) {
          dag_vertex tmp = v->clone();
          add_vertex(std::move(tmp));
        }
      );

      return (*this);
    }

    bool operator==(const dag &lhs, const dag &rhs)
    {
      bool ret = true;

      ret &= (lhs.graph_.size() == rhs.graph_.size());

      dag lhs_clone = (*const_cast<dag *>(&lhs)).clone();
      dag rhs_clone = (*const_cast<dag *>(&rhs)).clone();

      std::sort(lhs_clone.graph_.begin(), lhs_clone.graph_.end(),
        [](std::shared_ptr<dag_vertex> a, std::shared_ptr<dag_vertex> b) {
          return a->label() < b->label();
        }
      );
      std::sort(rhs_clone.graph_.begin(), rhs_clone.graph_.end(),
        [](std::shared_ptr<dag_vertex> a, std::shared_ptr<dag_vertex> b) {
          return a->label() < b->label();
        }
      );

      std::size_t index = 0;
      for (auto v : lhs.graph_) {
        ret &= ((*v) == (*(rhs.graph_[index])));
        auto o = rhs.graph_[index]; // Force use count up.
        ret &= (v.use_count() == o.use_count());

        if (!ret) {
          break;
        }

        ++index;
      }

      return ret;
    }

    bool operator!=(const dag &lhs, const dag &rhs)
    {
      return !(lhs == rhs);
    }
  }
}
