#ifndef HTTPS_SESSION_H_INCLUDED
#define HTTPS_SESSION_H_INCLUDED

#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/workflow_service.h"

#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>

#include <memory>

namespace com
{
  namespace dag_scheduler
  {
    class https_session :
      public logged_class<https_session>,
      public std::enable_shared_from_this<https_session>
      {
      private:
        friend class https_listener;

      private:
        class session_responder :
          public responder
        {
        public:
          explicit session_responder(https_session& self);

          virtual void send(StringMessageType &&) override;
          virtual void send(EmptyMessageType &&) override;

        private:
          https_session& self_;
        };

      public:
        https_session(
          boost::asio::ip::tcp::socket&& socket,
          boost::asio::ssl::context& ctx,
          const std::shared_ptr<const std::string>& doc_root,
          workflow_service::https_listener& owner,
          workflow_service::router& router_);

        void run();

      private:
        void on_run();
        void on_handshake(boost::beast::error_code ec);
        void do_read();
        void on_read(boost::beast::error_code ec,
          std::size_t bytes_transferred);
        void do_close();
        void on_shutdown(boost::beast::error_code ec);
        void on_write(bool close, boost::beast::error_code ec, std::size_t);

      private:
        boost::beast::ssl_stream<boost::beast::tcp_stream> stream_;
        boost::beast::flat_buffer buffer_;
        std::shared_ptr<std::string const> doc_root_;
        endpoint_handler::StringRequestType req_;
        std::shared_ptr<void> res_;
        std::unique_ptr<responder> responder_;
        workflow_service::https_listener&  owner_;
        workflow_service::router& router_;
    };
  }
}
#endif
