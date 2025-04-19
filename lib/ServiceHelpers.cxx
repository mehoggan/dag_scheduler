////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/filesystem/path.hpp>

#include "dag_scheduler/Logging.h"

namespace detail {
boost::beast::string_view mimeType(boost::beast::string_view path) {
    const auto extension = [&path] {
        const auto position = path.rfind(".");
        boost::beast::string_view ret_val;
        if (position != boost::beast::string_view::npos) {
            ret_val = path.substr(position);
        }
        return ret_val;
    }();
    boost::beast::string_view ret_val;
    if (boost::beast::iequals(extension, ".htm")) {
        ret_val = "text/html";
    } else if (boost::beast::iequals(extension, ".html")) {
        ret_val = "text/html";
    } else if (boost::beast::iequals(extension, ".php")) {
        ret_val = "text/html";
    } else if (boost::beast::iequals(extension, ".css")) {
        ret_val = "text/css";
    } else if (boost::beast::iequals(extension, ".txt")) {
        ret_val = "text/plain";
    } else if (boost::beast::iequals(extension, ".js")) {
        ret_val = "application/javascript";
    } else if (boost::beast::iequals(extension, ".json")) {
        ret_val = "application/json";
    } else if (boost::beast::iequals(extension, ".xml")) {
        ret_val = "application/xml";
    } else if (boost::beast::iequals(extension, ".swf")) {
        ret_val = "application/x-shockwave-flash";
    } else if (boost::beast::iequals(extension, ".flv")) {
        ret_val = "video/x-flv";
    } else if (boost::beast::iequals(extension, ".png")) {
        ret_val = "image/png";
    } else if (boost::beast::iequals(extension, ".jpe")) {
        ret_val = "image/jpeg";
    } else if (boost::beast::iequals(extension, ".jpeg")) {
        ret_val = "image/jpeg";
    } else if (boost::beast::iequals(extension, ".jpg")) {
        ret_val = "image/jpeg";
    } else if (boost::beast::iequals(extension, ".gif")) {
        ret_val = "image/gif";
    } else if (boost::beast::iequals(extension, ".bmp")) {
        ret_val = "image/bmp";
    } else if (boost::beast::iequals(extension, ".ico")) {
        ret_val = "image/vnd.microsoft.icon";
    } else if (boost::beast::iequals(extension, ".tiff")) {
        ret_val = "image/tiff";
    } else if (boost::beast::iequals(extension, ".tif")) {
        ret_val = "image/tiff";
    } else if (boost::beast::iequals(extension, ".svg")) {
        ret_val = "image/svg+xml";
    } else if (boost::beast::iequals(extension, ".sggz")) {
        ret_val = "image/svg+xml";
    } else {
        ret_val = "application/text";
    }
    return ret_val;
}

std::string pathCat(boost::beast::string_view base,
                    boost::beast::string_view path) {
    std::string ret_val;
    if (base.empty()) {
        ret_val = std::string(path);
    } else {
        ret_val = std::string(base);
        constexpr char path_separator = '/';
        if (ret_val.back() == path_separator) {
            ret_val.resize(ret_val.size() - 1);
        }
        ret_val.append(path.data(), path.size());
    }
    return ret_val;
}

void loadServerCert(boost::asio::ssl::context& ctx,
                    const boost::filesystem::path& pem_path_,
                    com::dag_scheduler::LogTag& LOG_TAG) {
    /*
     * The certificate was generated from CMD.EXE on Windows 10 using:
     * winpty openssl dhparam -out dh.pem 2048
     * winpty openssl req -newkey rsa:2048 -nodes -keyout key.pem -x509 \
     *  -days 10000 -out cert.pem -subj "//C=US\ST=CA\L=Los Angeles\O=Beast\\
     *  CN=www.example.com"
     */
    com::dag_scheduler::Logging::info(LOG_TAG, "Loading SSL cert...");

    ctx.set_password_callback(
            [](std::size_t, boost::asio::ssl::context_base::password_purpose) {
                return "test";
            });

    ctx.set_options(boost::asio::ssl::context::default_workarounds |
                    boost::asio::ssl::context::no_sslv2 |
                    boost::asio::ssl::context::single_dh_use);

    ctx.use_certificate_chain_file(pem_path_.string());
    ctx.use_private_key_file(pem_path_.string(),
                             boost::asio::ssl::context::file_format::pem);
}

boost::beast::http::response<boost::beast::http::string_body>
badRequestHandler(
        boost::beast::string_view why,
        boost::beast::http::request<boost::beast::http::string_body>& req) {
    boost::beast::http::response<boost::beast::http::string_body> resource(
            boost::beast::http::status::bad_request, req.version());
    resource.set(boost::beast::http::field::server,
                 BOOST_BEAST_VERSION_STRING);
    resource.set(boost::beast::http::field::content_script_type, "text/html");
    resource.keep_alive(req.keep_alive());
    resource.body() = std::string(why);
    resource.prepare_payload();
    return resource;
}

boost::beast::http::response<boost::beast::http::string_body> notFoundHandler(
        boost::beast::string_view target,
        boost::beast::http::request<boost::beast::http::string_body>&
                request) {
    boost::beast::http::response<boost::beast::http::string_body> response(
            boost::beast::http::status::not_found, request.version());
    response.set(boost::beast::http::field::server,
                 BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "text/html");
    response.keep_alive(request.keep_alive());
    response.body() =
            "The resource '" + std::string(target) + "' was not found.\n";
    response.prepare_payload();
    return response;
}

boost::beast::http::response<boost::beast::http::string_body>
serverErrorHandler(
        boost::beast::string_view what,
        boost::beast::http::request<boost::beast::http::string_body>& req) {
    boost::beast::http::response<boost::beast::http::string_body> response(
            boost::beast::http::status::internal_server_error, req.version());
    response.set(boost::beast::http::field::server,
                 BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "text/html");
    response.keep_alive(req.keep_alive());
    response.body() = "An error occurred: '" + std::string(what) + "'\n";
    return response;
}
}  // namespace detail
