#include "dag_scheduler/workflow_service.h"

#include "dag_scheduler/https_session.h"
#include "dag_scheduler/logging.h"
#include "dag_scheduler/service_helpers.h"

#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

namespace com
{
  namespace dag_scheduler
  {
    namespace
    {
      std::shared_ptr<com::dag_scheduler::workflow_service::https_listener>
      make_https_listener(
        boost::asio::io_context& ioc,
        boost::asio::ssl::context& ctx,
        std::shared_ptr<const std::string> doc_root,
        const boost::asio::ip::address& address,
        const uint16_t port,
        workflow_service::router& router)
      {
        auto endpoint = boost::asio::ip::tcp::endpoint(address, port);
        return std::make_shared<
          com::dag_scheduler::workflow_service::https_listener>(
            ioc, ctx, endpoint, doc_root, router);
      }
    }

    bool workflow_service::router::register_endpoint(
      const boost::beast::string_view &endpoint,
      std::unique_ptr<endpoint_handler> handler)
    {
      bool ret = true;
      if (router_.find(endpoint) != router_.end()) {
        ret = false;
      } else {
        router_[endpoint] = std::move(handler);
      }
      return ret;
    }

    std::unique_ptr<endpoint_handler>& workflow_service::router::operator[](
      const boost::beast::string_view &endpoint)
    {
      std::unique_ptr<endpoint_handler> ret(nullptr);
      if (router_.find(endpoint) == router_.end()) {
        std::string err_msg = "No handler for " + endpoint.to_string() +
          " was found.";
        throw std::runtime_error(err_msg);
      } else {
        return router_[endpoint];
      }
    }

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

      std::unique_ptr<endpoint_handler> doc_root_handler(
        new doc_root_endpoint(ci.doc_root_));
      router_.register_endpoint("/index.html", std::move(doc_root_handler));

      auto sig_handler = [&](
        const boost::system::error_code& ec, int signum) {
          logging::info(LOG_TAG, "Exiting with", ec.message(), "and signal",
            signum);
          exit(signum);
        };
      boost::asio::signal_set signals(ioc_, SIGINT);
      signals.async_wait(sig_handler);

      std::shared_ptr<https_listener> listener = make_https_listener(
        ioc_, ctx_, doc_root, address, port, router_);
      std::vector<std::thread> v;
      v.reserve(ci.threads_ - 1);
      for (auto i = v.size(); i > 0; --i) {
        v.emplace_back([&]() { ioc_.run(); });
      }

      listener->run();
      ioc_.run();
      for (auto &t : v) {
        if (t.joinable()) {
          t.join();
        }
      }
   }

   workflow_service::https_listener::https_listener(
      boost::asio::io_context& ioc,
      boost::asio::ssl::context& ctx,
      boost::asio::ip::tcp::endpoint& endpoint,
      std::shared_ptr<const std::string>& doc_root,
      router& router) :
      logged_class<https_listener>(*this),
      ioc_(ioc),
      ctx_(ctx),
      acceptor_(ioc),
      endpoint_(endpoint),
      doc_root_(doc_root),
      router_(router)
    {
      create_acceptor();
    }

    workflow_service::https_listener::~https_listener()
    {
      logging::info(LOG_TAG, "Shutting down https_listener...");
    }

    void workflow_service::https_listener::run()
    {
      do_accept();
    }

    void workflow_service::https_listener::reset()
    {
      acceptor_.close();
      create_acceptor();
      run();
    }

    void workflow_service::https_listener::do_accept()
    {
      acceptor_.async_accept(
        boost::asio::make_strand(ioc_),
        boost::beast::bind_front_handler(
          &https_listener::on_accept,
          shared_from_this()));
      logging::info(LOG_TAG, "Acceptor now accepting connections...");
    }

    void workflow_service::https_listener::on_accept(
      boost::beast::error_code ec,
      boost::asio::ip::tcp::socket socket)
    {
      if (ec) {
        logging::error(LOG_TAG, "Failed to service request,", ec.message());
      } else {
        logging::info(LOG_TAG, "Starting up session...");
        socket.set_option(boost::asio::socket_base::keep_alive(true), ec);
        if (not ec) {
          std::make_shared<https_session>(
            std::move(socket), ctx_, doc_root_, *this, router_)->run();
        } else {
          logging::error(LOG_TAG, "Failed to keep socket alive.");
        }
      }
    }

    void workflow_service::https_listener::create_acceptor()
    {
      logging::info(LOG_TAG, "Setting up connection acceptor...");
      boost::beast::error_code ec;
      acceptor_.open(endpoint_.protocol(), ec);
      if (ec) {
        logging::error(LOG_TAG, "Failed to accept incomming connections!");
        throw boost::beast::system_error(ec);
      }

      acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
      if (ec) {
        logging::error(LOG_TAG, "Failed to set options on acceptor!");
        throw boost::beast::system_error(ec);
      }

      acceptor_.set_option(boost::asio::socket_base::keep_alive(true), ec);
      if (ec) {
        logging::error(LOG_TAG, "Failet to set options on acceptor!");
        throw boost::beast::system_error(ec);
      }

      acceptor_.bind(endpoint_, ec);
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
  }
}
