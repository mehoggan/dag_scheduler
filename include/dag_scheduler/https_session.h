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
        struct send_it
          {
          https_session& self_;

          explicit send_it(https_session& self);

          template<bool isRequest, class Body, class Fields>
          void operator()(
            boost::beast::http::message<isRequest, Body, Fields>&& msg) const
            {
            auto sp = std::make_shared<
              boost::beast::http::message<
              isRequest, Body, Fields>>(std::move(msg));

            self_.res_ = sp;

            boost::beast::http::async_write(
              self_.stream_,
              *sp,
              boost::beast::bind_front_handler(
                &https_session::on_write,
                self_.shared_from_this(),
                sp->need_eof()));
            }
          };

      public:
        https_session(
          boost::asio::ip::tcp::socket&& socket,
          boost::asio::ssl::context& ctx,
          const std::shared_ptr<const std::string>& doc_root,
          https_listener& owner);

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
        boost::beast::http::request<boost::beast::http::string_body> req_;
        std::shared_ptr<void> res_;
        send_it lambda_;
        https_listener&  owner_;
    };
  }
}
#endif
