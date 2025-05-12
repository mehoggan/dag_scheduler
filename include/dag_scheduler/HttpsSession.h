////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef DAG_SCHEDULER_HTTPSSESSION_H  // cspell:disable-line
#define DAG_SCHEDULER_HTTPSSESSION_H  // cspell:disable-line

#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <memory>

#include "dag_scheduler/LoggedClass.hpp"
#include "dag_scheduler/WorkflowService.h"

namespace com::dag_scheduler {
class HTTPSSession : public LoggedClass<HTTPSSession>,
                     public std::enable_shared_from_this<HTTPSSession> {
private:
    friend class HTTPSListener;

private:
    class SessionResponder : public Responder {
    public:
        explicit SessionResponder(HTTPSSession& self);

        void send(StringMessageType&&) override;
        void send(EmptyMessageType&&) override;

    private:
        HTTPSSession& self_;
    };

public:
    HTTPSSession(boost::asio::ip::tcp::socket&& socket,
                 boost::asio::ssl::context& ctx,
                 std::shared_ptr<const std::string> doc_root,
                 WorkflowService::HTTPSListener& owner,
                 WorkflowService::Router& router);

    void run();

private:
    void onRun();
    void onHandshake(boost::beast::error_code error_code);
    void doRead();
    void onRead(boost::beast::error_code error_code,
                std::size_t bytes_transferred);
    void doClose();
    void onShutdown(boost::beast::error_code error_code);
    void onWrite(bool close, boost::beast::error_code error_code, std::size_t);

private:
    boost::beast::ssl_stream<boost::beast::tcp_stream> stream_;
    boost::beast::flat_buffer buffer_;
    std::shared_ptr<std::string const> doc_root_;
    EndpointHandler::StringRequestType req_;
    std::shared_ptr<void> res_;
    std::unique_ptr<Responder> responder_;
    WorkflowService::HTTPSListener& owner_;
    WorkflowService::Router& router_;
};
}  // namespace com::dag_scheduler
#endif
