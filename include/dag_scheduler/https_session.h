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
    class HTTPSSession :
      public LoggedClass<HTTPSSession>,
      public std::enable_shared_from_this<HTTPSSession>
      {
      private:
        friend class HTTPSListener;

      private:
        class SessionResponder :
          public Responder
        {
        public:
          explicit SessionResponder(HTTPSSession &self);

          void send(StringMessageType &&) override;
          void send(EmptyMessageType &&) override;

        private:
          HTTPSSession &self_;
        };

      public:
        HTTPSSession(
          boost::asio::ip::tcp::socket &&socket,
          boost::asio::ssl::context &ctx,
          std::shared_ptr<const std::string> doc_root,
          WorkflowService::HTTPSListener &owner,
          WorkflowService::Router &router);

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
        EndpointHandler::StringRequestType req_;
        std::shared_ptr<void> res_;
        std::unique_ptr<Responder> responder_;
        WorkflowService::HTTPSListener &owner_;
        WorkflowService::Router &router_;
    };
  }
}
#endif
