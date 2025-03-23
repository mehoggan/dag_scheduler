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
DAGEdge::DAGEdge() : current_status_(Status::initialized) {}

DAGEdge::~DAGEdge() {
    current_status_ = Status::non_traversable;
    connection_.reset();
}

DAGEdge::DAGEdge(DAGEdge&& other) {
    uuid_ = std::move(other.uuid_);
    current_status_ = other.current_status_;
    connection_ = std::move(other.connection_);
    other.current_status_ = Status::non_traversable;
}

DAGEdge& DAGEdge::operator=(DAGEdge&& rhs) {
    uuid_ = std::move(rhs.uuid_);
    current_status_ = rhs.current_status_;
    connection_ = std::move(rhs.connection_);
    rhs.current_status_ = Status::non_traversable;

    return (*this);
}

DAGEdge DAGEdge::clone() { return (*this); }

std::weak_ptr<DAGVertex> DAGEdge::get_connection() { return connection_; }

bool DAGEdge::connect_to(std::shared_ptr<DAGVertex> v) {
    bool ret = true;

    if (!connection_.expired()) {
        connection_.lock()->sub_incoming_edge();
        ret = false;
    }

    connection_ = v;
    if (!connection_.expired()) {
        connection_.lock()->add_incoming_edge();
    }

    return ret;
}

bool DAGEdge::is_a_connection_to(const DAGVertex& v) const {
    bool ret = false;

    if (!connection_.expired() && (v == (*(connection_.lock())))) {
        ret = true;
    }

    return ret;
}

const UUID& DAGEdge::get_uuid() const { return uuid_; }

const DAGEdge::Status& DAGEdge::current_status() const {
    return current_status_;
}

std::string DAGEdge::current_status_as_string() const {
    std::string ret;

    switch (current_status_) {
        case Status::initialized: {
            ret = "initialized";
        } break;
        case Status::traversed: {
            ret = "traversed";
        } break;
        case Status::non_traversable: {
            ret = "non_traversable";
        } break;
    }

    return ret;
}

DAGEdge::DAGEdge(const DAGEdge& other)
        : uuid_(const_cast<DAGEdge*>(&other)->uuid_.clone())
        , current_status_(other.current_status())
        , connection_(/*We cannot connect because we do NOT own.*/) {}

DAGEdge& DAGEdge::operator=(const DAGEdge& rhs) {
    uuid_ = const_cast<DAGEdge*>(&rhs)->uuid_.clone();
    current_status_ = rhs.current_status();
    connection_.reset(/*We cannot connect because we do NOT own.*/);

    return (*this);
}

std::ostream& operator<<(std::ostream& out, const std::nullptr_t n) {
    out << "nullptr";

    return out;
}

std::ostream& operator<<(std::ostream& out, const DAGEdge& e) {
    out << "uuid_ = " << e.uuid_ << " current_status_ = "
        << "(" << e.current_status_as_string() << ") connection = ";

    if (e.connection_.lock() == nullptr) {
        out << "nullptr";
    } else {
        out << (*(e.connection_.lock()));
    }

    return out;
}

bool operator==(const DAGEdge& lhs, const DAGEdge& rhs) {
    bool ret = true;

    ret &= (lhs.uuid_.as_string() == rhs.uuid_.as_string());
    ret &= (lhs.current_status_ == rhs.current_status_);

    std::shared_ptr<DAGVertex> lhs_connection = lhs.connection_.lock();
    std::shared_ptr<DAGVertex> rhs_connection = rhs.connection_.lock();

    if (lhs_connection != nullptr && rhs_connection != nullptr) {
        ret &= (lhs.connection_.lock().use_count() ==
                rhs.connection_.lock().use_count());
        // bool objs_are_same = (*lhs_connection) == (*rhs_connection);
        // ret &= objs_are_same;
    } else if (lhs_connection == nullptr && rhs_connection == nullptr) {
        ret &= true;
    } else {
        ret &= false;
    }

    return ret;
}

bool operator!=(const DAGEdge& lhs, const DAGEdge& rhs) {
    bool ret = !(lhs == rhs);

    return ret;
}
}  // namespace com::dag_scheduler
