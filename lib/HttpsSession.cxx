////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "dag_scheduler/HttpsSession.h"

#include <boost/asio/dispatch.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/string_type.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/version.hpp>
#include <boost/system/error_code.hpp>
#include <utility>

#include "dag_scheduler/ServiceHelpers.h"

namespace com::dag_scheduler {
HTTPSSession::SessionResponder::SessionResponder(HTTPSSession& self)
        : self_(self) {}

void HTTPSSession::SessionResponder::send(Responder::StringMessageType&& msg) {
    auto msg_ptr =
            std::make_shared<Responder::StringMessageType>(std::move(msg));
    self_.res_ = msg_ptr;
    boost::beast::http::async_write(
            self_.stream_,
            *msg_ptr,
            boost::beast::bind_front_handler(&HTTPSSession::onWrite,
                                             self_.shared_from_this(),
                                             msg_ptr->need_eof()));
}

void HTTPSSession::SessionResponder::send(Responder::EmptyMessageType&& msg) {
    auto msg_ptr =
            std::make_shared<Responder::EmptyMessageType>(std::move(msg));
    self_.res_ = msg_ptr;
    boost::beast::http::async_write(
            self_.stream_,
            *msg_ptr,
            boost::beast::bind_front_handler(&HTTPSSession::onWrite,
                                             self_.shared_from_this(),
                                             msg_ptr->need_eof()));
}

HTTPSSession::HTTPSSession(boost::asio::ip::tcp::socket&& socket,
                           boost::asio::ssl::context& ctx,
                           std::shared_ptr<const std::string> doc_root,
                           WorkflowService::HTTPSListener& owner,
                           WorkflowService::Router& router)
        : LoggedClass<HTTPSSession>(*this)
        , stream_(std::move(socket), ctx)
        , doc_root_(std::move(std::move(doc_root)))
        , responder_(new SessionResponder(*this))
        , owner_(owner)
        , router_(router) {}

void HTTPSSession::run() {
    Logging::info(LOG_TAG, "HTTP session startup...");
    boost::asio::dispatch(stream_.get_executor(),
                          boost::beast::bind_front_handler(
                                  &HTTPSSession::onRun, shared_from_this()));
}

void HTTPSSession::onRun() {
    Logging::info(LOG_TAG, "HTTP session executing...");
    boost::beast::get_lowest_layer(stream_);
    stream_.async_handshake(
            boost::asio::ssl::stream_base::server,
            boost::beast::bind_front_handler(&HTTPSSession::onHandshake,
                                             shared_from_this()));
}

void HTTPSSession::onHandshake(boost::beast::error_code error_code) {
    if (error_code) {
        Logging::error(
                LOG_TAG, "Failed to handshake with", error_code.message());
        doClose();
    } else {
        Logging::info(LOG_TAG, "SSL handshake successful.");
        doRead();
    }
}

void HTTPSSession::doRead() {
    req_ = {};
    boost::beast::get_lowest_layer(stream_);
    Logging::info(LOG_TAG, "Going to read in request...");
    boost::beast::http::async_read(
            stream_,
            buffer_,
            req_,
            boost::beast::bind_front_handler(&HTTPSSession::onRead,
                                             shared_from_this()));
}

void HTTPSSession::onRead(boost::beast::error_code error_code,
                          std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (error_code == boost::beast::http::error::end_of_stream) {
        Logging::info(LOG_TAG, "End of stream detected.");
        doClose();
    } else if (error_code) {
        Logging::error(LOG_TAG,
                       "Failed to read in session with",
                       error_code.message());
    } else {
        Logging::info(LOG_TAG, "Going to handle request...");
        try {
            auto& handler_ptr = router_[req_.target()];
            if (!handler_ptr) {
                throw std::runtime_error("Failed to handle " +
                                         std::string(req_.target()));
            } else {
                EndpointHandler& handler = *(router_[req_.target()]);
                bool status = handler(std::move(req_), std::move(responder_));
                if (!status) {
                    responder_->send(detail::serverErrorHandler(
                            "Internal Server error", req_));
                }
            }
        } catch (std::runtime_error& rte) {
            Logging::error(LOG_TAG,
                           "Failed to handle",
                           req_.target(),
                           "with",
                           rte.what());
            responder_->send(detail::notFoundHandler(req_.target(), req_));
        }
    }
}

void HTTPSSession::doClose() {
    boost::beast::get_lowest_layer(stream_).expires_after(
            std::chrono::seconds(30));

    Logging::info(LOG_TAG, "Closing session...");
    stream_.async_shutdown(boost::beast::bind_front_handler(
            &HTTPSSession::onShutdown, shared_from_this()));
}

void HTTPSSession::onShutdown(boost::beast::error_code error_code) {
    if (error_code) {
        Logging::error(
                LOG_TAG, "Failed to shutdown with", error_code.message());
    }
    owner_.reset();
}

void HTTPSSession::onWrite(bool close,
                           boost::beast::error_code error_code,
                           std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    Logging::info(LOG_TAG, "Going to write response...");
    if (error_code) {
        Logging::error(LOG_TAG,
                       "Failed to write response with",
                       error_code.message());
    } else if (close) {
        doClose();
    } else {
        Logging::info(LOG_TAG, "Going to read another request...");
        res_ = nullptr;
        doRead();
    }
}
}  // namespace com::dag_scheduler
