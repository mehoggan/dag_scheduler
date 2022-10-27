#include "dag_scheduler/https_session.h"

#include "dag_scheduler/service_helpers.h"

#include <boost/asio/placeholders.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/beast/core/string_type.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/beast/version.hpp>
#include <utility>

namespace com
{
  namespace dag_scheduler
  {
    HTTPSSession::SessionResponder::SessionResponder(HTTPSSession &self) :
      self_(self)
    {}

    void HTTPSSession::SessionResponder::send(
      Responder::StringMessageType &&msg)
    {
      auto sp = std::make_shared<Responder::StringMessageType>(std::move(msg));
      self_.res_ = sp;
      boost::beast::http::async_write(
        self_.stream_,
        *sp,
        boost::beast::bind_front_handler(
          &HTTPSSession::on_write,
          self_.shared_from_this(),
          sp->need_eof()));
    }

    void HTTPSSession::SessionResponder::send(
      Responder::EmptyMessageType &&msg)
    {
      auto sp = std::make_shared<Responder::EmptyMessageType>(std::move(msg));
      self_.res_ = sp;
      boost::beast::http::async_write(
        self_.stream_,
        *sp,
        boost::beast::bind_front_handler(
          &HTTPSSession::on_write,
          self_.shared_from_this(),
          sp->need_eof()));
    }

    HTTPSSession::HTTPSSession(
      boost::asio::ip::tcp::socket &&socket,
      boost::asio::ssl::context &ctx,
      std::shared_ptr<const std::string> doc_root,
      WorkflowService::HTTPSListener &owner,
      WorkflowService::Router &router) :
      LoggedClass<HTTPSSession>(*this),
      stream_(std::move(socket), ctx),
      doc_root_(std::move(std::move(doc_root))),
      responder_(new SessionResponder(*this)),
      owner_(owner),
      router_(router)
    {}

   void HTTPSSession::run()
   {
     Logging::info(LOG_TAG, "HTTP session startup...");
     boost::asio::dispatch(
       stream_.get_executor(),
       boost::beast::bind_front_handler(
         &HTTPSSession::on_run, shared_from_this()));
   }

    void HTTPSSession::on_run()
    {
      Logging::info(LOG_TAG, "HTTP session executing...");
      boost::beast::get_lowest_layer(stream_);//.expires_after(
        //std::chrono::seconds(30));
      stream_.async_handshake(
        boost::asio::ssl::stream_base::server,
        boost::beast::bind_front_handler(
          &HTTPSSession::on_handshake,
          shared_from_this()));
    }

    void HTTPSSession::on_handshake(boost::beast::error_code ec)
    {
      if (ec) {
        Logging::error(LOG_TAG, "Failed to handshake with", ec.message());
        do_close();
      } else {
        Logging::info(LOG_TAG, "SSL handshake successful.");
        do_read();
      }
    }

    void HTTPSSession::do_read()
    {
      req_ = {};
      boost::beast::get_lowest_layer(stream_);//.expires_after(
        //std::chrono::seconds(120));
      Logging::info(LOG_TAG, "Going to read in request...");
      boost::beast::http::async_read(stream_, buffer_, req_,
        boost::beast::bind_front_handler(
          &HTTPSSession::on_read, shared_from_this()));
    }

    void HTTPSSession::on_read(
      boost::beast::error_code ec,
      std::size_t bytes_transferred)
    {
      boost::ignore_unused(bytes_transferred);

      if (ec == boost::beast::http::error::end_of_stream) {
        Logging::info(LOG_TAG, "End of stream detected.");
        do_close();
      } else if (ec) {
        Logging::error(LOG_TAG, "Failed to read in session with", ec.message());
      } else {
        Logging::info(LOG_TAG, "Going to handle request...");
        try {
          auto &handler_ptr = router_[req_.target()];
          if (not handler_ptr) {
            throw std::runtime_error("Failed to handle " +
              std::string(req_.target()));
          } else {
            EndpointHandler &handler = *(router_[req_.target()]);
            bool status = handler(std::move(req_), std::move(responder_));
            if (not status) {
              responder_->send(detail::server_error_handler(
                "Internal Server error",
                req_));
            }
          }
        } catch (std::runtime_error &rte) {
          Logging::error(LOG_TAG, "Failed to handle", req_.target(), "with",
              rte.what());
          responder_->send(detail::not_found_handler(
            req_.target(),
            req_));
        }
      }
    }

    void HTTPSSession::do_close()
    {
      boost::beast::get_lowest_layer(stream_).expires_after(
        std::chrono::seconds(30));

      Logging::info(LOG_TAG, "Closing session...");
      stream_.async_shutdown(boost::beast::bind_front_handler(
        &HTTPSSession::on_shutdown, shared_from_this()));
    }

    void HTTPSSession::on_shutdown(boost::beast::error_code ec)
    {
      if (ec) {
        Logging::error(LOG_TAG, "Failed to shutdown with", ec.message());
      }
      owner_.reset();
    }

    void HTTPSSession::on_write(
      bool close,
      boost::beast::error_code ec,
      std::size_t bytes_transferred)
    {
      boost::ignore_unused(bytes_transferred);
      Logging::info(LOG_TAG, "Going to write response...");
      if (ec) {
        Logging::error(LOG_TAG, "Failed to write response with", ec.message());
      } else if (close) {
        do_close();
      } else {
        Logging::info(LOG_TAG, "Going to read another request...");
        res_ = nullptr;
        do_read();
      }
    }
  }
}
