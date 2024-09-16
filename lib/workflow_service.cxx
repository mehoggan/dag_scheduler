#include "dag_scheduler/workflow_service.h"

#include <atomic>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>
#include <memory>
#include <thread>
#include <vector>

#include "dag_scheduler/https_session.h"

namespace com {
namespace dag_scheduler {
namespace {
std::shared_ptr<com::dag_scheduler::WorkflowService::HTTPSListener>
make_https_listener(boost::asio::io_context &ioc,
                    boost::asio::ssl::context &ctx,
                    std::shared_ptr<const std::string> doc_root,
                    const boost::asio::ip::address &address,
                    const uint16_t port, WorkflowService::Router &router) {
  auto endpoint = boost::asio::ip::tcp::endpoint(address, port);
  return std::make_shared<com::dag_scheduler::WorkflowService::HTTPSListener>(
      ioc, ctx, endpoint, doc_root, router);
}
}  // namespace

bool WorkflowService::Router::register_endpoint(
    const boost::beast::string_view &endpoint,
    std::unique_ptr<EndpointHandler> handler) {
  bool ret = true;
  if (router_.find(endpoint) != router_.end()) {
    ret = false;
  } else {
    router_[endpoint] = std::move(handler);
  }
  return ret;
}

std::unique_ptr<EndpointHandler> &WorkflowService::Router::operator[](
    const boost::beast::string_view &endpoint) {
  std::unique_ptr<EndpointHandler> ret(nullptr);
  if (router_.find(endpoint) == router_.end()) {
    std::string err_msg =
        "No handler for " + std::string(endpoint) + " was found.";
    throw std::runtime_error(err_msg);
  } else {
    return router_[endpoint];
  }
}

WorkflowService::WorkflowService(const WorkflowService::ConnectionInfo &ci)
    : LoggedClass<WorkflowService>(*this),
      ioc_(ci.threads_),
      ctx_(boost::asio::ssl::context::tlsv12) {
  Logging::info(LOG_TAG, "Workflow service startup...");
  detail::load_server_cert(ctx_, ci.pem_, LOG_TAG);
  Logging::info(LOG_TAG, "Setting up https_listener...");
  auto doc_root = std::make_shared<const std::string>(ci.doc_root_);
  auto const address = boost::asio::ip::make_address(ci.address_);
  auto const port = static_cast<uint16_t>(ci.port_);

  std::unique_ptr<EndpointHandler> doc_root_handler(
      new DocRootEndpoint(ci.doc_root_));
  router_.register_endpoint("/index.html", std::move(doc_root_handler));

  auto sig_handler = [&](const boost::system::error_code &ec, int signum) {
    Logging::info(LOG_TAG, "Exiting with", ec.message(), "and signal", signum);
    exit(signum);
  };
  boost::asio::signal_set signals(ioc_, SIGINT);
  signals.async_wait(sig_handler);

  std::shared_ptr<HTTPSListener> listener =
      make_https_listener(ioc_, ctx_, doc_root, address, port, router_);
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

WorkflowService::HTTPSListener::HTTPSListener(
    boost::asio::io_context &ioc, boost::asio::ssl::context &ctx,
    boost::asio::ip::tcp::endpoint &endpoint,
    std::shared_ptr<const std::string> &doc_root, Router &router)
    : LoggedClass<HTTPSListener>(*this),
      ioc_(ioc),
      ctx_(ctx),
      acceptor_(ioc),
      endpoint_(endpoint),
      doc_root_(doc_root),
      router_(router) {
  create_acceptor();
}

WorkflowService::HTTPSListener::~HTTPSListener() {
  Logging::info(LOG_TAG, "Shutting down https_listener...");
}

void WorkflowService::HTTPSListener::run() { do_accept(); }

void WorkflowService::HTTPSListener::reset() {
  acceptor_.close();
  create_acceptor();
  run();
}

void WorkflowService::HTTPSListener::do_accept() {
  acceptor_.async_accept(boost::asio::make_strand(ioc_),
                         boost::beast::bind_front_handler(
                             &HTTPSListener::on_accept, shared_from_this()));
  Logging::info(LOG_TAG, "Acceptor now accepting connections...");
}

void WorkflowService::HTTPSListener::on_accept(
    boost::beast::error_code ec, boost::asio::ip::tcp::socket socket) {
  if (ec) {
    Logging::error(LOG_TAG, "Failed to service request,", ec.message());
  } else {
    Logging::info(LOG_TAG, "Starting up session...");
    socket.set_option(boost::asio::socket_base::keep_alive(true), ec);
    if (not ec) {
      std::make_shared<HTTPSSession>(std::move(socket), ctx_, doc_root_, *this,
                                     router_)
          ->run();
    } else {
      Logging::error(LOG_TAG, "Failed to keep socket alive.");
    }
  }
}

void WorkflowService::HTTPSListener::create_acceptor() {
  Logging::info(LOG_TAG, "Setting up connection acceptor...");
  boost::beast::error_code ec;
  acceptor_.open(endpoint_.protocol(), ec);
  if (ec) {
    Logging::error(LOG_TAG, "Failed to accept incoming connections!");
    throw boost::beast::system_error(ec);
  }

  acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec) {
    Logging::error(LOG_TAG, "Failed to set options on acceptor!");
    throw boost::beast::system_error(ec);
  }

  acceptor_.set_option(boost::asio::socket_base::keep_alive(true), ec);
  if (ec) {
    Logging::error(LOG_TAG, "Failed to set options on acceptor!");
    throw boost::beast::system_error(ec);
  }

  acceptor_.bind(endpoint_, ec);
  if (ec) {
    Logging::error(LOG_TAG, "Failed to bind to endpoint!");
    throw boost::beast::system_error(ec);
  }

  acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
  if (ec) {
    Logging::error(LOG_TAG, "Failed to listen to incoming connection!");
    throw boost::beast::system_error(ec);
  }
}
}  // namespace dag_scheduler
}  // namespace com
