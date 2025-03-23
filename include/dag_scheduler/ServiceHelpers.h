////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef DAG_SCHEDULER_SERVICE_HELPERS_H
#define DAG_SCHEDULER_SERVICE_HELPERS_H

#include "dag_scheduler/Logging.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace std {
template <> struct hash<boost::beast::string_view> {
    std::size_t operator()(const boost::beast::string_view& sv) const {
        return std::hash<string>()(sv);
    }
};
}  // namespace std

namespace detail {
boost::beast::string_view mime_type(boost::beast::string_view path);

std::string path_cat(boost::beast::string_view base,
                     boost::beast::string_view path);

void load_server_cert(boost::asio::ssl::context& ctx,
                      const boost::filesystem::path& pem_path_,
                      com::dag_scheduler::LogTag& LOG_TAG);

boost::beast::http::response<boost::beast::http::string_body>
bad_request_handler(
        boost::beast::string_view why,
        boost::beast::http::request<boost::beast::http::string_body>& req);

boost::beast::http::response<boost::beast::http::string_body> not_found_handler(
        boost::beast::string_view target,
        boost::beast::http::request<boost::beast::http::string_body>& req);

boost::beast::http::response<boost::beast::http::string_body>
server_error_handler(
        boost::beast::string_view what,
        boost::beast::http::request<boost::beast::http::string_body>& req);
}  // namespace detail
#endif
