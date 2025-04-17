////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "dag_scheduler/Dag.h"

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

#include "dag_scheduler/DagAlgorithms.h"
#include "dag_scheduler/DagEdge.h"
#include "dag_scheduler/LoggedClass.hpp"

namespace com::dag_scheduler {
DAG::DAGException::DAGException(const char* message)
        : std::runtime_error(message), what_(message) {}

const char* DAG::DAGException::what() const throw() { return what_.c_str(); }

DAG::DAG() : DAG("") {}

DAG::DAG(const std::string& title)
        : LoggedClass(*this)
        , title_(title)
        , json_config_(std::make_unique<rapidjson::Document>()) {
    Logging::info(LOG_TAG, "Created DAG with title=--", title_, "--");
}

DAG::DAG(const std::string& title, const rapidjson::Document& json_config)
        : LoggedClass(*this)
        , title_(title)
        , json_config_(std::make_unique<rapidjson::Document>()) {
    Logging::info(LOG_TAG, "Created DAG with title=--", title_, "--");
    json_config_->CopyFrom(json_config, json_config_->GetAllocator());
}

DAG::DAG(const rapidjson::Document& json_config)
        : LoggedClass(*this)
        , json_config_(std::make_unique<rapidjson::Document>()) {
    Logging::info(LOG_TAG, "Created DAG with title=--", title_, "--");
    json_config_->CopyFrom(json_config, json_config_->GetAllocator());
}

DAG::~DAG() {}

DAG::DAG(DAG&& other)
        : LoggedClass(*this)
        , graph_(std::move(other.graph_))
        , title_(std::move(other.title_))
        , json_config_(std::move(other.json_config_)) {
    Logging::info(LOG_TAG, "Moved DAG with title=", title_);
}

DAG& DAG::operator=(DAG&& other) {
    graph_ = std::move(other.graph_);
    title_ = other.title_;
    json_config_ = std::move(other.json_config_);
    Logging::info(LOG_TAG, "Moved Assigned DAG with title=", title_);
    return (*this);
}

DAG DAG::clone() { return (*this); }

bool DAG::addVertex(DAGVertex&& vertex) {
    bool ret_val = false;

    if (vertex.getTask() == nullptr) {
        Logging::warn(LOG_TAG, "Adding vertex with no task!!!");
    }

    if (!containsVertex(vertex)) {
        auto graph_vertex = std::make_shared<DAGVertex>(std::move(vertex));
        graph_.push_back(graph_vertex);

        if (graph_.back()->getTask() == nullptr) {
            Logging::warn(LOG_TAG, "Vertex at end of vertices has no task!!!");
        }

        ret_val = true;
    }

    return ret_val;
}

std::weak_ptr<DAGVertex> DAG::findVertex(const DAGVertex& vertex) {
    std::weak_ptr<DAGVertex> ret_val = findVertexByUUID(vertex.getUUID());
    return ret_val;
}

std::weak_ptr<DAGVertex> DAG::findVertexByUUID(const UUID& uuid) {
    std::weak_ptr<DAGVertex> ret_val;

    auto graph_it = std::find_if(graph_.begin(),
                                 graph_.end(),
                                 [&](std::shared_ptr<DAGVertex> vertex_i) {
                                     const DAGVertex& right = (*vertex_i);

                                     std::string u_str = uuid.asString();
                                     std::string rhs_str =
                                             right.getUUID().asString();
                                     (void)u_str;
                                     (void)rhs_str;

                                     return (uuid == right.getUUID());
                                 });

    if (graph_it != graph_.end()) {
        ret_val = (*graph_it);
    }

    return ret_val;
}

std::vector<std::weak_ptr<DAGVertex>> DAG::findAllVerticesWithLabel(
        const std::string& label) {
    std::vector<std::weak_ptr<DAGVertex>> ret_val;

    for (auto graph_it = graph_.begin(); graph_it != graph_.end(); ++graph_it) {
        if ((*graph_it)->label() == label) {
            std::weak_ptr<DAGVertex> tmp_v = (*graph_it);
            ret_val.push_back(tmp_v);
        }
    }

    return ret_val;
}

bool DAG::containsVertex(const DAGVertex& vertex) {
    bool ret_val = false;

    if (!findVertex(vertex).expired()) {
        ret_val = true;
    }

    return ret_val;
}

bool DAG::containsVertexByUUID(const UUID& uuid) {
    bool ret_val = false;

    if (!findVertexByUUID(uuid).expired()) {
        ret_val = true;
    }

    return ret_val;
}

bool DAG::containsVertexByLabel(const std::string& label) {
    bool ret_val = false;

    if (!findAllVerticesWithLabel(label).empty()) {
        ret_val = true;
    }

    return ret_val;
}

bool DAG::connectionWouldMakeCyclic(const DAGVertex& vertex1,
                                    const DAGVertex& vertex2) {
    bool ret_val = false;

    std::list<DAGVertex> sorted_vertices;
    DAG graph_clone = clone();

    DAGVertex v1_clone = *const_cast<DAGVertex*>(&vertex1);
    DAGVertex v2_clone = *const_cast<DAGVertex*>(&vertex2);
    graph_clone.addVertex(std::move(v1_clone));
    graph_clone.addVertex(std::move(v2_clone));
    std::weak_ptr<DAGVertex> v1_found = graph_clone.findVertex(v1_clone);
    std::weak_ptr<DAGVertex> v2_found = graph_clone.findVertex(v2_clone);
    // Bypass DAG interface to prevent infinite recursion.
    v1_found.lock()->connect(v2_found.lock());

    assert((!v1_found.expired() && !v2_found.expired()) &&
           "There must be a regression in add_vertex.");

    ret_val = dagTopologicalSort(graph_clone, sorted_vertices);

    return ret_val;
}

bool DAG::connectionWouldMakeCyclicByUUID(const UUID& uuid1,
                                          const UUID& uuid2) {
    bool ret_val = false;

    std::weak_ptr<DAGVertex> v1_tmp = findVertexByUUID(uuid1);
    std::weak_ptr<DAGVertex> v2_tmp = findVertexByUUID(uuid2);

    ret_val = connectionWouldMakeCyclic(*(v1_tmp.lock()), *(v2_tmp.lock()));

    return ret_val;
}

bool DAG::connectionWouldMakeCyclicByLabel(const std::string& label1,
                                           const std::string& label2) {
    bool ret_val = true;

    std::vector<std::weak_ptr<DAGVertex>> vertex1;
    vertex1 = findAllVerticesWithLabel(label1);
    std::vector<std::weak_ptr<DAGVertex>> vertex2;
    vertex2 = findAllVerticesWithLabel(label2);

    for (const auto& vertex : vertex1) {
        bool good = true;

        for (const auto& uuid : vertex2) {
            ret_val &= (!connectionWouldMakeCyclic(*(vertex.lock()),
                                                   *(uuid.lock())));
            if (!ret_val) {
                good = false;
                break;
            }
        }
        if (!good) {
            break;
        }
    }
    return ret_val;
}

bool DAG::connect(const DAGVertex& vertex1, const DAGVertex& vertex2) {
    bool ret_val = false;

    std::weak_ptr<DAGVertex> v1_tmp = findVertex(vertex1);
    std::weak_ptr<DAGVertex> v2_tmp = findVertex(vertex2);

    if (!v1_tmp.expired() && !v2_tmp.expired()) {
        auto v1_it = std::find(graph_.begin(), graph_.end(), v1_tmp.lock());
        auto v2_it = std::find(graph_.begin(), graph_.end(), v2_tmp.lock());

        if (v1_it != graph_.end() && v2_it != graph_.end()) {
            if (!connectionWouldMakeCyclic(**v1_it, **v2_it)) {
                v1_it->get()->connect(*v2_it);
                ret_val = true;
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

    return ret_val;
}

bool DAG::connectByUUID(const UUID& uuid1, const UUID& uuid2) {
    bool ret_val = false;

    std::weak_ptr<DAGVertex> v1_tmp = findVertexByUUID(uuid1);
    std::weak_ptr<DAGVertex> v2_tmp = findVertexByUUID(uuid2);

    ret_val = connect(*(v1_tmp.lock()), *(v2_tmp.lock()));

    return ret_val;
}

bool DAG::connectAllByLabel(const std::string label1,
                            const std::string label2) {
    bool ret_val = true;

    std::vector<std::weak_ptr<DAGVertex>> vertex1;
    vertex1 = findAllVerticesWithLabel(label1);
    std::vector<std::weak_ptr<DAGVertex>> vertex2;
    vertex2 = findAllVerticesWithLabel(label2);

    for (const auto& vertex_x : vertex1) {
        for (const auto& vertex_u : vertex2) {
            ret_val &= connect(*(vertex_x.lock()), *(vertex_u.lock()));
        }
    }

    return ret_val;
}

bool DAG::addAndConnect(DAGVertex&& vertex1, DAGVertex&& vertex2) {
    bool ret_val = false;

    DAGVertex v1_clone = vertex1.clone();
    DAGVertex v2_clone = vertex2.clone();

    addVertex(std::move(vertex1));
    addVertex(std::move(vertex2));

    std::weak_ptr<DAGVertex> v1_ptr = findVertex(v1_clone);
    std::weak_ptr<DAGVertex> v2_ptr = findVertex(v2_clone);

    ret_val = connect(*(v1_ptr.lock()), *(v2_ptr.lock()));

    return ret_val;
}

bool DAG::areConnected(const DAGVertex& vertex1, const DAGVertex& vertex2) {
    bool ret_val = false;

    std::weak_ptr<DAGVertex> v1_tmp = findVertex(vertex1);
    std::weak_ptr<DAGVertex> v2_tmp = findVertex(vertex2);

    DAGVertex& v2_ref = *(v2_tmp.lock());
    std::uintptr_t v2_addr = reinterpret_cast<std::uintptr_t>(&v2_ref);
    v1_tmp.lock()->visitAllEdges([&](const DAGEdge& edge) {
        DAGEdge& e_tmp = *const_cast<DAGEdge*>(&edge);
        DAGVertex& c_ref = *(e_tmp.getConnection().lock());
        std::uintptr_t c_addr = reinterpret_cast<std::uintptr_t>(&c_ref);
        if (c_addr == v2_addr) {
            ret_val = true;
        }
    });

    return ret_val;
}

bool DAG::areConnectedByUUID(const UUID& uuid1, const UUID& uuid2) {
    bool ret_val = false;

    std::weak_ptr<DAGVertex> v1_tmp = findVertexByUUID(uuid1);
    std::weak_ptr<DAGVertex> v2_tmp = findVertexByUUID(uuid2);

    ret_val = areConnected(*(v1_tmp.lock()), *(v2_tmp.lock()));

    return ret_val;
}

bool DAG::allAreConnectedByLabel(const std::string label1,
                                 const std::string label2) {
    bool ret_val = true;

    std::vector<std::weak_ptr<DAGVertex>> vertex1;
    vertex1 = findAllVerticesWithLabel(label1);
    std::vector<std::weak_ptr<DAGVertex>> vertex2;
    vertex2 = findAllVerticesWithLabel(label2);

    ret_val &= (!vertex1.empty() && !vertex2.empty());

    for (const auto& from_vertex : vertex1) {
        for (const auto& to_vertex : vertex2) {
            ret_val &= areConnected(*(from_vertex.lock()), *(to_vertex.lock()));
        }
    }

    return ret_val;
}

void DAG::linearTraversal(
        std::function<void(std::shared_ptr<DAGVertex>)> call_back) {
    std::for_each(graph_.begin(), graph_.end(), call_back);
}

std::size_t DAG::vertexCount() const { return graph_.size(); }

std::size_t DAG::edgeCount() const {
    std::size_t ret_val = 0;

    (const_cast<DAG*>(this))
            ->linearTraversal([&](std::shared_ptr<DAGVertex> vertex) {
                ret_val += vertex->edgeCount();
            });

    return ret_val;
}

const std::string& DAG::title() const { return title_; }

bool DAG::removeVertex(const DAGVertex& vertex) {
    bool ret_val = false;

    graph_.erase(
            std::remove_if(
                    graph_.begin(),
                    graph_.end(),
                    [&](std::shared_ptr<DAGVertex> i_vertex) {
                        bool found =
                                (i_vertex != nullptr && (*i_vertex == vertex));
                        if (found) {
                            i_vertex->visitAllEdges([&](const DAGEdge& edge) {
                                if (edge.connection_.lock() != nullptr) {
                                    edge.connection_.lock()->subIncomingEdge();
                                }
                            });
                            i_vertex->clearEdges();
                            ret_val = true;
                        }
                        return found;
                    }),
            graph_.end());

    return ret_val;
}

bool DAG::removeVertexByUUID(const UUID& uuid) {
    bool ret_val = false;

    std::vector<std::shared_ptr<DAGVertex>>::iterator remove_it =
            std::remove_if(
                    graph_.begin(),
                    graph_.end(),
                    [&](std::shared_ptr<DAGVertex> i_vertex) {
                        bool found = ((i_vertex != nullptr) &&
                                      (i_vertex->getUUID() == uuid));
                        if (found) {
                            i_vertex->visitAllEdges([&](const DAGEdge& edge) {
                                if (edge.connection_.lock() != nullptr) {
                                    edge.connection_.lock()->subIncomingEdge();
                                }
                            });
                            i_vertex->clearEdges();
                            ret_val = true;
                        }
                        return found;
                    });
    graph_.erase(remove_it, graph_.end());

    return ret_val;
}

bool DAG::removeAllVertexWithLabel(const std::string& label) {
    bool ret_val = false;

    std::vector<std::shared_ptr<DAGVertex>> found_with_label;

    std::copy_if(graph_.begin(),
                 graph_.end(),
                 std::back_inserter(found_with_label),
                 [&](std::shared_ptr<DAGVertex> vertex) {
                     return (vertex.get() && (vertex->label() == label));
                 });

    ret_val = !(found_with_label.empty());

    if (ret_val) {
        std::for_each(found_with_label.begin(),
                      found_with_label.end(),
                      [&](std::shared_ptr<DAGVertex> vertex) {
                          if (vertex != nullptr) {
                              removeVertex(*vertex);
                          }
                      });
    }

    return ret_val;
}

void DAG::reset() { graph_.clear(); }

const rapidjson::Document& DAG::jsonConfig() const { return (*json_config_); }

void DAG::jsonConfigStr(std::string& out_str) const {
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

void DAG::overrideInitialInputForVertexTask(
        const UUID& vertex_uuid, const rapidjson::Document& initial_input) {
    std::shared_ptr<DAGVertex> vertex_to_update =
            findVertexByUUID(vertex_uuid).lock();
    if (vertex_to_update->getTask()) {
        Logging::info(LOG_TAG,
                      "Going to update inputs for",
                      *(vertex_to_update->getTask()),
                      "...");
        const UUID& task_uuid = vertex_to_update->getTask()->getUUID();
        UUID clone(task_uuid.asString());
        auto uuid_ptr = std::make_unique<UUID>(task_uuid.asString());

        rapidjson::Document json_input;
        json_input.CopyFrom(initial_input, json_input.GetAllocator());

        if (vertex_to_update->getTask()) {
            // TODO (mehoggan): Implement this method in // cspell:disable-line
            // task.h/cxx
            throw std::logic_error(__FUNCTION__ +
                                   std::string(" not implemented."));
            // vertex_to_update->override_inputs();
        }
    }
}

std::ostream& operator<<(std::ostream& out, const DAG& graph) {
    out << "Title: \"" << graph.title_ << "\"";
    if (!graph.graph_.empty()) {
        out << std::endl;
    }
    for (const std::shared_ptr<DAGVertex>& vertex : graph.graph_) {
        out << (*vertex) << std::endl;
    }
    std::string json_config_string;
    graph.jsonConfigStr(json_config_string);
    out << std::endl << "Configuration: " << json_config_string << std::endl;

    return out;
}

bool operator==(const DAG& lhs, const DAG& rhs) {
    bool ret_val = true;

    ret_val &= (lhs.graph_.size() == rhs.graph_.size());

    DAG lhs_clone = (*const_cast<DAG*>(&lhs)).clone();
    DAG rhs_clone = (*const_cast<DAG*>(&rhs)).clone();

    std::sort(lhs_clone.graph_.begin(),
              lhs_clone.graph_.end(),
              [](std::shared_ptr<DAGVertex> vertex_a,
                 std::shared_ptr<DAGVertex> vertex_b) {
                  return vertex_a->label() < vertex_b->label();
              });
    std::sort(rhs_clone.graph_.begin(),
              rhs_clone.graph_.end(),
              [](std::shared_ptr<DAGVertex> vertex_a,
                 std::shared_ptr<DAGVertex> vertex_b) {
                  return vertex_a->label() < vertex_b->label();
              });

    std::size_t index = 0;
    for (std::weak_ptr<DAGVertex> vertex : lhs.graph_) {
        ret_val &= ((*vertex.lock()) == (*(rhs.graph_[index])));
        auto other = rhs.graph_[index];  // Force use count up.
        ret_val &= (vertex.lock().use_count() == other.use_count());

        if (!ret_val) {
            break;
        }

        ++index;
    }
    ret_val &= (lhs.vertexCount() == rhs.vertexCount());
    ret_val &= (lhs.edgeCount() == rhs.edgeCount());
    ret_val &= (lhs.jsonConfig() == rhs.jsonConfig());

    return ret_val;
}

bool operator!=(const DAG& lhs, const DAG& rhs) { return !(lhs == rhs); }

std::shared_ptr<DAGVertex> DAG::getVertexAt(std::size_t index) {
    assert(index < graph_.size() && "Index out of bounds.");
    return graph_[index];
}

void DAG::cloneConnections(DAGVertex& from_vertex, DAGVertex& to_vertex) {
    assert(from_vertex.getUUID() == to_vertex.getUUID() &&
           "Cloning connections on dag_vertices that are not the same is not "
           "permitted.");

    std::vector<DAGVertex::DAGVertexConnection> from_connections =
            from_vertex.cloneAllConnections();

    for (auto& connection : from_connections) {
        std::weak_ptr<DAGVertex> find = findVertex(connection.vertex());
        assert(!find.expired() && "This should never happen.");
        to_vertex.connect(find.lock());
    }
}

DAG::DAG(const DAG& other)
        : LoggedClass(*this)
        , json_config_(std::make_unique<rapidjson::Document>()) {
    DAG* other_ptr = (const_cast<DAG*>(&other));
    other_ptr->linearTraversal([&](std::shared_ptr<DAGVertex> vertex) {
        DAGVertex tmp_vertex = vertex->clone();
        addVertex(std::move(tmp_vertex));
    });

    for (std::size_t i = 0; i < graph_.size(); ++i) {
        cloneConnections(*other.graph_[i], *graph_[i]);
    }
    title_ = other.title_;
    json_config_->CopyFrom((*other.json_config_), json_config_->GetAllocator());
}

DAG& DAG::operator=(const DAG& rhs) {
    DAG& other = *(const_cast<DAG*>(&rhs));
    other.linearTraversal([&](std::shared_ptr<DAGVertex> vertex) {
        DAGVertex tmp_vertex = vertex->clone();
        addVertex(std::move(tmp_vertex));
    });

    for (std::size_t i = 0; i < graph_.size(); ++i) {
        cloneConnections(*rhs.graph_[i], *graph_[i]);
    }

    title_ = rhs.title_;
    json_config_->CopyFrom((*rhs.json_config_), json_config_->GetAllocator());
    Logging::info(LOG_TAG, "Assigned DAG with title=", title_);

    return (*this);
}
}  // namespace com::dag_scheduler
