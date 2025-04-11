////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef DAG_SCHEDULER_SERVICEHELPERS_H  // cspell:disable-line
#define DAG_SCHEDULER_SERVICEHELPERS_H  // cspell:disable-line

#include "dag_scheduler/Logging.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"  // cspell:disable-line
#pragma clang diagnostic ignored "-Wshadow"           // cspell:disable-line
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace std {
template <> struct hash<boost::beast::string_view> {
    std::size_t operator()(const boost::beast::string_view& stringView) const {
        return std::hash<string>()(stringView);
    }
};
}  // namespace std

namespace detail {
boost::beast::string_view mimeType(boost::beast::string_view path);

std::string pathCat(boost::beast::string_view base,
                    boost::beast::string_view path);

void loadServerCert(boost::asio::ssl::context& ctx,
                    const boost::filesystem::path& pem_path_,
                    com::dag_scheduler::LogTag& LOG_TAG);

boost::beast::http::response<boost::beast::http::string_body> badRequestHandler(
        boost::beast::string_view why,
        boost::beast::http::request<boost::beast::http::string_body>& req);

boost::beast::http::response<boost::beast::http::string_body> notFoundHandler(
        boost::beast::string_view target,
        boost::beast::http::request<boost::beast::http::string_body>& req);

boost::beast::http::response<boost::beast::http::string_body>
serverErrorHandler(
        boost::beast::string_view what,
        boost::beast::http::request<boost::beast::http::string_body>& req);
}  // namespace detail
#endif
