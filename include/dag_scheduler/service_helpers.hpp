#ifndef SERVICE_HELPERS_H_INCLUDED
#define SERVICE_HELPERS_H_INCLUDED

#include "dag_scheduler/logging.h"

#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>

namespace detail
{
  boost::beast::string_view mime_type(boost::beast::string_view path);

  std::string path_cat(
    boost::beast::string_view base,
    boost::beast::string_view path);

  void load_server_cert(
    boost::asio::ssl::context& ctx,
    com::dag_scheduler::log_tag& LOG_TAG);

  template <class Body, class Allocator>
  boost::beast::http::response<boost::beast::http::string_body>
  bad_request_handler(
    boost::beast::string_view why,
    boost::beast::http::request<Body,
    boost::beast::http::basic_fields<Allocator>>& req,
    com::dag_scheduler::log_tag& LOG_TAG)
  {
    com::dag_scheduler::logging::error(LOG_TAG, "Bad Request:",
                                       why.to_string());
    boost::beast::http::response<boost::beast::http::string_body> res(
      boost::beast::http::status::bad_request, req.version());
    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_script_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
  }

  template <class Body, class Allocator>
  boost::beast::http::response<boost::beast::http::string_body>
  not_found_handler(
    boost::beast::string_view target,
    boost::beast::http::request<Body,
    boost::beast::http::basic_fields<Allocator>>& req,
    com::dag_scheduler::log_tag& LOG_TAG)
  {
    com::dag_scheduler::logging::error(LOG_TAG, "Not Found",
      target.to_string());
    boost::beast::http::response<boost::beast::http::string_body> res(
      boost::beast::http::status::not_found, req.version());
    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "The resource '" + std::string(target) + "'was not found.\n";
    res.prepare_payload();
    return res;
  }

  template <class Body, class Allocator>
  boost::beast::http::response<boost::beast::http::string_body>
  server_error_handler(
    boost::beast::string_view what,
    boost::beast::http::request<Body,
    boost::beast::http::basic_fields<Allocator>>& req,
    com::dag_scheduler::log_tag& LOG_TAG)
  {
    com::dag_scheduler::logging::error(LOG_TAG, "Server Error:",
                                       what.to_string());
    boost::beast::http::response<boost::beast::http::string_body> res(
      boost::beast::http::status::internal_server_error, req.version());
    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "An error occurred: '" + std::string(what) + "'\n";
    return res;
  }

  template <class Body, class Allocator, class Send>
  void handle_request(
    const boost::beast::string_view& doc_root,
    boost::beast::http::request<Body,
    boost::beast::http::basic_fields<Allocator>>&& req,
    Send&& send,
    com::dag_scheduler::log_tag LOG_TAG)
  {
    com::dag_scheduler::logging::info(LOG_TAG, "Handling request for",
      doc_root.to_string());
    boost::beast::http::response<boost::beast::http::string_body> response;
    if (req.method() != boost::beast::http::verb::get &&
      req.method() != boost::beast::http::verb::head) {
      response = bad_request_handler<Body, Allocator>(
        "Unknown HTTP-metho\n", req, LOG_TAG);
    } else if (req.target().empty() || req.target()[0] != '/' ||
      req.target().find("..") != boost::beast::string_view::npos) {
      response = bad_request_handler<Body, Allocator>(
        "Illegal request-target\n", req, LOG_TAG);
    } else {
      std::string path = path_cat(doc_root, req.target());
      boost::beast::error_code ec;
      boost::beast::http::file_body::value_type body;
      body.open(path.c_str(), boost::beast::file_mode::scan, ec);

      if (ec == boost::beast::errc::no_such_file_or_directory) {
        response = not_found_handler<Body, Allocator>(req.target(), req,
          LOG_TAG);
      } else if (ec) {
        response = server_error_handler<Body, Allocator>(ec.message(), req,
          LOG_TAG);
      } else {
        const std::size_t body_size = body.size();
        if (req.method() == boost::beast::http::verb::head) {
          boost::beast::http::response<boost::beast::http::empty_body> res(
            boost::beast::http::status::ok, req.version());
          res.set(boost::beast::http::field::server,
            BOOST_BEAST_VERSION_STRING);
          res.set(boost::beast::http::field::content_type, mime_type(path));
          res.content_length(body_size);
          res.keep_alive(req.keep_alive());
          return send(std::move(res));
        } else if (req.method() == boost::beast::http::verb::get) {
          std::string body_str;
          body_str.resize(body.size());
          boost::beast::error_code read_ec;
          body.file().read(&body_str[0], body.size(), read_ec);
          if (not read_ec) {
            com::dag_scheduler::logging::info(LOG_TAG, "Returning", body_str);
            boost::beast::http::response<boost::beast::http::string_body> res(
              std::piecewise_construct,
              std::make_tuple(std::move(body_str)),
              std::make_tuple(boost::beast::http::status::ok, req.version()));
            return send(std::move(res));
          } else {
            com::dag_scheduler::logging::warn(LOG_TAG, "Could not read body!");
            response = server_error_handler<Body, Allocator>(ec.message(), req,
                                                             LOG_TAG);
          }
        } else {
          response = bad_request_handler<Body, Allocator>(
            "Illegal request-target", req, LOG_TAG);
        }
      }
    }
    send(std::move(response));
  }
}
#endif
