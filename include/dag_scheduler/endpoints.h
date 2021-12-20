#ifndef ENDPOINTS_H_INCLUDED
#define ENDPOINTS_H_INCLUDED

#include "dag_scheduler/logged_class.hpp"

#include <boost/beast.hpp>

namespace com
{
  namespace dag_scheduler
  {
    struct responder
    {
    public:
      typedef boost::beast::http::empty_body EmptyBodyType;
      typedef boost::beast::http::string_body StringBodyType;
      typedef boost::beast::http::basic_fields<std::allocator<char>> FieldsType;
      typedef boost::beast::http::message<false, StringBodyType, FieldsType>
        StringMessageType;
      typedef boost::beast::http::message<false, EmptyBodyType, FieldsType>
        EmptyMessageType;

      virtual ~responder();

      virtual void send(StringMessageType &&) = 0;
      virtual void send(EmptyMessageType &&) = 0;
    };

    struct endpoint_handler
    {
    public:
      typedef boost::beast::http::string_body StringBodyType;
      typedef boost::beast::http::request<StringBodyType> StringRequestType;

    public:
      endpoint_handler();

      explicit endpoint_handler(const boost::beast::string_view& endpoint);

      virtual ~endpoint_handler();

      virtual bool operator()(StringRequestType&& req,
        std::unique_ptr<responder>&& responder);

      virtual bool operator()(StringRequestType&& req,
        std::unique_ptr<responder>&& responder) const;

    private:
      boost::beast::string_view endpoint_;
    };

    struct doc_root_endpoint :
      public endpoint_handler,
      logged_class<doc_root_endpoint>
    {
    public:
      explicit doc_root_endpoint(const boost::beast::string_view& doc_root);

      virtual ~doc_root_endpoint();

      bool operator()(StringRequestType&& req,
        std::unique_ptr<responder>&& responder) final;

      bool operator()(StringRequestType&& req,
        std::unique_ptr<responder>&& responder) const final;

    private:
      bool handle_request(StringRequestType&& req,
        std::unique_ptr<responder>&& responder) const;

    private:
      boost::beast::string_view doc_root_;
    };

    struct register_dag_endpoint :
      public endpoint_handler,
      logged_class<register_dag_endpoint>
    {
    public:
      ~register_dag_endpoint() final;

      bool operator()(StringRequestType&& req,
        std::unique_ptr<responder>&& responder) final;

      bool operator()(StringRequestType&& req,
        std::unique_ptr<responder>&& responder) const final;
    };
  }
}

#endif
