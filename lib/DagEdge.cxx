////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "dag_scheduler/DagEdge.h"

#include <iostream>

namespace com::dag_scheduler {
DAGEdge::DAGEdge() : current_status_(Status::INITIALIZED) {}

DAGEdge::~DAGEdge() {
    current_status_ = Status::NON_TRAVERSABLE;
    connection_.reset();
}

DAGEdge::DAGEdge(DAGEdge&& other) {
    uuid_ = std::move(other.uuid_);
    current_status_ = other.current_status_;
    connection_ = std::move(other.connection_);
    other.current_status_ = Status::NON_TRAVERSABLE;
}

DAGEdge& DAGEdge::operator=(DAGEdge&& rhs) {
    uuid_ = std::move(rhs.uuid_);
    current_status_ = rhs.current_status_;
    connection_ = std::move(rhs.connection_);
    rhs.current_status_ = Status::NON_TRAVERSABLE;

    return (*this);
}

DAGEdge DAGEdge::clone() { return (*this); }

std::weak_ptr<DAGVertex> DAGEdge::getConnection() { return connection_; }

bool DAGEdge::connectTo(std::shared_ptr<DAGVertex> vertex) {
    bool ret_val = true;

    if (!connection_.expired()) {
        connection_.lock()->subIncomingEdge();
        ret_val = false;
    }

    connection_ = vertex;
    if (!connection_.expired()) {
        connection_.lock()->addIncomingEdge();
    }

    return ret_val;
}

bool DAGEdge::isAConnectionTo(const DAGVertex& vertex) const {
    bool ret_val = false;

    if (!connection_.expired() && (vertex == (*(connection_.lock())))) {
        ret_val = true;
    }

    return ret_val;
}

const UUID& DAGEdge::getUUID() const { return uuid_; }

const DAGEdge::Status& DAGEdge::currentStatus() const {
    return current_status_;
}

std::string DAGEdge::currentStatusAsString() const {
    std::string ret_val;

    switch (current_status_) {
        case Status::INITIALIZED: {
            ret_val = "initialized";
        } break;
        case Status::TRAVERSED: {
            ret_val = "traversed";
        } break;
        case Status::NON_TRAVERSABLE: {
            ret_val = "non_traversable";
        } break;
    }

    return ret_val;
}

DAGEdge::DAGEdge(const DAGEdge& other)
        : uuid_(const_cast<DAGEdge*>(&other)->uuid_.clone())
        , current_status_(other.currentStatus())
        , connection_(/*We cannot connect because we do NOT own.*/) {}

DAGEdge& DAGEdge::operator=(const DAGEdge& rhs) {
    uuid_ = const_cast<DAGEdge*>(&rhs)->uuid_.clone();
    current_status_ = rhs.currentStatus();
    connection_.reset(/*We cannot connect because we do NOT own.*/);

    return (*this);
}

std::ostream& operator<<(std::ostream& out, const std::nullptr_t n) {
    out << "nullptr";

    return out;
}

std::ostream& operator<<(std::ostream& out, const DAGEdge& edge) {
    out << "uuid_ = " << edge.uuid_ << " current_status_ = " << "("
        << edge.currentStatusAsString() << ") connection = ";

    if (edge.connection_.lock() == nullptr) {
        out << "nullptr";
    } else {
        out << (*(edge.connection_.lock()));
    }

    return out;
}

bool operator==(const DAGEdge& lhs, const DAGEdge& rhs) {
    bool ret_val = true;

    ret_val &= (lhs.uuid_.asString() == rhs.uuid_.asString());
    ret_val &= (lhs.current_status_ == rhs.current_status_);

    std::shared_ptr<DAGVertex> lhs_connection = lhs.connection_.lock();
    std::shared_ptr<DAGVertex> rhs_connection = rhs.connection_.lock();

    if (lhs_connection != nullptr && rhs_connection != nullptr) {
        ret_val &= (lhs.connection_.lock().use_count() ==
                    rhs.connection_.lock().use_count());
        // bool objs_are_same = (*lhs_connection) == (*rhs_connection);
        // ret &= objs_are_same;
    } else if (lhs_connection == nullptr && rhs_connection == nullptr) {
        ret_val &= true;
    } else {
        ret_val &= false;
    }

    return ret_val;
}

bool operator!=(const DAGEdge& lhs, const DAGEdge& rhs) {
    bool ret_val = !(lhs == rhs);

    return ret_val;
}
}  // namespace com::dag_scheduler
