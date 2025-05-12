////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "dag_scheduler/WorkflowService.h"

#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>
#include <boost/utility/string_view.hpp>
#include <memory>
#include <thread>
#include <vector>

#include "dag_scheduler/HttpsSession.h"
#include "dag_scheduler/ServiceHelpers.h"

namespace com::dag_scheduler {
namespace {
std::shared_ptr<com::dag_scheduler::WorkflowService::HTTPSListener>
makeHttpsListener(boost::asio::io_context& ioc,
                  boost::asio::ssl::context& ctx,
                  std::shared_ptr<const std::string> doc_root,
                  const boost::asio::ip::address& address,
                  const uint16_t port,
                  WorkflowService::Router& router) {
    auto endpoint = boost::asio::ip::tcp::endpoint(address, port);
    return std::make_shared<
            com::dag_scheduler::WorkflowService::HTTPSListener>(
            ioc, ctx, endpoint, doc_root, router);
}
}  // namespace

bool WorkflowService::Router::registerEndpoint(
        const boost::beast::string_view& endpoint,
        std::unique_ptr<EndpointHandler> handler) {
    bool ret_val = true;
    if (router_.find(endpoint) != router_.end()) {
        ret_val = false;
    } else {
        router_[endpoint] = std::move(handler);
    }
    return ret_val;
}

std::unique_ptr<EndpointHandler>& WorkflowService::Router::operator[](
        const boost::beast::string_view& endpoint) {
    std::unique_ptr<EndpointHandler> ret_val(nullptr);
    if (router_.find(endpoint) == router_.end()) {
        std::string err_msg =
                "No handler for " + std::string(endpoint) + " was found.";
        throw std::runtime_error(err_msg);
    } else {
        return router_[endpoint];
    }
}

WorkflowService::WorkflowService(
        const WorkflowService::ConnectionInfo& connection_info)
        : LoggedClass<WorkflowService>(*this)
        , ioc_(connection_info.threads_)
        , ctx_(boost::asio::ssl::context::tlsv12) {
    Logging::info(LOG_TAG, "Workflow service startup...");
    detail::loadServerCert(ctx_, connection_info.pem_, LOG_TAG);
    Logging::info(LOG_TAG, "Setting up https_listener...");
    auto doc_root =
            std::make_shared<const std::string>(connection_info.doc_root_);
    auto const address =
            boost::asio::ip::make_address(connection_info.address_);
    auto const port = static_cast<uint16_t>(connection_info.port_);

    std::unique_ptr<EndpointHandler> doc_root_handler(
            new DocRootEndpoint(connection_info.doc_root_));
    router_.registerEndpoint("/index.html", std::move(doc_root_handler));

    auto sig_handler = [&](const boost::system::error_code& error_code,
                           int signum) {
        Logging::info(LOG_TAG,
                      "Exiting with",
                      error_code.message(),
                      "and signal",
                      signum);
        exit(signum);
    };
    boost::asio::signal_set signals(ioc_, SIGINT);
    signals.async_wait(sig_handler);

    std::shared_ptr<HTTPSListener> listener =
            makeHttpsListener(ioc_, ctx_, doc_root, address, port, router_);
    std::vector<std::thread> threads;
    threads.reserve(connection_info.threads_ - 1);
    for (auto i = threads.size(); i > 0; --i) {
        threads.emplace_back([&]() { ioc_.run(); });
    }

    listener->run();
    ioc_.run();
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

WorkflowService::HTTPSListener::HTTPSListener(
        boost::asio::io_context& ioc,
        boost::asio::ssl::context& ctx,
        boost::asio::ip::tcp::endpoint& endpoint,
        std::shared_ptr<const std::string>& doc_root,
        Router& router)
        : LoggedClass<HTTPSListener>(*this)
        , ioc_(ioc)
        , ctx_(ctx)
        , acceptor_(ioc)
        , endpoint_(endpoint)
        , doc_root_(doc_root)
        , router_(router) {
    createAcceptor();
}

WorkflowService::HTTPSListener::~HTTPSListener() {
    Logging::info(LOG_TAG, "Shutting down https_listener...");
}

void WorkflowService::HTTPSListener::run() { doAccept(); }

void WorkflowService::HTTPSListener::reset() {
    acceptor_.close();
    createAcceptor();
    run();
}

void WorkflowService::HTTPSListener::doAccept() {
    acceptor_.async_accept(
            boost::asio::make_strand(ioc_),
            boost::beast::bind_front_handler(&HTTPSListener::onAccept,
                                             shared_from_this()));
    Logging::info(LOG_TAG, "Acceptor now accepting connections...");
}

void WorkflowService::HTTPSListener::onAccept(
        boost::beast::error_code error_code,
        boost::asio::ip::tcp::socket socket) {
    if (error_code) {
        Logging::error(
                LOG_TAG, "Failed to service request,", error_code.message());
    } else {
        Logging::info(LOG_TAG, "Starting up session...");
        socket.set_option(boost::asio::socket_base::keep_alive(true),
                          error_code);
        if (!error_code) {
            std::make_shared<HTTPSSession>(
                    std::move(socket), ctx_, doc_root_, *this, router_)
                    ->run();
        } else {
            Logging::error(LOG_TAG, "Failed to keep socket alive.");
        }
    }
}

void WorkflowService::HTTPSListener::createAcceptor() {
    Logging::info(LOG_TAG, "Setting up connection acceptor...");
    boost::beast::error_code error_code;
    acceptor_.open(endpoint_.protocol(), error_code);
    if (error_code) {
        Logging::error(LOG_TAG, "Failed to accept incoming connections!");
        throw boost::beast::system_error(error_code);
    }

    acceptor_.set_option(boost::asio::socket_base::reuse_address(true),
                         error_code);
    if (error_code) {
        Logging::error(LOG_TAG, "Failed to set options on acceptor!");
        throw boost::beast::system_error(error_code);
    }

    acceptor_.set_option(boost::asio::socket_base::keep_alive(true),
                         error_code);
    if (error_code) {
        Logging::error(LOG_TAG, "Failed to set options on acceptor!");
        throw boost::beast::system_error(error_code);
    }

    acceptor_.bind(endpoint_, error_code);
    if (error_code) {
        Logging::error(LOG_TAG, "Failed to bind to endpoint!");
        throw boost::beast::system_error(error_code);
    }

    acceptor_.listen(boost::asio::socket_base::max_listen_connections,
                     error_code);
    if (error_code) {
        Logging::error(LOG_TAG, "Failed to listen to incoming connection!");
        throw boost::beast::system_error(error_code);
    }
}
}  // namespace com::dag_scheduler
