#ifndef WORKFLOW_ORCHESTRATOR_H_INCLUDED
#define WORKFLOW_ORCHESTRATOR_H_INCLUDED

#include "dag_scheduler/endpoints.h"
#include "dag_scheduler/logged_class.hpp"
#include "dag_scheduler/service_helpers.h"

#include <yaml-cpp/yaml.h>

#include <boost/asio/connect.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core/tcp_stream.hpp>

#include <memory>

#include "declspec.h"

/*
 * Code in this module borrowed from:
 * https://www.boost.org/doc/libs/develop/libs/beast/example/http/server/
 * async-ssl/http_server_async_ssl.cpp
 */

namespace com
{
  namespace dag_scheduler
  {
    class DLLSPEC_DAGTASKS workflow_service :
      public logged_class<workflow_service>
    {
    public:
      struct connection_info
      {
        std::string address_;
        std::uint32_t port_;
        std::string doc_root_;
        std::uint16_t threads_;
      };

      struct router
      {
      public:
        bool register_endpoint(const boost::beast::string_view &endpoint,
          std::unique_ptr<endpoint_handler> handler);

        std::unique_ptr<endpoint_handler>& operator[](
          const boost::beast::string_view &endpoint);

      private:
        std::unordered_map<
          boost::beast::string_view,
          std::unique_ptr<endpoint_handler>> router_;
      };

      class https_listener :
        public logged_class<https_listener>,
        public std::enable_shared_from_this<https_listener>
      {
      public:
        https_listener(
          boost::asio::io_context& ioc,
          boost::asio::ssl::context& ctx,
          boost::asio::ip::tcp::endpoint endpoint,
          std::shared_ptr<const std::string>& doc_root,
          router& router_);

        ~https_listener() override;

        void run();

        void reset();

      private:
        void do_accept();

        void on_accept(boost::beast::error_code ec,
                       boost::asio::ip::tcp::socket socket);

        void create_acceptor();

      private:
        boost::asio::io_context& ioc_;
        boost::asio::ssl::context& ctx_;
        boost::asio::ip::tcp::acceptor acceptor_;
        boost::asio::ip::tcp::endpoint endpoint_;
        std::shared_ptr<const std::string> doc_root_;
        router& router_;
      };

    public:
      /**
       * @brief default ctor
       */
      explicit workflow_service(const connection_info& ci);

    private:
      boost::asio::io_context ioc_;
      boost::asio::ssl::context ctx_;
      router router_;
    };
  }
}

namespace YAML
{
  template<>
  struct convert<com::dag_scheduler::workflow_service::connection_info>
  {
    static bool decode(const Node& node,
      com::dag_scheduler::workflow_service::connection_info &rhs)
    {
      if (node.size() != 4) {
        return false;
      }
      rhs.address_ = node["address"].as<std::string>();
      if (rhs.address_ == "localhost") {
        rhs.address_ = "127.0.0.1";
      }
      rhs.port_ = node["port"].as<std::uint32_t>();
      rhs.doc_root_ = node["doc-root"].as<std::string>();
      rhs.threads_ = node["threads"].as<std::uint16_t>();
      return true;
    }
  };
}

#endif
