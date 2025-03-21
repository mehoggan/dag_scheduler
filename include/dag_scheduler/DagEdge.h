#ifndef DAG_EDGE_H_INCLUDED
#define DAG_EDGE_H_INCLUDED

#include <gtest/gtest_prod.h>

#include <list>
#include <memory>
#include <ostream>
#include <string>

#include "dag_scheduler/dag_vertex.h"
#include "dag_scheduler/uuid.h"

namespace com {
namespace dag_scheduler {
/**
 * @brief A class that represents a directed edge in a acyclic graph
 *        (dag).
 *
 * A class that represents a directed edge in a acyclic graph (dag) and
 * operations that can be performed on it. Users can check for connected
 * \ref DAGVertex and get access to the connected \ref DAGVertex.
 */
class DAGEdge {
private:
    friend class DAG;
    friend class DAGVertex;
    friend struct DAGVertex::DAGVertex_connection;
    friend bool dag_topological_sort(DAG& g,
                                     std::list<DAGVertex>& sorted_vertices);

public:
    // TODO: Use DAG class to manage status.
    enum class Status { initialized, traversed, non_traverable };

public:
    /**
     * @brief A constructor for a \ref DAGEdge
     *
     * A constructor for a \ref DAGEdge.
     */
    DAGEdge();

    /**
     * @brief A destructor for a \ref DAGEdge
     *
     * A destructor for a \ref DAGEdge
     */
    virtual ~DAGEdge();

    /**
     * @brief A move constructor for a \ref DAGEdge
     *
     * A move constructor for a \ref DAGEdge
     *
     * @param[out] other The \ref DAGEdge to move into (*this).
     */
    DAGEdge(DAGEdge&& other);

    //!
    /*!
     */
    /**
     * @brief A assignment move operator for a \ref DAGEdge
     *
     * A assignment move operator for a \ref DAGEdge
     *
     * @param[out] rhs The \ref DAGEdge to move into (*this).
     *
     * @return A reference to (*this).
     */
    DAGEdge& operator=(DAGEdge&& rhs);

    /**
     * @brief Clones this object into an identical DAGEdge.
     *
     * A member function of \ref DAGEdge that makes an identical copy of
     * this and returns it. However, it resets the connection which can only
     * be made from an object that knows about the connection.
     *
     * @return A \ref DAGEdge that is an identical copy of this.
     */
    DAGEdge clone();

    /**
     * @brief Get a weak reference to the \ref DAGVertex pointed to by
     *        this.
     *
     * A member function of \ref DAGEdge that returns a weak reference to
     * the \ref DAGVertex that this edge points to.
     *
     * @return A \ref std::weak_ptr<\ref DAGVertex> that points to the
     *         \ref DAGVertex that this points to.
     */
    std::weak_ptr<DAGVertex> get_connection();

    /**
     * @brief Checks to see if this \ref DAGEdge points to \ref v
     *
     * @param[in] v A \ref DAGVertex for which we are checking for a
     *              connection to.
     *
     * @return true if \ref this points to \ref v. False other wise.
     */
    bool is_a_connection_to(const DAGVertex& v) const;

    /**
     * @brief A getter for the \ref uuid of \ref this.
     *
     * A member function of \ref DAGEdge that return the \ref uuid that
     * uniqly identifies any clone of \ref this.
     *
     * @return A \ref uuid that uniqly identifies any clone of \ref this.
     */
    const UUID& get_uuid() const;

    //! TODO (mhoggan): Add doc string once implemented.
    const Status& current_status() const;

    //! TODO (mhoggan): Add doc string once implemented.
    std::string current_status_as_string() const;

public:
    /**
     * @brief A stream operator for writting a \ref dag_egde to a stream.
     *
     * @param[out] out The stream to write \p e to.
     * @param[in] e The \ref DAGEdge to write to \p out.
     *
     * @return A reference to \p out after it was streamed to.
     */
    friend std::ostream& operator<<(std::ostream& out, const DAGEdge& e);

    /**
     * @brief Comparison operator to compare equivalnce of two
     *        \ref DAGEdge (s).
     *
     * Two \ref DAGEdge (s) are considered equivalent if:
     *   * First they have the same uuid.
     *   * Second if they have the same status.
     *   * Third if they have the same connection (TODO not well defined.)
     *
     * @param[in] lhs The \ref DAGEdge on the left hand side of the '=='.
     * @param[in] rhs The \ref DAGEdge on the right hand side of the '=='.
     *
     * @return true if \p lhs == \p rhs.
     */
    friend bool operator==(const DAGEdge& lhs, const DAGEdge& rhs);

    /**
     * @brief Comparison operator to compare non equivalnce of two
     *        \ref DAGEdge (s).
     *
     * Two \ref DAGEdge (s) are considered equivalent if:
     *     * First they have the same uuid.
     *     * Second if they have the same status.
     *     * Third if they have the same connection (TODO not well defined.)
     *
     * @param[in] lhs The \ref DAGEdge on the left hand side of the '!='.
     * @param[in] rhs The \ref DAGEdge on the right hand side of the '!='.
     *
     * @return true if \p lhs != \p rhs.
     */
    friend bool operator!=(const DAGEdge& lhs, const DAGEdge& rhs);

protected:
    bool connect_to(std::shared_ptr<DAGVertex> v);
    DAGEdge(const DAGEdge& other);
    DAGEdge& operator=(const DAGEdge& rhs);

private:
    class UUID uuid_;
    Status current_status_;
    std::weak_ptr<DAGVertex> connection_;

private:
    FRIEND_TEST(TestDagEdge, copy_ctor);
    FRIEND_TEST(TestDagEdge, assignment_operator);
    FRIEND_TEST(TestDagEdge, connect_and_clone);
    FRIEND_TEST(TestDagEdge, connect_to_null);
    FRIEND_TEST(TestDagEdge, connections);
    FRIEND_TEST(TestDagEdge, equality_operators_with_connection);
};
}  // namespace dag_scheduler
}  // namespace com
#endif
