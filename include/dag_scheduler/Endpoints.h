////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef DAG_SCHEDULER_ENDPOINTS_H
#define DAG_SCHEDULER_ENDPOINTS_H

#include "dag_scheduler/LoggedClass.hpp"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"  // cspell:disable-line
#pragma clang diagnostic ignored "-Wshadow"           // cspell:disable-line
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <boost/beast.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace com::dag_scheduler {
struct Responder {
public:
    typedef boost::beast::http::empty_body EmptyBodyType;
    typedef boost::beast::http::string_body StringBodyType;
    typedef boost::beast::http::basic_fields<std::allocator<char>> FieldsType;
    typedef boost::beast::http::message<false, StringBodyType, FieldsType>
            StringMessageType;
    typedef boost::beast::http::message<false, EmptyBodyType, FieldsType>
            EmptyMessageType;

    virtual ~Responder();

    virtual void send(StringMessageType&&) = 0;
    virtual void send(EmptyMessageType&&) = 0;
};

struct EndpointHandler {
public:
    typedef boost::beast::http::string_body StringBodyType;
    typedef boost::beast::http::request<StringBodyType> StringRequestType;

public:
    EndpointHandler();

    explicit EndpointHandler(const boost::beast::string_view& endpoint);

    virtual ~EndpointHandler();

    virtual bool operator()(StringRequestType&& req,
                            std::unique_ptr<Responder>&& responder);

    virtual bool operator()(StringRequestType&& req,
                            std::unique_ptr<Responder>&& responder) const;

private:
    boost::beast::string_view endpoint_;
};

struct DocRootEndpoint : public EndpointHandler, LoggedClass<DocRootEndpoint> {
public:
    explicit DocRootEndpoint(const boost::beast::string_view& doc_root);

    ~DocRootEndpoint() override;

    bool operator()(StringRequestType&& req,
                    std::unique_ptr<Responder>&& responder) final;

    bool operator()(StringRequestType&& req,
                    std::unique_ptr<Responder>&& responder) const final;

private:
    bool handle_request(StringRequestType&& req,
                        std::unique_ptr<Responder>&& responder) const;

private:
    boost::beast::string_view doc_root_;
};

struct RegisterDAGEndpoint : public EndpointHandler,
                             LoggedClass<RegisterDAGEndpoint> {
public:
    ~RegisterDAGEndpoint() override;

    bool operator()(StringRequestType&& req,
                    std::unique_ptr<Responder>&& responder) final;

    bool operator()(StringRequestType&& req,
                    std::unique_ptr<Responder>&& responder) const final;
};
}  // namespace com::dag_scheduler

#endif
