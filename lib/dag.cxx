#include "dag_scheduler/dag.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "dag_scheduler/dag_algorithms.h"
#include "dag_scheduler/dag_edge.h"
#include "dag_scheduler/logged_class.hpp"

namespace com {
namespace dag_scheduler {
DAG::DAGException::DAGException(const char *message)
    : std::runtime_error(message), what_(message) {}

const char *DAG::DAGException::what() const throw() { return what_.c_str(); }

DAG::DAG() : DAG("") {}

DAG::DAG(const std::string &title)
    : LoggedClass(*this),
      title_(title),
      json_config_(std::make_unique<rapidjson::Document>()) {
  Logging::info(LOG_TAG, "Created DAG with title=--", title_, "--");
}

DAG::DAG(const std::string &title, const rapidjson::Document &json_config)
    : LoggedClass(*this),
      title_(title),
      json_config_(std::make_unique<rapidjson::Document>()) {
  Logging::info(LOG_TAG, "Created DAG with title=--", title_, "--");
  json_config_->CopyFrom(json_config, json_config_->GetAllocator());
}

DAG::DAG(const rapidjson::Document &json_config)
    : LoggedClass(*this),
      json_config_(std::make_unique<rapidjson::Document>()) {
  Logging::info(LOG_TAG, "Created DAG with title=--", title_, "--");
  json_config_->CopyFrom(json_config, json_config_->GetAllocator());
}

DAG::~DAG() {}

DAG::DAG(DAG &&other)
    : LoggedClass(*this),
      graph_(std::move(other.graph_)),
      title_(other.title_),
      json_config_(std::move(other.json_config_)) {
  Logging::info(LOG_TAG, "Moved DAG with title=", title_);
}

DAG &DAG::operator=(DAG &&other) {
  graph_ = std::move(other.graph_);
  title_ = other.title_;
  json_config_ = std::move(other.json_config_);
  Logging::info(LOG_TAG, "Moved Assigned DAG with title=", title_);
  return (*this);
}

DAG DAG::clone() { return (*this); }

bool DAG::add_vertex(DAGVertex &&v) {
  bool ret = false;

  if (v.get_task() == nullptr) {
    Logging::warn(LOG_TAG, "Adding vertex with no task!!!");
  }

  if (!contains_vertex(v)) {
    auto graph_vertex = std::make_shared<DAGVertex>(std::move(v));
    graph_.push_back(graph_vertex);

    if (graph_.back()->get_task() == nullptr) {
      Logging::warn(LOG_TAG, "Vertex at end of vertices has no task!!!");
    }

    ret = true;
  }

  return ret;
}

std::weak_ptr<DAGVertex> DAG::find_vertex(const DAGVertex &v) {
  std::weak_ptr<DAGVertex> ret = find_vertex_by_uuid(v.get_uuid());
  return ret;
}

std::weak_ptr<DAGVertex> DAG::find_vertex_by_uuid(const UUID &u) {
  std::weak_ptr<DAGVertex> ret;

  auto it = std::find_if(graph_.begin(), graph_.end(),
                         [&](std::shared_ptr<DAGVertex> vi) {
                           const DAGVertex &rhs = (*(vi.get()));

                           std::string u_str = u.as_string();
                           std::string rhs_str = rhs.get_uuid().as_string();
                           (void)u_str;
                           (void)rhs_str;

                           return (u == rhs.get_uuid());
                         });

  if (it != graph_.end()) {
    ret = (*it);
  }

  return ret;
}

std::vector<std::weak_ptr<DAGVertex>> DAG::find_all_verticies_with_label(
    const std::string &l) {
  std::vector<std::weak_ptr<DAGVertex>> ret;

  for (auto it = graph_.begin(); it != graph_.end(); ++it) {
    if ((*it)->label() == l) {
      std::weak_ptr<DAGVertex> tmp = (*it);
      ret.push_back(tmp);
    }
  }

  return ret;
}

bool DAG::contains_vertex(const DAGVertex &v) {
  bool ret = false;

  if (!find_vertex(v).expired()) {
    ret = true;
  }

  return ret;
}

bool DAG::contains_vertex_by_uuid(const UUID &u) {
  bool ret = false;

  if (!find_vertex_by_uuid(u).expired()) {
    ret = true;
  }

  return ret;
}

bool DAG::contains_vertex_by_label(const std::string &l) {
  bool ret = false;

  if (!find_all_verticies_with_label(l).empty()) {
    ret = true;
  }

  return ret;
}

bool DAG::connection_would_make_cyclic(const DAGVertex &v1,
                                       const DAGVertex &v2) {
  bool ret = false;

  std::list<DAGVertex> sorted_vertices;
  DAG graph_clone = clone();

  DAGVertex v1_clone = *const_cast<DAGVertex *>(&v1);
  DAGVertex v2_clone = *const_cast<DAGVertex *>(&v2);
  graph_clone.add_vertex(std::move(v1_clone));
  graph_clone.add_vertex(std::move(v2_clone));
  std::weak_ptr<DAGVertex> v1_found = graph_clone.find_vertex(v1_clone);
  std::weak_ptr<DAGVertex> v2_found = graph_clone.find_vertex(v2_clone);
  // Bypass DAG interface to prevent infinite recursion.
  v1_found.lock()->connect(v2_found.lock());

  assert((!v1_found.expired() && !v2_found.expired()) &&
         "There must be a regression in add_vertex.");

  ret = dag_topological_sort(graph_clone, sorted_vertices);

  return ret;
}

bool DAG::connection_would_make_cyclic_by_uuid(const UUID &u1, const UUID &u2) {
  bool ret = false;

  std::weak_ptr<DAGVertex> v1_tmp = find_vertex_by_uuid(u1);
  std::weak_ptr<DAGVertex> v2_tmp = find_vertex_by_uuid(u2);

  ret = connection_would_make_cyclic(*(v1_tmp.lock()), *(v2_tmp.lock()));

  return ret;
}

bool DAG::connection_would_make_cyclic_by_label(const std::string &l1,
                                                const std::string &l2) {
  bool ret = true;

  std::vector<std::weak_ptr<DAGVertex>> v1;
  v1 = find_all_verticies_with_label(l1);
  std::vector<std::weak_ptr<DAGVertex>> v2;
  v2 = find_all_verticies_with_label(l2);

  for (auto v : v1) {
    bool good = true;

    for (auto u : v2) {
      ret &= (!connection_would_make_cyclic(*(v.lock()), *(u.lock())));
      if (!ret) {
        good = false;
        break;
      }
    }
    if (!good) {
      break;
    }
  }
  return ret;
}

bool DAG::connect(const DAGVertex &v1, const DAGVertex &v2) {
  bool ret = false;

  std::weak_ptr<DAGVertex> v1_tmp = find_vertex(v1);
  std::weak_ptr<DAGVertex> v2_tmp = find_vertex(v2);

  if (!v1_tmp.expired() && !v2_tmp.expired()) {
    auto v1_it = std::find(graph_.begin(), graph_.end(), v1_tmp.lock());
    auto v2_it = std::find(graph_.begin(), graph_.end(), v2_tmp.lock());

    if (v1_it != graph_.end() && v2_it != graph_.end()) {
      if (!connection_would_make_cyclic(*(v1_it->get()), *(v2_it->get()))) {
        v1_it->get()->connect(*v2_it);
        ret = true;
      } else {
        std::stringstream error_str;
        error_str << "Connecting " << std::endl
                  << (*(v1_tmp.lock())) << std::endl
                  << "to " << std::endl
                  << (*(v2_tmp.lock())) << std::endl
                  << "would cause a cycle.";
        throw DAGException(error_str.str().c_str());
      }
    }
  }

  return ret;
}

bool DAG::connect_by_uuid(const UUID &u1, const UUID &u2) {
  bool ret = false;

  std::weak_ptr<DAGVertex> v1_tmp = find_vertex_by_uuid(u1);
  std::weak_ptr<DAGVertex> v2_tmp = find_vertex_by_uuid(u2);

  ret = connect(*(v1_tmp.lock()), *(v2_tmp.lock()));

  return ret;
}

bool DAG::connect_all_by_label(const std::string l1, const std::string l2) {
  bool ret = true;

  std::vector<std::weak_ptr<DAGVertex>> v1;
  v1 = find_all_verticies_with_label(l1);
  std::vector<std::weak_ptr<DAGVertex>> v2;
  v2 = find_all_verticies_with_label(l2);

  for (auto v : v1) {
    for (auto u : v2) {
      ret &= connect(*(v.lock()), *(u.lock()));
    }
  }

  return ret;
}

bool DAG::add_and_connect(DAGVertex &&v1, DAGVertex &&v2) {
  bool ret = false;

  DAGVertex v1_clone = v1.clone();
  DAGVertex v2_clone = v2.clone();

  add_vertex(std::move(v1));
  add_vertex(std::move(v2));

  std::weak_ptr<DAGVertex> v1_ptr = find_vertex(v1_clone);
  std::weak_ptr<DAGVertex> v2_ptr = find_vertex(v2_clone);

  ret = connect(*(v1_ptr.lock().get()), *(v2_ptr.lock().get()));

  return ret;
}

bool DAG::are_connected(const DAGVertex &v1, const DAGVertex &v2) {
  bool ret = false;

  std::weak_ptr<DAGVertex> v1_tmp = find_vertex(v1);
  std::weak_ptr<DAGVertex> v2_tmp = find_vertex(v2);

  DAGVertex &v2_ref = *(v2_tmp.lock().get());
  std::uintptr_t v2_addr = reinterpret_cast<std::uintptr_t>(&v2_ref);
  v1_tmp.lock()->visit_all_edges([&](const DAGEdge &e) {
    DAGEdge &e_tmp = *const_cast<DAGEdge *>(&e);
    DAGVertex &c_ref = *(e_tmp.get_connection().lock().get());
    std::uintptr_t c_addr = reinterpret_cast<std::uintptr_t>(&c_ref);
    if (c_addr == v2_addr) {
      ret = true;
    }
  });

  return ret;
}

bool DAG::are_connected_by_uuid(const UUID &u1, const UUID &u2) {
  bool ret = false;

  std::weak_ptr<DAGVertex> v1_tmp = find_vertex_by_uuid(u1);
  std::weak_ptr<DAGVertex> v2_tmp = find_vertex_by_uuid(u2);

  ret = are_connected(*(v1_tmp.lock().get()), *(v2_tmp.lock().get()));

  return ret;
}

bool DAG::all_are_connected_by_label(const std::string l1,
                                     const std::string l2) {
  bool ret = true;

  std::vector<std::weak_ptr<DAGVertex>> v1;
  v1 = find_all_verticies_with_label(l1);
  std::vector<std::weak_ptr<DAGVertex>> v2;
  v2 = find_all_verticies_with_label(l2);

  ret &= (!v1.empty() && !v2.empty());

  for (auto vfrom : v1) {
    for (auto vto : v2) {
      ret &= are_connected(*(vfrom.lock()), *(vto.lock()));
    }
  }

  return ret;
}

void DAG::linear_traversal(std::function<void(std::shared_ptr<DAGVertex>)> cb) {
  std::for_each(graph_.begin(), graph_.end(), cb);
}

std::size_t DAG::vertex_count() const { return graph_.size(); }

std::size_t DAG::edge_count() const {
  std::size_t ret = 0;

  (const_cast<DAG *>(this))
      ->linear_traversal(
          [&](std::shared_ptr<DAGVertex> v) { ret += v->edge_count(); });

  return ret;
}

const std::string &DAG::title() const { return title_; }

bool DAG::remove_vertex(const DAGVertex &v) {
  bool ret = false;

  graph_.erase(
      std::remove_if(graph_.begin(), graph_.end(),
                     [&](std::shared_ptr<DAGVertex> o) {
                       bool found = ((o.get() != nullptr) && (*(o.get()) == v));
                       if (found) {
                         o->visit_all_edges([&](const DAGEdge &e) {
                           if (e.connection_.lock() != nullptr) {
                             e.connection_.lock()->sub_incomming_edge();
                           }
                         });
                         o->clear_edges();
                         ret = true;
                       }
                       return found;
                     }),
      graph_.end());

  return ret;
}

bool DAG::remove_vertex_by_uuid(const UUID &id) {
  bool ret = false;

  std::vector<std::shared_ptr<DAGVertex>>::iterator remove_it = std::remove_if(
      graph_.begin(), graph_.end(), [&](std::shared_ptr<DAGVertex> o) {
        bool found = ((o.get() != nullptr) && (o->get_uuid() == id));
        if (found) {
          o->visit_all_edges([&](const DAGEdge &e) {
            if (e.connection_.lock() != nullptr) {
              e.connection_.lock()->sub_incomming_edge();
            }
          });
          o->clear_edges();
          ret = true;
        }
        return found;
      });
  graph_.erase(remove_it, graph_.end());

  return ret;
}

bool DAG::remove_all_vertex_with_label(const std::string &label) {
  bool ret = false;

  std::vector<std::shared_ptr<DAGVertex>> found_with_label;

  std::copy_if(graph_.begin(), graph_.end(),
               std::back_inserter(found_with_label),
               [&](std::shared_ptr<DAGVertex> v) {
                 return (v.get() && (v->label() == label));
               });

  ret = !(found_with_label.empty());

  if (ret) {
    std::for_each(found_with_label.begin(), found_with_label.end(),
                  [&](std::shared_ptr<DAGVertex> v) {
                    if (v != nullptr) {
                      remove_vertex(*v);
                    }
                  });
  }

  return ret;
}

void DAG::reset() { graph_.clear(); }

const rapidjson::Document &DAG::json_config() const { return (*json_config_); }

void DAG::json_config_str(std::string &out_str) const {
  if (json_config_) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json_config_->Accept(writer);
    out_str = buffer.GetString();
    if (out_str == "null") {
      out_str = "{}";
    }
  }
}

void DAG::override_initial_input_for_vertex_task(
    const UUID &vertex_uuid, const rapidjson::Document &initial_input) {
  std::shared_ptr<DAGVertex> vertex_to_update =
      find_vertex_by_uuid(vertex_uuid).lock();
  if (vertex_to_update->get_task()) {
    Logging::info(LOG_TAG, "Going to update inputs for",
                  *(vertex_to_update->get_task().get()), "...");
    const UUID &task_uuid = vertex_to_update->get_task()->get_uuid();
    UUID clone(task_uuid.as_string());
    auto uuid_ptr = std::make_unique<UUID>(task_uuid.as_string());

    rapidjson::Document json_input;
    json_input.CopyFrom(initial_input, json_input.GetAllocator());

    if (vertex_to_update->get_task()) {
      // TODO (mehoggan): Implement this method in task.h/cxx
      throw std::logic_error(__FUNCTION__ + std::string(" not implemented."));
      // vertex_to_update->override_inputs();
    }
  }
}

std::ostream &operator<<(std::ostream &out, const DAG &g) {
  out << "Title: \"" << g.title_ << "\"";
  if (not g.graph_.empty()) {
    out << std::endl;
  }
  for (std::shared_ptr<DAGVertex> v : g.graph_) {
    out << (*v) << std::endl;
  }
  std::string json_config_string;
  g.json_config_str(json_config_string);
  out << std::endl << "Configuration: " << json_config_string << std::endl;

  return out;
}

bool operator==(const DAG &lhs, const DAG &rhs) {
  bool ret = true;

  ret &= (lhs.graph_.size() == rhs.graph_.size());

  DAG lhs_clone = (*const_cast<DAG *>(&lhs)).clone();
  DAG rhs_clone = (*const_cast<DAG *>(&rhs)).clone();

  std::sort(lhs_clone.graph_.begin(), lhs_clone.graph_.end(),
            [](std::shared_ptr<DAGVertex> a, std::shared_ptr<DAGVertex> b) {
              return a->label() < b->label();
            });
  std::sort(rhs_clone.graph_.begin(), rhs_clone.graph_.end(),
            [](std::shared_ptr<DAGVertex> a, std::shared_ptr<DAGVertex> b) {
              return a->label() < b->label();
            });

  std::size_t index = 0;
  for (auto v : lhs.graph_) {
    ret &= ((*v) == (*(rhs.graph_[index])));
    auto o = rhs.graph_[index];  // Force use count up.
    ret &= (v.use_count() == o.use_count());

    if (!ret) {
      break;
    }

    ++index;
  }
  ret &= (lhs.vertex_count() == rhs.vertex_count());
  ret &= (lhs.edge_count() == rhs.edge_count());
  ret &= (lhs.json_config() == rhs.json_config());

  return ret;
}

bool operator!=(const DAG &lhs, const DAG &rhs) { return !(lhs == rhs); }

std::shared_ptr<DAGVertex> DAG::get_vertex_at(std::size_t i) {
  assert(i < graph_.size() && "Index out of bounds.");
  return graph_[i];
}

void DAG::clone_connections(DAGVertex &from, DAGVertex &to) {
  assert(from.get_uuid() == to.get_uuid() &&
         "Cloning connections on dag_vertices that are not the same is not "
         "permitted.");

  std::vector<DAGVertex::DAGVertex_connection> from_connections =
      from.clone_all_connections();

  for (auto &connection : from_connections) {
    std::weak_ptr<DAGVertex> find = find_vertex(connection.vertex());
    assert(!find.expired() && "This should never happen.");
    to.connect(find.lock());
  }
}

DAG::DAG(const DAG &other)
    : LoggedClass(*this),
      json_config_(std::make_unique<rapidjson::Document>()) {
  DAG *o = (const_cast<DAG *>(&other));
  o->linear_traversal([&](std::shared_ptr<DAGVertex> v) {
    DAGVertex tmp = v->clone();
    add_vertex(std::move(tmp));
  });

  for (std::size_t i = 0; i < graph_.size(); ++i) {
    clone_connections(*other.graph_[i], *graph_[i]);
  }
  title_ = other.title_;
  json_config_->CopyFrom((*other.json_config_), json_config_->GetAllocator());
}

DAG &DAG::operator=(const DAG &rhs) {
  DAG &o = *(const_cast<DAG *>(&rhs));
  o.linear_traversal([&](std::shared_ptr<DAGVertex> v) {
    DAGVertex tmp = v->clone();
    add_vertex(std::move(tmp));
  });

  for (std::size_t i = 0; i < graph_.size(); ++i) {
    clone_connections(*rhs.graph_[i], *graph_[i]);
  }

  title_ = rhs.title_;
  json_config_->CopyFrom((*rhs.json_config_), json_config_->GetAllocator());
  Logging::info(LOG_TAG, "Assigned DAG with title=", title_);

  return (*this);
}
}  // namespace dag_scheduler
}  // namespace com
