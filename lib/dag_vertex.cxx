#include "dag_scheduler/dag_vertex.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "dag_scheduler/dag_edge.h"

namespace com {
namespace dag_scheduler {
DAGVertex::DAGVertex_connection::DAGVertex_connection() {}

DAGVertex::DAGVertex_connection::DAGVertex_connection(DAGEdge &edge,
                                                      DAGVertex &vertex)
    : edge_(new DAGEdge(edge.clone())),
      vertex_(new DAGVertex(vertex.clone())) {}

const DAGEdge &DAGVertex::DAGVertex_connection::edge() const { return *edge_; }

const DAGVertex &DAGVertex::DAGVertex_connection::vertex() const {
  return *vertex_;
}

DAGVertex::DAGVertex()
    : current_status_(Status::INITIALIZED),
      label_(uuid_.as_string()),
      incomming_edge_count_(0) {}

DAGVertex::DAGVertex(const std::string &label)
    : current_status_(Status::INITIALIZED),
      label_(label),
      incomming_edge_count_(0) {}

DAGVertex::DAGVertex(const std::string &label, std::unique_ptr<Task> &&task)
    : current_status_(Status::INITIALIZED),
      label_(label),
      incomming_edge_count_(0),
      task_(std::move(task)) {}

DAGVertex::DAGVertex(const std::string &label, std::unique_ptr<Task> &&task,
                     UUID &&uuid)
    : uuid_(std::move(uuid)),
      current_status_(Status::INITIALIZED),
      label_(label),
      incomming_edge_count_(0),
      task_(std::move(task)) {}

DAGVertex::~DAGVertex() {
  current_status_ = Status::INVALID;
  label_.clear();
  clear_edges();
  incomming_edge_count_.store(0);
}

DAGVertex::DAGVertex(DAGVertex &&other) {
  uuid_ = std::move(other.uuid_);
  label_ = std::move(other.label_);
  current_status_ = other.current_status_;
  edges_ = std::move(other.edges_);
  incomming_edge_count_ = other.incomming_edge_count_.load();
  task_ = std::move(other.task_);

  other.label_.clear();
  other.current_status_ = Status::INVALID;
  other.incomming_edge_count_.store(0ul);
}

DAGVertex &DAGVertex::operator=(DAGVertex &&rhs) {
  uuid_ = std::move(rhs.uuid_);
  label_ = std::move(rhs.label_);
  current_status_ = rhs.current_status_;
  edges_ = std::move(rhs.edges_);
  incomming_edge_count_ = rhs.incomming_edge_count_.load();
  task_ = std::move(rhs.task_);

  rhs.label_.clear();
  rhs.current_status_ = Status::INVALID;
  rhs.incomming_edge_count_.store(0ul);

  return (*this);
}

DAGVertex DAGVertex::clone() { return (*this); }

bool DAGVertex::connect(std::shared_ptr<DAGVertex> other) {
  bool ret = false;

  if (!contains_connection_to(*other)) {
    std::unique_ptr<DAGEdge> e(new DAGEdge);
    e->connect_to(other);
    edges_.push_back(std::move(e));
    ret = true;
    assert(!edges_.empty());
    assert(edges_.back()->is_a_connection_to(*other));
  }

  return ret;
}

bool DAGVertex::contains_connection_to(const DAGVertex &other) {
  bool ret = false;

  for (const auto &ptrref : edges_) {
    if (ptrref->is_a_connection_to(other)) {
      ret = true;
      break;
    }
  }

  return ret;
}

std::vector<std::shared_ptr<DAGVertex>> DAGVertex::restablish_connections(
    std::vector<DAGVertex_connection> &connections) {
  std::vector<std::shared_ptr<DAGVertex>> ret;
  ret.reserve(connections.size());

  for (auto &connection : connections) {
    DAGVertex vertex = *(const_cast<DAGVertex *>(&connection.vertex()));
    std::shared_ptr<DAGVertex> other =
        std::make_shared<DAGVertex>(vertex.clone());
    DAGEdge edge = *(const_cast<DAGEdge *>(&connection.edge()));
    std::unique_ptr<DAGEdge> tmp_edge(new DAGEdge(edge.clone()));
    tmp_edge->connect_to(other);
    // We need to preserve the shared_ptr so the edge's weak_ptr does
    // not expire.
    ret.push_back(other);
    edges_.push_back(std::move(tmp_edge));
  }

  return ret;
}

std::size_t DAGVertex::edge_count() const { return edges_.size(); }

void DAGVertex::visit_all_edges(std::function<void(const DAGEdge &)> cb) const {
  std::size_t edge_count = edges_.size();
  (void)edge_count;

  std::for_each(edges_.begin(), edges_.end(),
                [&](const std::unique_ptr<DAGEdge> &e) { cb(*e); });
}

std::vector<DAGVertex::DAGVertex_connection>
DAGVertex::clone_all_connections() {
  std::vector<DAGVertex_connection> ret;
  ret.reserve(edges_.size());

  for (std::unique_ptr<DAGEdge> &e : edges_) {
    DAGEdge e_clone = e->clone();
    DAGVertex v_clone = e->get_connection().lock()->clone();
    ret.push_back(DAGVertex_connection(e_clone, v_clone));
    DAGVertex &tmp = *const_cast<DAGVertex *>(&ret.back().vertex());
    tmp.reset_incomming_edge_count();
    assert(ret.back().vertex().incomming_edge_count() == 0 &&
           "Reseting edge count failed.");
  }

  return ret;
}

const UUID &DAGVertex::get_uuid() const { return uuid_; }

const DAGVertex::Status &DAGVertex::current_status() const {
  return current_status_;
}

std::string DAGVertex::current_status_as_string() const {
  std::string ret;

  switch (current_status_) {
    case Status::INITIALIZED: {
      ret = "initialized";
    } break;
    case Status::SCHEDULED: {
      ret = "scheduled";
    } break;
    case Status::RUNNING: {
      ret = "running";
    } break;
    case Status::PASSED: {
      ret = "passed";
    } break;
    case Status::FAILED: {
      ret = "failed";
    } break;
    case Status::INVALID: {
      ret = "invalid";
    } break;
  }

  return ret;
}

const std::string &DAGVertex::label() const { return label_; }

std::unique_ptr<Task> &DAGVertex::get_task() { return task_; }

bool DAGVertex::has_incomming_edges() const {
  return (incomming_edge_count_ > 0);
}

std::size_t DAGVertex::incomming_edge_count() const {
  return incomming_edge_count_;
}

void DAGVertex::add_incomming_edge() { ++incomming_edge_count_; }

void DAGVertex::sub_incomming_edge() { --incomming_edge_count_; }

void DAGVertex::clear_edges() { edges_.clear(); }

void DAGVertex::reset_incomming_edge_count() { incomming_edge_count_.store(0); }

const DAGEdge &DAGVertex::get_edge_at(std::size_t i) const {
  assert(i < edges_.size() && "Index out of bounds.");
  return *(edges_[i]);
}

DAGVertex::DAGVertex(const DAGVertex &other)
    : uuid_(const_cast<DAGVertex *>(&other)->uuid_.clone()),
      current_status_(other.current_status_),
      label_(other.label()) {
  if (other.task_) {
    task_ = other.task_->clone();
  }
  reset_incomming_edge_count();
  // We cannot add back the connections since the edge adds a weak_ptr
  // to a DAGVertex we no longer can duplicate. This has to be done
  // outside the class by the code that is cloning the DAGVertex.
  // DAG_t should be the object that orchestrates that.
}

DAGVertex &DAGVertex::operator=(const DAGVertex &rhs) {
  DAGVertex &t = *(const_cast<DAGVertex *>(&rhs));
  uuid_ = t.uuid_.clone();
  current_status_ = t.current_status_;
  label_ = rhs.label();
  if (rhs.task_) {
    task_ = rhs.task_->clone();
  }
  reset_incomming_edge_count();
  // We cannot add back the connections since the edge adds a weak_ptr
  // to a DAGVertex we no longer can duplicate. This has to be done
  // outside the class by the code that is cloning the DAGVertex.
  // DAG_t should be the object that orchestrates that.
  return (*this);
}

std::ostream &operator<<(std::ostream &out, const DAGVertex &v) {
  out << "uuid_ = " << v.uuid_
      << " current_status_ = " << v.current_status_as_string()
      << " label = " << v.label_ << " "
      << "incomming_edge_count = " << v.incomming_edge_count_ << " "
      << " edges = " << std::endl
      << std::endl
      << "edges(" << v.edge_count() << "): ";
  v.visit_all_edges([&](const DAGEdge &e) { out << "\t" << e << std::endl; });

  const std::unique_ptr<Task> &task_ptr = const_cast<DAGVertex &>(v).get_task();
  out << "task_ = ";
  if (task_ptr) {
    out << (*task_ptr);
  } else {
    out << "nullptr";
  }

  return out;
}

bool operator==(const DAGVertex &lhs, const DAGVertex &rhs) {
  bool ret = true;

  if (lhs.current_status_ == rhs.current_status_) {
    auto lhs_str = lhs.uuid_.as_string();
    auto rhs_str = rhs.uuid_.as_string();

    ret &= (lhs_str == rhs_str);
    ret &= (lhs.label() == rhs.label());

    std::size_t lhs_edge_count = lhs.edge_count();
    std::size_t rhs_edge_count = rhs.edge_count();
    ret &= (lhs_edge_count == rhs_edge_count);
    ret &= (lhs.incomming_edge_count() == rhs.incomming_edge_count());
    /*
     * We omit comparision of edges since a vertex checks its edges
     * in equality which would lead to infinite recursion.
     */
  } else if (lhs.current_status_ == DAGVertex::Status::INVALID &&
             rhs.current_status_ == DAGVertex::Status::INVALID) {
    ret = true;
  } else {
    ret = false;
  }

  return ret;
}

bool operator!=(const DAGVertex &lhs, const DAGVertex &rhs) {
  return !(lhs == rhs);
}
}  // namespace dag_scheduler
}  // namespace com
