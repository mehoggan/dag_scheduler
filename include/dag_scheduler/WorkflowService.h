////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef DAG_SCHEDULER_WORKFLOWSERVICE_H
#define DAG_SCHEDULER_WORKFLOWSERVICE_H

#include <yaml-cpp/yaml.h>

#include <boost/asio/connect.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http.hpp>
#include <boost/functional/hash.hpp>
#include <memory>

#include "dag_scheduler/Endpoints.h"
#include "dag_scheduler/LoggedClass.hpp"

class StringViewHasher {
public:
    std::size_t operator()(
            const boost::beast::string_view& string_view) const {
        std::string hash_string(string_view.data(), string_view.size());
        return boost::hash_range(hash_string.begin(), hash_string.end());
    }
};

/*
 * Code in this module borrowed from:
 * https://www.boost.org/doc/libs/develop/libs/beast/example/http/server/
 * async-ssl/http_server_async_ssl.cpp
 */

namespace com::dag_scheduler {
class WorkflowService : public LoggedClass<WorkflowService> {
public:
    struct ConnectionInfo {
        std::string address_;
        std::uint32_t port_;
        std::string doc_root_;
        std::uint16_t threads_;
        std::string pem_;
    };

    struct Router {
    public:
        bool registerEndpoint(const boost::beast::string_view& endpoint,
                              std::unique_ptr<EndpointHandler> handler);

        std::unique_ptr<EndpointHandler>& operator[](
                const boost::beast::string_view& endpoint);

    private:
        std::unordered_map<boost::beast::string_view,
                           std::unique_ptr<EndpointHandler>,
                           StringViewHasher>
                router_;
    };

    class HTTPSListener : public LoggedClass<HTTPSListener>,
                          public std::enable_shared_from_this<HTTPSListener> {
    public:
        HTTPSListener(boost::asio::io_context& ioc,
                      boost::asio::ssl::context& ctx,
                      boost::asio::ip::tcp::endpoint& endpoint,
                      std::shared_ptr<const std::string>& doc_root,
                      Router& router_);

        ~HTTPSListener() override;

        void run();

        void reset();

    private:
        void doAccept();

        void onAccept(boost::beast::error_code error_code,
                      boost::asio::ip::tcp::socket socket);

        void createAcceptor();

    private:
        boost::asio::io_context& ioc_;
        boost::asio::ssl::context& ctx_;
        boost::asio::ip::tcp::acceptor acceptor_;
        boost::asio::ip::tcp::endpoint endpoint_;
        std::shared_ptr<const std::string> doc_root_;
        Router& router_;
    };

public:
    /**
     * @brief default ctor
     */
    explicit WorkflowService(const ConnectionInfo& connectionInfo);

private:
    boost::asio::io_context ioc_;
    boost::asio::ssl::context ctx_;
    Router router_;
};
}  // namespace com::dag_scheduler

namespace YAML {
template <>
struct convert<com::dag_scheduler::WorkflowService::ConnectionInfo> {
    static bool decode(
            const Node& node,
            com::dag_scheduler::WorkflowService::ConnectionInfo& rhs) {
        if (node.size() != 5) {
            return false;
        }
        rhs.address_ = node["address"].as<std::string>();
        if (rhs.address_ == "localhost") {
            rhs.address_ = "127.0.0.1";
        }
        rhs.port_ = node["port"].as<std::uint32_t>();
        rhs.doc_root_ = node["doc-root"].as<std::string>();
        rhs.threads_ = node["threads"].as<std::uint16_t>();
        rhs.pem_ = node["pem"].as<std::string>();
        return true;
    }
};
}  // namespace YAML

#endif
