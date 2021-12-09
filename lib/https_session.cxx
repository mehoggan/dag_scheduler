#include "dag_scheduler/https_session.h"

#include "dag_scheduler/service_helpers.h"

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
    https_session::session_responder::session_responder(https_session &self) :
      self_(self)
    {}

    void https_session::session_responder::send(
      responder::StringMessageType&& msg)
    {
      auto sp = std::make_shared<responder::StringMessageType>(std::move(msg));
      self_.res_ = sp;
      boost::beast::http::async_write(
        self_.stream_,
        *sp,
        boost::beast::bind_front_handler(
          &https_session::on_write,
          self_.shared_from_this(),
          sp->need_eof()));
    }

    void https_session::session_responder::send(
      responder::EmptyMessageType&& msg)
    {
      auto sp = std::make_shared<responder::EmptyMessageType>(std::move(msg));
      self_.res_ = sp;
      boost::beast::http::async_write(
        self_.stream_,
        *sp,
        boost::beast::bind_front_handler(
          &https_session::on_write,
          self_.shared_from_this(),
          sp->need_eof()));
    }

    https_session::https_session(
      boost::asio::ip::tcp::socket&& socket,
      boost::asio::ssl::context& ctx,
      const std::shared_ptr<const std::string>& doc_root,
      workflow_service::https_listener& owner,
      workflow_service::router& router) :
      logged_class<https_session>(*this),
      stream_(std::move(socket), ctx),
      doc_root_(doc_root),
      responder_(new session_responder(*this)),
      owner_(owner),
      router_(router)
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
        logging::error(LOG_TAG, "Failed to read in session with", ec.message());
      } else {
        logging::info(LOG_TAG, "Going to handle request...");
        try {
          auto& handler_ptr =router_[req_.target()];
          if (not handler_ptr) {
            throw std::runtime_error("Failed to handle " +
              req_.target().to_string());
          } else {
            endpoint_handler& handler = *(router_[req_.target()]);
            bool status = handler(std::move(req_), std::move(responder_));
            if (not status) {
              throw std::runtime_error("Failed to handle " +
                req_.target().to_string());
            }
          }
        } catch (std::runtime_error& rte) {
          logging::error(LOG_TAG, "Failed to handle", req_.target().to_string(),
            "with", rte.what());
          do_close();
        }
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
