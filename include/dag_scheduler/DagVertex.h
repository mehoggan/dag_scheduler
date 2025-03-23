////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef DAG_VERTEX_H_INCLUDED
#define DAG_VERTEX_H_INCLUDED

#include <gtest/gtest_prod.h>

#include <atomic>
#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "dag_scheduler/Task.h"
#include "dag_scheduler/Uuid.h"

namespace com::dag_scheduler {
class DAGEdge;
class DAG;

/**
 * @brief A class that represents a vertex or node in a acyclic graph
 *        (dag).
 *
 * A class that represents a vertex or node in a acyclic graph (dag) and
 * operations that can be performed on it. Users can check for connected
 * \ref DAGVertex and get access to the connected \ref DAGVertex. In
 * addition users can connect instance of \ref DAGVertex, find out
 * how many \ref DAGEdges come from an instance, and check to see how
 * many \ref DAGEdges point at a given instance.
 */
class DAGVertex {
private:
    friend class DAGEdge;
    friend class DAG;
    friend struct DAGVertex_connection;

public:
    /**
     * @brief A utility struct used to reconnect a \ref DAGVertex
     *
     * This is a utility class used to reconnect two \ref DAGVertex
     * and their associated \ref DAGEdge.
     */
    struct DAGVertex_connection {
    public:
        /**
         * @brief A constructor for a \ref DAGVertex_connection
         *
         * A constructor for a \ref DAGVertex_connection
         */
        DAGVertex_connection();

        /**
         * @brief A constructor for a \ref DAGVertex_connection
         *
         * A constructor for a \ref DAGVertex_connection
         *
         * @param edge The \ref DAGEdge that points to \ref vertex.
         * @param vertex The \ref DAGVertex pointed to by \ref edge.
         */
        DAGVertex_connection(DAGEdge& edge, DAGVertex& vertex);

        /**
         * @brief A getter for the \ref DAGEdge that points to \ref vertex_
         *
         * A getter for the \ref DAGEdge that points to \ref vertex_
         *
         * @return A const reference to the member variable edge_
         */
        const DAGEdge& edge() const;

        /**
         * @brief A getter for the \ref DAGVertex that is pointed to by
         *        \ref edge_.
         *
         * A getter for the \ref DAGVertex that is pointed to by \ref edge_.
         *
         * @return A const reference to the member variable vertex_.
         */
        const DAGVertex& vertex() const;

    private:
        std::unique_ptr<DAGEdge> edge_;
        std::unique_ptr<DAGVertex> vertex_;
    };

    // TODO: Use dag class to manage status.
    enum class Status {
        INITIALIZED,
        SCHEDULED,
        RUNNING,
        PASSED,
        FAILED,
        INVALID
    };

public:
    /**
     * @brief A constructor for a \ref DAGVertex
     *
     * A constructor for a \ref DAGVertex
     */
    DAGVertex();

    /**
     * @brief A constructor for a \ref DAGVertex
     *
     * A constructor for a \ref DAGVertex
     *
     * @param[in] label A string by which to identify one or more
     *                  \ref DAGVertex.
     */
    explicit DAGVertex(const std::string& label);

    /**
     * @brief A constructor for a \ref DAGVertex
     *
     * A constructor for a \ref DAGVertex
     *
     * @param[in] label A string by which to identify one or more
     *                  \ref DAGVertex.
     * @param[in] task A DAGTask which contains stages to be executed.
     */
    DAGVertex(const std::string& label, std::unique_ptr<Task>&& task);

    /**
     * @brief A constructor for a \ref DAGVertex
     *
     * A constructor for a \ref DAGVertex
     *
     * @param[in] label A string by which to identify one or more
     *                  \ref DAGVertex.
     * @param[in] task A DAGTask which contains stages to be executed.
     * @param[in] uuid A user defined UUID.
     */
    DAGVertex(const std::string& label,
              std::unique_ptr<Task>&& task,
              class UUID&& uuid);

    /**
     * @brief A constructor for a \ref DAGVertex
     *
     * A constructor for a \ref DAGVertex
     *
     * @param[in] label A string by which to identify one or more
     *                  \ref DAGVertex.
     * @param[in] task A DAGTask which contains stages to be executed.
     * @param[in] uuid A user defined UUID.
     */

    /**
     * @brief A destructor for a \ref DAGVertex
     *
     * A destructor for a \ref DAGVertex
     */
    virtual ~DAGVertex();

    /**
     * @brief A move constructor for a \ref DAGVertex.
     *
     * A move constructor for a \ref DAGVertex
     *
     * @param[out] other The \ref DAGVertex to move into (*this).
     */
    DAGVertex(DAGVertex&& other);

    /**
     * @brief A assignment move operator for a \ref DAGVertex.
     *
     * A assignment move operator for a \ref DAGVertex
     *
     * @param[out] rhs The \ref DAGVertex to move into (*this).
     *
     * @return A reference to (*this).
     */
    DAGVertex& operator=(DAGVertex&& rhs);

    /**
     * @brief Clones this object into an identical DAGVertex.
     *
     * A member function of \ref DAGVertex that makes an identical copy of
     * this and returns it. However, does not preseve \ref DAGEdges.
     *
     * @return A \ref DAGVertex that is an identical copy of this.
     */
    DAGVertex clone();

    /**
     * @brief Connects a ref counted instance of a \ref DAGVertex to this.
     *
     * @param[in] other The \ref DAGVertex for which an \ref DAGEdge is
     *                  drawn to.
     *
     * @return true if connection is made false otherwise.
     */
    bool connect(std::shared_ptr<DAGVertex> other);

    /**
     * @brief Checks for the existance of a \ref DAGEdge to \ref other.
     *
     * A member fucntion of \ref DAGVertex that checks if \ref this
     * contains a \ref DAGEdge to \ref other.
     *
     * @param[in] other The \ref DAGVertex which we are checking for a
     *                  \ref DAGEdge that is drawn to it.
     *
     * @return true if (*this) is connected via a \ref DAGEdge to \p other.
     */
    bool contains_connection_to(const DAGVertex& other);

    /**
     * @brief A utility function used by \ref dag to reconnect after a
     *        clone.
     *
     * A member function of \ref DAGVertex that is to be used after
     * \ref clone. Because a clone does not preserve connections users who
     * choose to use this class outside of a \ref dag will need a way to
     * reconnect the \ref dag_eged (s) that existed from \ref this before
     * \ref clone was called. To get the \ref connections users must call
     * \ref the member function \ref clone_all_connections before calling
     * this member function.
     *
     * @param[in] connections A collection of \ref DAGVertex_connection
     *                        that contains the results of calling
     *                        \ref clone_all_connections.
     *
     * @return A \ref std::vector<\ref std::shared_ptr<\ref DAGVertex>>
     *         which contains ref counted instances of the connections
     *         restablished by calling this function.
     */
    std::vector<std::shared_ptr<DAGVertex>> restablish_connections(
            std::vector<DAGVertex_connection>& connections);

    /**
     * @brief Used to get the number of \ref DAGVertex (s) this points to.
     *
     * A member function of \ref DAGVertex that returns the number of
     * current connections from \ref this to other \ref DAGVertex via
     * a \ref DAGEdge.
     *
     * @return A positive integer with the number of \ref DAGEdge (s) that
     *         extend from \ref this to other \ref DAGVertex (s).
     */
    std::size_t edge_count() const;

    /**
     * @brief A utility function that visits all \ref DAGEdge (s) of this.
     *
     * A utility member function of \ref DAGVertex that visits each
     * \ref DAGEdge in the order they were added to the collection of
     * \ref DAGEdge (s) \ref this contains.
     *
     * @param[in] cb A user defined function that is called once per
     *               \ref DAGEdge that \ref this contains in the order
     *               that the \ref DAGEdge (s) were added.
     */
    void visit_all_edges(std::function<void(const DAGEdge&)> cb) const;

    /**
     * @brief A utility function used with \ref clone to preserve
     *        connections.
     *
     * A utility member function of \ref DAGVertex that is to be used with
     * the member function of \ref DAGVertex, \ref clone to make it
     * possible for user's code to preserve connections after a \ref clone.
     * To restablish the connections the user would call the member function
     * \ref restablish_connections with the return value.
     *
     * @return A \ref std::vector<\ref DAGVertex_connections> of the
     *         \ref DAGEdge (s) and their associate \ref DAGVertex.
     */
    std::vector<DAGVertex_connection> clone_all_connections();

    /**
     * @brief A utility function to see if any \ref DAGEdges point to this.
     *
     * A utility member function of \ref DAGVertex that returns a boolean
     * value of \ref true if any \ref DAGEdge (s) point at \ref this.
     *
     * @return true if \ref DAGEdge::connect_to was called with \ref this
     *         provided as the ref counted object proveded as an argument to
     *         the aforementioned function of \ref DAGEdge. If the
     *         contrary, then \ref false is returned.
     */
    bool has_incoming_edges() const;

    /**
     * @brief A utility function used to get the number of \ref DAGEdge (s)
     *        that point at this.
     *
     * A utility function of \ref DAGVertex that returns a positive integer
     * for the number of \ref DAGEdge (s) that point at \ref this.
     *
     * @return A positive integer that represents the number of
     *         \ref DAGEdge (s) that point at this.
     */
    std::size_t incoming_edge_count() const;

    /**
     * @brief A getter for the unique id owned by a instance of
     *        \ref DAGEdge.
     *
     * A member getter function for \ref DAGVertex for the unique id owned
     * by a instance or any clone of that instance of a \ref DAGEdge.
     *
     * @return A \ref uuid owned by the instance of \ref this.
     */
    const UUID& get_uuid() const;

    //*! TODO (mhoggan): Add doc string.
    const Status& current_status() const;

    //*! TODO (mhoggan): Add doc string.
    std::string current_status_as_string() const;

    /**
     * @brief A getter for the label used to visualize a \ref DAGVertex.
     *
     * A member getter function for \ref DAGVertex for the label that
     * helps identify a instance of \ref DAGVertex. Note that it is not
     * guaranteed to be unique.
     *
     * @return A string representation of that label.
     */
    const std::string& label() const;

    /**
     * @brief A getter for the Task assigned to this \ref DAGVertex.
     *
     * @return The Task held by this vertex.
     */
    std::unique_ptr<Task>& get_task();

public:
    /**
     * @brief A stream operator for writting a \ref DAGVertex to a stream.
     *
     * @param[out] out The stream to write \p v to.
     * @param[in] v The \ref DAGVertex to write it \p out.
     *
     * @return A reference to \p out after it was streamed to.
     */
    friend std::ostream& operator<<(std::ostream& out, const DAGVertex& v);

    /**
     * @brief Comparison operator to compare equivalence of two
     *        \ref DAGVertex(s).
     *
     * Two \ref DAGVertexe (s) are considered equivalent if:
     *   * First their uuid (s) are equivalent.
     *   * Second ther label (s) are equivalent.
     *   * Third their edge counts are equivalnet.
     *   * Fourth their incoming edge counts are equivalent.
     *   * Fith their status' are the same.
     *
     * @param[in] lhs The \ref DAGVertex on the left hand side of the '=='.
     * @param[in] rhs The \ref DAGVertex on the right hand side of the
     *                '=='.
     *
     * @return true if \p lhs == \p rhs.
     */
    friend bool operator==(const DAGVertex& lhs, const DAGVertex& rhs);

    /**
     * @brief Comparison operator to compare non equivalence of two
     *        \ref DAGVertex(s).
     *
     * Two \ref DAGVertexe (s) are considered equivalent if:
     *   * First their uuid (s) are equivalent.
     *   * Second ther label (s) are equivalent.
     *   * Third their edge counts are equivalnet.
     *   * Fourth their incoming edge counts are equivalent.
     *   * Fith their status' are the same.
     *
     * @param[in] lhs The \ref DAGVertex on the left hand side of the '!='.
     * @param[in] rhs The \ref DAGVertex on the right hand side of the
     *                '!='.
     *
     * @return true if \p lhs != \p rhs.
     */
    friend bool operator!=(const DAGVertex& lhs, const DAGVertex& rhs);

protected:
    void add_incoming_edge();
    void sub_incoming_edge();
    void clear_edges();
    void reset_incoming_edge_count();
    const DAGEdge& get_edge_at(std::size_t i) const;

    DAGVertex(const DAGVertex& other);
    DAGVertex& operator=(const DAGVertex& rhs);

private:
    class UUID uuid_;
    Status current_status_;
    std::string label_;
    std::vector<std::unique_ptr<DAGEdge>> edges_;
    std::atomic<std::size_t> incoming_edge_count_;
    std::unique_ptr<Task> task_;

private:
    FRIEND_TEST(TestDagVertex, connect_and_contains_connection);
    FRIEND_TEST(TestDagVertex, visit_all_edges_points_to_actual_verticies);
    FRIEND_TEST(TestDagVertex, clone_all_edges_and_restablish_connections);
    FRIEND_TEST(TestDagVertex, move_ctor_with_edges);
    FRIEND_TEST(TestDagVertex, assignment_move_operator_with_edges);
    FRIEND_TEST(TestDagVertex, copy_ctor_no_edges);
    FRIEND_TEST(TestDagVertex, assignment_operator_no_edges);
    FRIEND_TEST(TestDagVertex, clone_all_edges);
    FRIEND_TEST(TestDagVertex, copy_ctor_with_edges);
    FRIEND_TEST(TestDagVertex, assignment_operator_with_edges);
    FRIEND_TEST(TestDagVertex, move_ctor_with_edges_with_task);
    FRIEND_TEST(TestDagVertex, assignment_move_operator_with_edges_with_task);
    FRIEND_TEST(TestDagVertex, copy_ctor_no_edges_with_task);
    FRIEND_TEST(TestDagVertex, assignment_operator_no_edges_with_task);
    FRIEND_TEST(TestDagVertex, clone_all_edges_with_task);
    FRIEND_TEST(TestDagVertex, copy_ctor_with_edges_with_task);
    FRIEND_TEST(TestDagVertex, assignment_operator_with_edges_with_task);
    FRIEND_TEST(TestDagVertex, add_incoming_edge);
    FRIEND_TEST(TestDagVertex, sub_incoming_edge);
    FRIEND_TEST(TestDagVertex, clear_edges);
    FRIEND_TEST(TestDagVertex, reset_incoming_edge_count);
    FRIEND_TEST(TestDagVertex, get_edge_at);
};
}  // namespace com::dag_scheduler
#endif
