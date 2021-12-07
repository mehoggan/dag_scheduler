#include "dag_scheduler/https_session.h"

#include "dag_scheduler/service_helpers.hpp"

#include <boost/asio/placeholders.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/beast/core/string_type.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/system/detail/error_code.hpp>

namespace com
{
  namespace dag_scheduler
  {
    https_session::send_it::send_it(https_session &self) :
    self_(self)
    {}

    https_session::https_session(
      boost::asio::ip::tcp::socket&& socket,
      boost::asio::ssl::context& ctx,
      const std::shared_ptr<const std::string>& doc_root,
      https_listener& owner) :
      logged_class<https_session>(*this),
      stream_(std::move(socket), ctx),
      doc_root_(doc_root),
      lambda_(*this),
      owner_(owner)
   {}

   void https_session::run()
   {
     logging::info(LOG_TAG, "HTTP session startup...");
     boost::asio::dispatch(
       stream_.get_executor(),
       boost::beast::bind_front_handler(
         &https_session::on_run, shared_from_this()));
   }

    void https_session::on_run()
    {
      logging::info(LOG_TAG, "HTTP session executing...");
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
        logging::info(LOG_TAG, "SSL handshake successful.");
      }
    }

    void https_session::do_read()
    {
      req_ = {};
      boost::beast::get_lowest_layer(stream_).expires_after(
        std::chrono::seconds(30));
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
        detail::handle_request(*doc_root_, std::move(req_), lambda_, LOG_TAG);
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
      owner_.reset();
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
