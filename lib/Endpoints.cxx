////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "dag_scheduler/Endpoints.h"

#include <rapidjson/rapidjson.h>

#include "dag_scheduler/ServiceHelpers.h"

namespace com::dag_scheduler {
Responder::~Responder() {}

EndpointHandler::EndpointHandler() {}

EndpointHandler::~EndpointHandler() {}

EndpointHandler::EndpointHandler(const boost::beast::string_view& endpoint)
        : endpoint_(endpoint) {}

bool EndpointHandler::operator()(StringRequestType&& req,
                                 std::unique_ptr<Responder>&& responder) {
    (void)req;
    (void)responder;
    return false;
}

bool EndpointHandler::operator()(StringRequestType&& req,
                                 std::unique_ptr<Responder>&& responder) const {
    (void)req;
    (void)responder;
    return false;
}

DocRootEndpoint::DocRootEndpoint(const boost::beast::string_view& doc_root)
        : EndpointHandler()
        , LoggedClass<DocRootEndpoint>(*this)
        , doc_root_(doc_root) {}

DocRootEndpoint::~DocRootEndpoint() {}

bool DocRootEndpoint::operator()(EndpointHandler::StringRequestType&& req,
                                 std::unique_ptr<Responder>&& responder) {
    return handle_request(std::move(req), std::move(responder));
}

bool DocRootEndpoint::operator()(StringRequestType&& req,
                                 std::unique_ptr<Responder>&& responder) const {
    return handle_request(std::move(req), std::move(responder));
}

bool DocRootEndpoint::handle_request(
        StringRequestType&& req, std::unique_ptr<Responder>&& Responder) const {
    com::dag_scheduler::Logging::info(
            LOG_TAG, "Handling request for", doc_root_);
    Responder::StringMessageType response;
    if (req.method() != boost::beast::http::verb::get &&
        req.method() != boost::beast::http::verb::head) {
        response = detail::bad_request_handler("Unknown HTTP-method\n", req);
    } else if (req.target().empty() || req.target()[0] != '/' ||
               req.target().find("..") != boost::beast::string_view::npos) {
        response = detail::bad_request_handler("Illegal request-target\n", req);
    } else {
        std::string path = detail::path_cat(doc_root_, req.target());
        boost::beast::error_code ec;
        boost::beast::http::file_body::value_type body;
        body.open(path.c_str(), boost::beast::file_mode::scan, ec);
        if (ec == boost::beast::errc::no_such_file_or_directory) {
            response = detail::not_found_handler(req.target(), req);
        } else if (ec) {
            response = detail::server_error_handler(ec.message(), req);
        } else {
            const std::size_t body_size = body.size();
            if (req.method() == boost::beast::http::verb::head) {
                boost::beast::http::response<boost::beast::http::empty_body>
                        res(boost::beast::http::status::ok, req.version());
                res.set(boost::beast::http::field::server,
                        BOOST_BEAST_VERSION_STRING);
                res.set(boost::beast::http::field::content_type,
                        detail::mime_type(path));
                res.content_length(body_size);
                res.keep_alive(req.keep_alive());
                Responder->send(std::move(res));
                return true;
            } else if (req.method() == boost::beast::http::verb::get) {
                std::string body_str;
                body_str.resize(body.size());
                boost::beast::error_code read_ec;
                body.file().read(&body_str[0], body.size(), read_ec);
                if (!read_ec) {
                    com::dag_scheduler::Logging::info(
                            LOG_TAG, "Returning", body_str);
                    response = boost::beast::http::response<
                            boost::beast::http::string_body>(
                            std::piecewise_construct,
                            std::make_tuple(std::move(body_str)),
                            std::make_tuple(boost::beast::http::status::ok,
                                            req.version()));
                } else {
                    com::dag_scheduler::Logging::warn(LOG_TAG,
                                                      "Could not read body!");
                    response = detail::server_error_handler(ec.message(), req);
                }
            }
        }
    }
    Responder->send(std::move(response));
    return true;
}

RegisterDAGEndpoint::~RegisterDAGEndpoint() {}

bool RegisterDAGEndpoint::operator()(StringRequestType&& req,
                                     std::unique_ptr<Responder>&& responder) {
    (void)req;
    (void)responder;
    return true;
}

bool RegisterDAGEndpoint::operator()(
        StringRequestType&& req, std::unique_ptr<Responder>&& responder) const {
    (void)req;
    (void)responder;
    return true;
}
}  // namespace com::dag_scheduler
