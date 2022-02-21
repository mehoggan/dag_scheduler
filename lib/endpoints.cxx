#include "dag_scheduler/endpoints.h"
#include "dag_scheduler/service_helpers.h"

#include <rapidjson/rapidjson.h>

namespace com
{
  namespace dag_scheduler
  {
    responder::~responder()
    {}

    endpoint_handler::endpoint_handler()
    {}

    endpoint_handler::~endpoint_handler()
    {}

    endpoint_handler::endpoint_handler(
      const boost::beast::string_view& endpoint) :
      endpoint_(endpoint)
    {}

    bool endpoint_handler::operator()(StringRequestType&& req,
      std::unique_ptr<responder>&& responder)
    {
      (void) req;
      (void) responder;
      return false;
    }

    bool endpoint_handler::operator()(StringRequestType&& req,
      std::unique_ptr<responder>&& responder) const
    {
      (void) req;
      (void) responder;
      return false;
    }

    doc_root_endpoint::doc_root_endpoint(
      const boost::beast::string_view& doc_root) :
      endpoint_handler(),
      logged_class<doc_root_endpoint>(*this),
      doc_root_(doc_root)
    {}

    doc_root_endpoint::~doc_root_endpoint()
    {}

    bool doc_root_endpoint::operator()(
      endpoint_handler::StringRequestType&& req,
      std::unique_ptr<responder>&& responder)
    {
      return handle_request(std::move(req), std::move(responder));
    }

    bool doc_root_endpoint::operator()(StringRequestType&& req,
      std::unique_ptr<responder>&& responder) const
    {
      return handle_request(std::move(req), std::move(responder));
    }

    bool doc_root_endpoint::handle_request(StringRequestType&& req,
      std::unique_ptr<responder>&& responder) const
    {
      com::dag_scheduler::logging::info(LOG_TAG, "Handling request for",
        doc_root_.to_string());
      responder::StringMessageType response;
      if (req.method() != boost::beast::http::verb::get &&
      req.method() != boost::beast::http::verb::head) {
        response = detail::bad_request_handler("Unknown HTTP-method\n", req);
      } else if (req.target().empty() || req.target()[0] != '/' ||
      req.target().find("..") != boost::beast::string_view::npos) {
        response = detail::bad_request_handler(
          "Illegal request-target\n", req);
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
            boost::beast::http::response<boost::beast::http::empty_body> res(
              boost::beast::http::status::ok, req.version());
            res.set(boost::beast::http::field::server,
              BOOST_BEAST_VERSION_STRING);
            res.set(boost::beast::http::field::content_type,
              detail::mime_type(path));
            res.content_length(body_size);
            res.keep_alive(req.keep_alive());
            responder->send(std::move(res));
            return true;
          } else if (req.method() == boost::beast::http::verb::get) {
            std::string body_str;
            body_str.resize(body.size());
            boost::beast::error_code read_ec;
            body.file().read(&body_str[0], body.size(), read_ec);
            if (not read_ec) {
              com::dag_scheduler::logging::info(LOG_TAG, "Returning",
                body_str);
              response = boost::beast::http::response<
                boost::beast::http::string_body>(
                  std::piecewise_construct,
                  std::make_tuple(std::move(body_str)),
                  std::make_tuple(boost::beast::http::status::ok,
                    req.version()));
            } else {
              com::dag_scheduler::logging::warn(LOG_TAG,
                "Could not read body!");
              response = detail::server_error_handler(ec.message(), req);
            }
          }
        }
      }
      responder->send(std::move(response));
      return true;
    }

    register_dag_endpoint::~register_dag_endpoint()
    {
    }

    bool register_dag_endpoint::operator()(StringRequestType&& req,
      std::unique_ptr<responder>&& responder)
    {
      (void) req;
      (void) responder;
      return true;
    }

    bool register_dag_endpoint::operator()(StringRequestType&& req,
      std::unique_ptr<responder>&& responder) const
    {
      (void) req;
      (void) responder;
      return true;
    }
  }
}
