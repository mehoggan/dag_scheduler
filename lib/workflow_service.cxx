#include "dag_scheduler/workflow_service.h"

#include "boost/beast/core/error.hpp"
#include "boost/beast/core/file_base.hpp"
#include "boost/beast/core/string_type.hpp"
#include "boost/beast/http/empty_body.hpp"
#include "boost/beast/http/file_body.hpp"
#include "boost/system/detail/errc.hpp"
#include "dag_scheduler/logging.h"
#include <boost/asio/signal_set.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/version.hpp>

#include <memory>
#include <vector>

namespace detail
{
  boost::beast::string_view mime_type(boost::beast::string_view path)
  {
      const auto ext = [&path] {
        const auto pos = path.rfind(".");
        boost::beast::string_view ret;
        if (pos != boost::beast::string_view::npos) {
          ret = path.substr(pos);
        }
        return ret;
      }();
      boost::beast::string_view ret;
      if (boost::beast::iequals(ext, ".htm")) {
        ret = "text/html";
      } else if (boost::beast::iequals(ext, ".html")) {
        ret = "text/html";
      } else if (boost::beast::iequals(ext, ".php")) {
        ret = "text/html";
      } else if (boost::beast::iequals(ext, ".css")) {
        ret = "text/css";
      } else if (boost::beast::iequals(ext, ".txt")) {
        ret = "text/plain";
      } else if (boost::beast::iequals(ext, ".js")) {
        ret = "application/javascript";
      } else if (boost::beast::iequals(ext, ".json")) {
        ret = "application/json";
      } else if (boost::beast::iequals(ext, ".xml")) {
        ret = "application/xml";
      } else if (boost::beast::iequals(ext, ".swf")) {
        ret = "application/x-shockwave-flash";
      } else if (boost::beast::iequals(ext, ".flv")) {
        ret = "video/x-flv";
      } else if (boost::beast::iequals(ext, ".png")) {
        ret = "image/png";
      } else if (boost::beast::iequals(ext, ".jpe")) {
        ret = "image/jpeg";
      } else if (boost::beast::iequals(ext, ".jpeg")) {
        ret = "image/jpeg";
      } else if (boost::beast::iequals(ext, ".jpg")) {
        ret = "image/jpeg";
      } else if (boost::beast::iequals(ext, ".gif")) {
        ret = "image/gif";
      } else if (boost::beast::iequals(ext, ".bmp")) {
        ret = "image/bmp";
      } else if (boost::beast::iequals(ext, ".ico")) {
        ret = "image/vnd.microsoft.icon";
      } else if (boost::beast::iequals(ext, ".tiff")) {
        ret = "image/tiff";
      } else if (boost::beast::iequals(ext, ".tif")) {
        ret = "image/tiff";
      } else if (boost::beast::iequals(ext, ".svg")) {
        ret = "image/svg+xml";
      } else if (boost::beast::iequals(ext, ".sggz")) {
        ret = "image/svg+xml";
      } else {
        ret = "application/text";
      }
      return ret;
    };

  std::string path_cat(
    boost::beast::string_view base,
    boost::beast::string_view path)
  {
    std::string ret;
    if (base.empty()) {
      ret = std::string(path);
    } else {
      ret = std::string(base);
      constexpr char path_separator = '/';
      if (ret.back() == path_separator) {
        ret.resize(ret.size() - 1);
      }
      ret.append(path.data(), path.size());
    }
    return ret;
  };

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
    com::dag_scheduler::log_tag LOG_TAG = com::dag_scheduler::log_tag(
      "handle_request"))
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
          } else {
            com::dag_scheduler::logging::warn(LOG_TAG, "Could not read body!");
          }
          body.file().seek(0, read_ec);
          boost::beast::http::response<boost::beast::http::file_body> res(
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
              std::make_tuple(boost::beast::http::status::ok, req.version())); 
        } else {
          response = bad_request_handler<Body, Allocator>(
            "Illegal request-target", req, LOG_TAG);
        }
      }
    }
    send(std::move(response));
  }

  void load_server_cert(
    boost::asio::ssl::context& ctx,
    com::dag_scheduler::log_tag& LOG_TAG)
  {
    /*
     * The certificate was generated from CMD.EXE on Windows 10 using:
     * winpty openssl dhparam -out dh.pem 2048
     * winpty openssl req -newkey rsa:2048 -nodes -keyout key.pem -x509 \
     *  -days 10000 -out cert.pem -subj "//C=US\ST=CA\L=Los Angeles\O=Beast\\
     *  CN=www.example.com"
    */
    com::dag_scheduler::logging::info(LOG_TAG, "Loading SSL cert...");

    std::string const cert =
      "-----BEGIN CERTIFICATE-----\n"
      "MIIDaDCCAlCgAwIBAgIJAO8vBu8i8exWMA0GCSqGSIb3DQEBCwUAMEkxCzAJBgNV\n"
      "BAYTAlVTMQswCQYDVQQIDAJDQTEtMCsGA1UEBwwkTG9zIEFuZ2VsZXNPPUJlYXN0\n"
      "Q049d3d3LmV4YW1wbGUuY29tMB4XDTE3MDUwMzE4MzkxMloXDTQ0MDkxODE4Mzkx\n"
      "MlowSTELMAkGA1UEBhMCVVMxCzAJBgNVBAgMAkNBMS0wKwYDVQQHDCRMb3MgQW5n\n"
      "ZWxlc089QmVhc3RDTj13d3cuZXhhbXBsZS5jb20wggEiMA0GCSqGSIb3DQEBAQUA\n"
      "A4IBDwAwggEKAoIBAQDJ7BRKFO8fqmsEXw8v9YOVXyrQVsVbjSSGEs4Vzs4cJgcF\n"
      "xqGitbnLIrOgiJpRAPLy5MNcAXE1strVGfdEf7xMYSZ/4wOrxUyVw/Ltgsft8m7b\n"
      "Fu8TsCzO6XrxpnVtWk506YZ7ToTa5UjHfBi2+pWTxbpN12UhiZNUcrRsqTFW+6fO\n"
      "9d7xm5wlaZG8cMdg0cO1bhkz45JSl3wWKIES7t3EfKePZbNlQ5hPy7Pd5JTmdGBp\n"
      "yY8anC8u4LPbmgW0/U31PH0rRVfGcBbZsAoQw5Tc5dnb6N2GEIbq3ehSfdDHGnrv\n"
      "enu2tOK9Qx6GEzXh3sekZkxcgh+NlIxCNxu//Dk9AgMBAAGjUzBRMB0GA1UdDgQW\n"
      "BBTZh0N9Ne1OD7GBGJYz4PNESHuXezAfBgNVHSMEGDAWgBTZh0N9Ne1OD7GBGJYz\n"
      "4PNESHuXezAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQCmTJVT\n"
      "LH5Cru1vXtzb3N9dyolcVH82xFVwPewArchgq+CEkajOU9bnzCqvhM4CryBb4cUs\n"
      "gqXWp85hAh55uBOqXb2yyESEleMCJEiVTwm/m26FdONvEGptsiCmF5Gxi0YRtn8N\n"
      "V+KhrQaAyLrLdPYI7TrwAOisq2I1cD0mt+xgwuv/654Rl3IhOMx+fKWKJ9qLAiaE\n"
      "fQyshjlPP9mYVxWOxqctUdQ8UnsUKKGEUcVrA08i1OAnVKlPFjKBvk+r7jpsTPcr\n"
      "9pWXTO9JrYMML7d+XRSZA1n3856OqZDX4403+9FnXCvfcLZLLKTBvwwFgEFGpzjK\n"
      "UEVbkhd5qstF6qWK\n"
      "-----END CERTIFICATE-----\n";

    std::string const key =
      "-----BEGIN PRIVATE KEY-----\n"
      "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDJ7BRKFO8fqmsE\n"
      "Xw8v9YOVXyrQVsVbjSSGEs4Vzs4cJgcFxqGitbnLIrOgiJpRAPLy5MNcAXE1strV\n"
      "GfdEf7xMYSZ/4wOrxUyVw/Ltgsft8m7bFu8TsCzO6XrxpnVtWk506YZ7ToTa5UjH\n"
      "fBi2+pWTxbpN12UhiZNUcrRsqTFW+6fO9d7xm5wlaZG8cMdg0cO1bhkz45JSl3wW\n"
      "KIES7t3EfKePZbNlQ5hPy7Pd5JTmdGBpyY8anC8u4LPbmgW0/U31PH0rRVfGcBbZ\n"
      "sAoQw5Tc5dnb6N2GEIbq3ehSfdDHGnrvenu2tOK9Qx6GEzXh3sekZkxcgh+NlIxC\n"
      "Nxu//Dk9AgMBAAECggEBAK1gV8uETg4SdfE67f9v/5uyK0DYQH1ro4C7hNiUycTB\n"
      "oiYDd6YOA4m4MiQVJuuGtRR5+IR3eI1zFRMFSJs4UqYChNwqQGys7CVsKpplQOW+\n"
      "1BCqkH2HN/Ix5662Dv3mHJemLCKUON77IJKoq0/xuZ04mc9csykox6grFWB3pjXY\n"
      "OEn9U8pt5KNldWfpfAZ7xu9WfyvthGXlhfwKEetOuHfAQv7FF6s25UIEU6Hmnwp9\n"
      "VmYp2twfMGdztz/gfFjKOGxf92RG+FMSkyAPq/vhyB7oQWxa+vdBn6BSdsfn27Qs\n"
      "bTvXrGe4FYcbuw4WkAKTljZX7TUegkXiwFoSps0jegECgYEA7o5AcRTZVUmmSs8W\n"
      "PUHn89UEuDAMFVk7grG1bg8exLQSpugCykcqXt1WNrqB7x6nB+dbVANWNhSmhgCg\n"
      "VrV941vbx8ketqZ9YInSbGPWIU/tss3r8Yx2Ct3mQpvpGC6iGHzEc/NHJP8Efvh/\n"
      "CcUWmLjLGJYYeP5oNu5cncC3fXUCgYEA2LANATm0A6sFVGe3sSLO9un1brA4zlZE\n"
      "Hjd3KOZnMPt73B426qUOcw5B2wIS8GJsUES0P94pKg83oyzmoUV9vJpJLjHA4qmL\n"
      "CDAd6CjAmE5ea4dFdZwDDS8F9FntJMdPQJA9vq+JaeS+k7ds3+7oiNe+RUIHR1Sz\n"
      "VEAKh3Xw66kCgYB7KO/2Mchesu5qku2tZJhHF4QfP5cNcos511uO3bmJ3ln+16uR\n"
      "GRqz7Vu0V6f7dvzPJM/O2QYqV5D9f9dHzN2YgvU9+QSlUeFK9PyxPv3vJt/WP1//\n"
      "zf+nbpaRbwLxnCnNsKSQJFpnrE166/pSZfFbmZQpNlyeIuJU8czZGQTifQKBgHXe\n"
      "/pQGEZhVNab+bHwdFTxXdDzr+1qyrodJYLaM7uFES9InVXQ6qSuJO+WosSi2QXlA\n"
      "hlSfwwCwGnHXAPYFWSp5Owm34tbpp0mi8wHQ+UNgjhgsE2qwnTBUvgZ3zHpPORtD\n"
      "23KZBkTmO40bIEyIJ1IZGdWO32q79nkEBTY+v/lRAoGBAI1rbouFYPBrTYQ9kcjt\n"
      "1yfu4JF5MvO9JrHQ9tOwkqDmNCWx9xWXbgydsn/eFtuUMULWsG3lNjfst/Esb8ch\n"
      "k5cZd6pdJZa4/vhEwrYYSuEjMCnRb0lUsm7TsHxQrUd6Fi/mUuFU/haC0o0chLq7\n"
      "pVOUFq5mW8p0zbtfHbjkgxyF\n"
      "-----END PRIVATE KEY-----\n";

    std::string const dh =
      "-----BEGIN DH PARAMETERS-----\n"
      "MIIBCAKCAQEArzQc5mpm0Fs8yahDeySj31JZlwEphUdZ9StM2D8+Fo7TMduGtSi+\n"
      "/HRWVwHcTFAgrxVdm+dl474mOUqqaz4MpzIb6+6OVfWHbQJmXPepZKyu4LgUPvY/\n"
      "4q3/iDMjIS0fLOu/bLuObwU5ccZmDgfhmz1GanRlTQOiYRty3FiOATWZBRh6uv4u\n"
      "tff4A9Bm3V9tLx9S6djq31w31Gl7OQhryodW28kc16t9TvO1BzcV3HjRPwpe701X\n"
      "oEEZdnZWANkkpR/m/pfgdmGPU66S2sXMHgsliViQWpDCYeehrvFRHEdR9NV+XJfC\n"
      "QMUk26jPTIVTLfXmmwU0u8vUkpR7LQKkwwIBAg==\n"
      "-----END DH PARAMETERS-----\n";

    ctx.set_password_callback(
      [](std::size_t, boost::asio::ssl::context_base::password_purpose)
      {
        return "test";
      });

    ctx.set_options(
      boost::asio::ssl::context::default_workarounds |
      boost::asio::ssl::context::no_sslv2 |
      boost::asio::ssl::context::single_dh_use);

    ctx.use_certificate_chain(
      boost::asio::buffer(cert.data(), cert.size()));

    ctx.use_private_key(
      boost::asio::buffer(key.data(), key.size()),
      boost::asio::ssl::context::file_format::pem);

    ctx.use_tmp_dh(
      boost::asio::buffer(dh.data(), dh.size()));
  }
}

namespace com
{
  namespace dag_scheduler
  {
    workflow_service::workflow_service(
      const workflow_service::connection_info& ci) :
      logged_class<workflow_service>(*this),
      ioc_(ci.threads_),
      ctx_(boost::asio::ssl::context::tlsv12)
    {
      logging::info(LOG_TAG, "Workflow service startup...");
      detail::load_server_cert(ctx_, LOG_TAG);
      logging::info(LOG_TAG, "Setting up https_listener...");
      auto doc_root = std::make_shared<const std::string>(ci.doc_root_);
      auto const address = boost::asio::ip::make_address(ci.address_);
      auto const port = static_cast<uint16_t>(ci.port_);
      auto listener = std::make_shared<https_listener>(
        ioc_,
        ctx_,
        boost::asio::ip::tcp::endpoint(address, port),
        doc_root);
      listener->run();

      // Run the I/O service on the requested number of threads
      std::vector<std::thread> v;
      v.reserve(ci.threads_ - 1);
      for (auto i = v.size(); i > 0; --i) {
        v.emplace_back([this] { ioc_.run(); });
      }
      ioc_.run();
   }

    https_listener::https_listener(
      boost::asio::io_context& ioc,
      boost::asio::ssl::context& ctx,
      boost::asio::ip::tcp::endpoint endpoint,
      std::shared_ptr<const std::string>& doc_root) :
      logged_class<https_listener>(*this),
      ioc_(ioc),
      ctx_(ctx),
      acceptor_(ioc),
      doc_root_(doc_root)
    {
      logging::info(LOG_TAG, "Setting up connection acceptor...");
      boost::beast::error_code ec;
      acceptor_.open(endpoint.protocol(), ec);
      if (ec) {
        logging::error(LOG_TAG, "Failed to accept incomming connections!");
        throw boost::beast::system_error(ec);
      }

      acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
      if (ec) {
        logging::error(LOG_TAG, "Failed to set options on acceptor!");
        throw boost::beast::system_error(ec);
      }

      acceptor_.bind(endpoint, ec);
      if (ec) {
        logging::error(LOG_TAG, "Failed to bind to endpoint!");
        throw boost::beast::system_error(ec);
      }

      acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
      if (ec) {
        logging::error(LOG_TAG, "Failed to listen to incoming connection!");
        throw boost::beast::system_error(ec);
      }
    }

    https_listener::~https_listener()
    {
      logging::info(LOG_TAG, "Shutting down https_listener...");
    }

    void https_listener::run()
    {
      do_accept();
    }

    void https_listener::do_accept()
    {
      logging::info(LOG_TAG, "Acceptor now accepting connections...");
      acceptor_.async_accept(
        boost::asio::make_strand(ioc_),
        boost::beast::bind_front_handler(
          &https_listener::on_accept,
          shared_from_this()));
    }

    void https_listener::on_accept(
      boost::beast::error_code ec,
      boost::asio::ip::tcp::socket socket)
    {
      if (ec) {
        logging::error(LOG_TAG, "Failed to service request,", ec.message());
        return;
      } else {
        logging::info(LOG_TAG, "Starting up session...");
        std::make_shared<https_session>(
          std::move(socket),
          ctx_,
          doc_root_)->run();
      }
    }

    https_session::send_it::send_it(https_session &self) :
      self_(self)
    {}

    https_session::https_session(
      boost::asio::ip::tcp::socket&& socket,
      boost::asio::ssl::context& ctx,
      const std::shared_ptr<const std::string>& doc_root) :
      logged_class<https_session>(*this),
      stream_(std::move(socket), ctx),
      doc_root_(doc_root),
      labmda_(*this)
    {}

    void https_session::run()
    {
      boost::asio::dispatch(
        stream_.get_executor(),
        boost::beast::bind_front_handler(
          &https_session::on_run, shared_from_this()));
    }

    void https_session::on_run()
    {
      logging::info(LOG_TAG, "HTTP session running...");
      boost::beast::get_lowest_layer(stream_).expires_after(
          std::chrono::seconds(30));
      stream_.async_handshake(
        boost::asio::ssl::stream_base::server,
        boost::beast::bind_front_handler(
          &https_session::on_handshake,
          shared_from_this()));
    }

    void https_session::on_handshake(boost::beast::error_code ec)
    {
      if (ec) {
        logging::error(LOG_TAG, "Failed to handshake with", ec.message());
      } else {
        do_read();
      }
    }

    void https_session::do_read()
    {
      req_ = {};

      boost::beast::get_lowest_layer(stream_).expires_after(
        std::chrono::seconds(30));

      logging::info(LOG_TAG, "SSL handshake successful.");
      logging::info(LOG_TAG, "Going to read in request...");
      boost::beast::http::async_read(stream_, buffer_, req_,
        boost::beast::bind_front_handler(
          &https_session::on_read, shared_from_this()));
    }

    void https_session::on_read(
      boost::beast::error_code ec,
      std::size_t bytes_transferred)
    {
      boost::ignore_unused(bytes_transferred);

      if (ec == boost::beast::http::error::end_of_stream) {
        logging::info(LOG_TAG, "End of stream detected.");
        do_close();
      } else if (ec) {
        logging::error(LOG_TAG, "Failed to read in session with",
          ec.message());
      } else {
        logging::info(LOG_TAG, "Going to handle request...");
        detail::handle_request(*doc_root_, std::move(req_), labmda_, LOG_TAG);
      }
    }

    void https_session::do_close()
    {
      boost::beast::get_lowest_layer(stream_).expires_after(
        std::chrono::seconds(30));

      logging::info(LOG_TAG, "Closing session...");
      stream_.async_shutdown(boost::beast::bind_front_handler(
        &https_session::on_shutdown, shared_from_this()));
    }

    void https_session::on_shutdown(boost::beast::error_code ec)
    {
      if (ec) {
        logging::error(LOG_TAG, "Failed to shutdown with", ec.message());
      }
    }

    void https_session::on_write(
      bool close,
      boost::beast::error_code ec,
      std::size_t bytes_transferred)
    {
      boost::ignore_unused(bytes_transferred);

      logging::info(LOG_TAG, "Going to write response...");
      if (ec) {
        logging::error(LOG_TAG, "Failed to write response with", ec.message());
      } else if (close) {
        do_close();
      } else {
        logging::info(LOG_TAG, "Going to read another request...");
        res_ = nullptr;
        do_read();
      }
    }
  }
}
