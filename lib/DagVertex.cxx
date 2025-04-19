////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "dag_scheduler/DagVertex.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "dag_scheduler/DagEdge.h"

namespace com::dag_scheduler {
DAGVertex::DAGVertexConnection::DAGVertexConnection() {}

DAGVertex::DAGVertexConnection::DAGVertexConnection(DAGEdge& edge,
                                                    DAGVertex& vertex)
        : edge_(new DAGEdge(edge.clone()))
        , vertex_(new DAGVertex(vertex.clone())) {}

const DAGEdge& DAGVertex::DAGVertexConnection::edge() const { return *edge_; }

const DAGVertex& DAGVertex::DAGVertexConnection::vertex() const {
    return *vertex_;
}

DAGVertex::DAGVertex()
        : current_status_(Status::INITIALIZED)
        , label_(uuid_.asString())
        , incoming_edge_count_(0) {}

DAGVertex::DAGVertex(const std::string& label)
        : current_status_(Status::INITIALIZED)
        , label_(label)
        , incoming_edge_count_(0) {}

DAGVertex::DAGVertex(const std::string& label, std::unique_ptr<Task>&& task)
        : current_status_(Status::INITIALIZED)
        , label_(label)
        , incoming_edge_count_(0)
        , task_(std::move(task)) {}

DAGVertex::DAGVertex(const std::string& label,
                     std::unique_ptr<Task>&& task,
                     UUID&& uuid)
        : uuid_(std::move(uuid))
        , current_status_(Status::INITIALIZED)
        , label_(label)
        , incoming_edge_count_(0)
        , task_(std::move(task)) {}

DAGVertex::~DAGVertex() {
    current_status_ = Status::INVALID;
    label_.clear();
    clearEdges();
    incoming_edge_count_.store(0);
}

DAGVertex::DAGVertex(DAGVertex&& other) {
    uuid_ = std::move(other.uuid_);
    label_ = std::move(other.label_);
    current_status_ = other.current_status_;
    edges_ = std::move(other.edges_);
    incoming_edge_count_ = other.incoming_edge_count_.load();
    task_ = std::move(other.task_);

    other.label_.clear();
    other.current_status_ = Status::INVALID;
    other.incoming_edge_count_.store(0ul);
}

DAGVertex& DAGVertex::operator=(DAGVertex&& rhs) {
    uuid_ = std::move(rhs.uuid_);
    label_ = std::move(rhs.label_);
    current_status_ = rhs.current_status_;
    edges_ = std::move(rhs.edges_);
    incoming_edge_count_ = rhs.incoming_edge_count_.load();
    task_ = std::move(rhs.task_);

    rhs.label_.clear();
    rhs.current_status_ = Status::INVALID;
    rhs.incoming_edge_count_.store(0ul);

    return (*this);
}

DAGVertex DAGVertex::clone() { return (*this); }

bool DAGVertex::connect(std::shared_ptr<DAGVertex> other) {
    bool ret_val = false;

    if (!containsConnectionTo(*other)) {
        std::unique_ptr<DAGEdge> edge(new DAGEdge);
        edge->connectTo(other);
        edges_.push_back(std::move(edge));
        ret_val = true;
        assert(!edges_.empty());
        assert(edges_.back()->isAConnectionTo(*other));
    }

    return ret_val;
}

bool DAGVertex::containsConnectionTo(const DAGVertex& other) {
    bool ret_val = false;

    for (const auto& ptrref : edges_) {
        if (ptrref->isAConnectionTo(other)) {
            ret_val = true;
            break;
        }
    }

    return ret_val;
}

std::vector<std::shared_ptr<DAGVertex>> DAGVertex::reestablishConnections(
        std::vector<DAGVertexConnection>& connections) {
    std::vector<std::shared_ptr<DAGVertex>> ret_val;
    ret_val.reserve(connections.size());

    for (auto& connection : connections) {
        DAGVertex vertex = *(const_cast<DAGVertex*>(&connection.vertex()));
        std::shared_ptr<DAGVertex> other =
                std::make_shared<DAGVertex>(vertex.clone());
        DAGEdge edge = *(const_cast<DAGEdge*>(&connection.edge()));
        std::unique_ptr<DAGEdge> tmp_edge(new DAGEdge(edge.clone()));
        tmp_edge->connectTo(other);
        // We need to preserve the shared_ptr so the edge's weak_ptr does
        // not expire.
        ret_val.push_back(other);
        edges_.push_back(std::move(tmp_edge));
    }

    return ret_val;
}

std::size_t DAGVertex::edgeCount() const { return edges_.size(); }

void DAGVertex::visitAllEdges(
        std::function<void(const DAGEdge&)> call_back) const {
    std::size_t edge_count = edges_.size();
    (void)edge_count;

    std::for_each(
            edges_.begin(),
            edges_.end(),
            [&](const std::unique_ptr<DAGEdge>& edge) { call_back(*edge); });
}

std::vector<DAGVertex::DAGVertexConnection> DAGVertex::cloneAllConnections() {
    std::vector<DAGVertexConnection> ret_val;
    ret_val.reserve(edges_.size());

    for (std::unique_ptr<DAGEdge>& edge : edges_) {
        DAGEdge e_clone = edge->clone();
        DAGVertex v_clone = edge->getConnection().lock()->clone();
        ret_val.push_back(DAGVertexConnection(e_clone, v_clone));
        DAGVertex& tmp_vertex =
                *const_cast<DAGVertex*>(&ret_val.back().vertex());
        tmp_vertex.resetIncomingEdgeCount();
        assert(ret_val.back().vertex().incomingEdgeCount() == 0 &&
               "Reseting edge count failed.");
    }

    return ret_val;
}

const UUID& DAGVertex::getUUID() const { return uuid_; }

const DAGVertex::Status& DAGVertex::currentStatus() const {
    return current_status_;
}

std::string DAGVertex::currentStatusAsString() const {
    std::string ret_val;

    switch (current_status_) {
        case Status::INITIALIZED: {
            ret_val = "initialized";
        } break;
        case Status::SCHEDULED: {
            ret_val = "scheduled";
        } break;
        case Status::RUNNING: {
            ret_val = "running";
        } break;
        case Status::PASSED: {
            ret_val = "passed";
        } break;
        case Status::FAILED: {
            ret_val = "failed";
        } break;
        case Status::INVALID: {
            ret_val = "invalid";
        } break;
    }

    return ret_val;
}

const std::string& DAGVertex::label() const { return label_; }

std::unique_ptr<Task>& DAGVertex::getTask() { return task_; }

bool DAGVertex::hasIncomingEdges() const { return (incoming_edge_count_ > 0); }

std::size_t DAGVertex::incomingEdgeCount() const {
    return incoming_edge_count_;
}

void DAGVertex::addIncomingEdge() { ++incoming_edge_count_; }

void DAGVertex::subIncomingEdge() { --incoming_edge_count_; }

void DAGVertex::clearEdges() { edges_.clear(); }

void DAGVertex::resetIncomingEdgeCount() { incoming_edge_count_.store(0); }

const DAGEdge& DAGVertex::getEdgeAt(std::size_t index) const {
    assert(index < edges_.size() && "Index out of bounds.");
    return *(edges_[index]);
}

DAGVertex::DAGVertex(const DAGVertex& other)
        : uuid_(const_cast<DAGVertex*>(&other)->uuid_.clone())
        , current_status_(other.current_status_)
        , label_(other.label()) {
    if (other.task_) {
        task_ = other.task_->clone();
    }
    resetIncomingEdgeCount();
    // We cannot add back the connections since the edge adds a weak_ptr
    // to a DAGVertex we no longer can duplicate. This has to be done
    // outside the class by the code that is cloning the DAGVertex.
    // DAG_t should be the object that orchestrates that.
}

DAGVertex& DAGVertex::operator=(const DAGVertex& rhs) {
    DAGVertex& vertex_ref = *(const_cast<DAGVertex*>(&rhs));
    uuid_ = vertex_ref.uuid_.clone();
    current_status_ = vertex_ref.current_status_;
    label_ = rhs.label();
    if (rhs.task_) {
        task_ = rhs.task_->clone();
    }
    resetIncomingEdgeCount();
    // We cannot add back the connections since the edge adds a weak_ptr
    // to a DAGVertex we no longer can duplicate. This has to be done
    // outside the class by the code that is cloning the DAGVertex.
    // DAG_t should be the object that orchestrates that.
    return (*this);
}

std::ostream& operator<<(std::ostream& out, const DAGVertex& vertex) {
    out << "uuid_ = " << vertex.uuid_
        << " current_status_ = " << vertex.currentStatusAsString()
        << " label = " << vertex.label_ << " "
        << "incoming_edge_count = " << vertex.incoming_edge_count_ << " "
        << " edges = " << std::endl
        << std::endl
        << "edges(" << vertex.edgeCount() << "): ";
    vertex.visitAllEdges(
            [&](const DAGEdge& edge) { out << "\t" << edge << std::endl; });

    const std::unique_ptr<Task>& task_ptr =
            const_cast<DAGVertex&>(vertex).getTask();
    out << "task_ = ";
    if (task_ptr) {
        out << (*task_ptr);
    } else {
        out << "nullptr";
    }

    return out;
}

bool operator==(const DAGVertex& lhs, const DAGVertex& rhs) {
    bool ret_val = true;

    if (lhs.current_status_ == rhs.current_status_) {
        auto lhs_str = lhs.uuid_.asString();
        auto rhs_str = rhs.uuid_.asString();

        ret_val &= (lhs_str == rhs_str);
        ret_val &= (lhs.label() == rhs.label());

        std::size_t lhs_edge_count = lhs.edgeCount();
        std::size_t rhs_edge_count = rhs.edgeCount();
        ret_val &= (lhs_edge_count == rhs_edge_count);
        ret_val &= (lhs.incomingEdgeCount() == rhs.incomingEdgeCount());
        /*
         * We omit comparision of edges since a vertex checks its edges
         * in equality which would lead to infinite recursion.
         */
    } else if (lhs.current_status_ == DAGVertex::Status::INVALID &&
               rhs.current_status_ == DAGVertex::Status::INVALID) {
        ret_val = true;
    } else {
        ret_val = false;
    }

    return ret_val;
}

bool operator!=(const DAGVertex& lhs, const DAGVertex& rhs) {
    return !(lhs == rhs);
}
}  // namespace com::dag_scheduler
