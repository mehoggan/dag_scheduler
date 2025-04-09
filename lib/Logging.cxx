////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#include "dag_scheduler/Logging.h"

#include <boost/core/null_deleter.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/formatter.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sources/severity_feature.hpp>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <boost/log/support/date_time.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <iostream>

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity,
                            "Severity",
                            boost::log::trivial::severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)

namespace com::dag_scheduler {
std::atomic<bool> Logging::init_(false);
std::unordered_map<LogTag, boost::weak_ptr<Logging::TextSink>>
        Logging::loggers_;
std::mutex Logging::loggers_mutex_;

namespace detail {
boost::log::formatter fmt =
        boost::log::expressions::stream
        << boost::log::expressions::format_date_time<boost::posix_time::ptime>(
                   "TimeStamp", "%Y-%m-%d %H:%M:%S")
        << " ["
        << boost::log::expressions::attr<
                   boost::log::attributes::current_thread_id::value_type>(
                   "ThreadID")
        << "] "
        << boost::log::expressions::if_(boost::log::expressions::has_attr(
                   tag_attr))[boost::log::expressions::stream << tag_attr]
        << " " << severity << "(" << line_id << ")" << " - \'"
        << boost::log::expressions::smessage << "\'";

boost::shared_ptr<Logging::TextSink> make_sink() {
    boost::shared_ptr<Logging::TextSink> sink =
            boost::make_shared<Logging::TextSink>();
    return sink;
}

void configure_sink(boost::shared_ptr<Logging::TextSink> sink,
                    boost::log::trivial::severity_level level,
                    const std::string& tag) {
    sink->set_formatter(detail::fmt);
    boost::log::core::get()->add_sink(sink);
    sink->set_filter(
            severity >= level &&
            (boost::log::expressions::has_attr(tag_attr) && tag_attr == tag));
}

Logging::Dict::iterator find_sink(const LogTag& tag, Logging::Dict& dict) {
    Logging::Dict::iterator it = std::find_if(
            dict.begin(), dict.end(), [&](Logging::Dict::value_type& v) {
                bool ret = std::string(v.first.tag()) == std::string(tag.tag());
                return ret;
            });
    return it;
}
}  // namespace detail

LogTag::LogTag(const ::std::string& tag) : tag_(tag) {}

const char* LogTag::tag() const { return tag_.c_str(); }

bool Logging::init() {
    if (!init_.load()) {
        boost::log::add_common_attributes();
        init_.store(true);
    }
    return init_.load();
}

boost::filesystem::path Logging::mktmpdir(
        const boost::filesystem::path& model) {
    boost::filesystem::path tmppath = boost::filesystem::unique_path(model);
    boost::filesystem::create_directories(tmppath);
    return tmppath;
}

bool Logging::add_std_cout_logger(const LogTag& tag,
                                  boost::log::trivial::severity_level level) {
    bool ret = false;

    std::lock_guard<std::mutex> lock(Logging::loggers_mutex_);
    auto end_it = Logging::loggers_.end();
    auto tag_it = detail::find_sink(tag, Logging::loggers_);
    if (tag_it == end_it) {
        boost::shared_ptr<Logging::TextSink> sink = detail::make_sink();
        boost::shared_ptr<std::ostream> stream(&std::cout,
                                               boost::null_deleter());
        sink->locked_backend()->add_stream(stream);
        detail::configure_sink(sink, level, tag.tag());
        Logging::loggers_[tag] = sink;
        ret = true;
    }

    return ret;
}

bool Logging::add_std_cerr_logger(const LogTag& tag,
                                  boost::log::trivial::severity_level level) {
    bool ret = false;

    std::lock_guard<std::mutex> lock(Logging::loggers_mutex_);
    auto end_it = Logging::loggers_.end();
    auto tag_it = detail::find_sink(tag, Logging::loggers_);
    if (tag_it == end_it) {
        boost::shared_ptr<Logging::TextSink> sink = detail::make_sink();
        boost::shared_ptr<std::ostream> stream(&std::cerr,
                                               boost::null_deleter());
        sink->locked_backend()->add_stream(stream);
        detail::configure_sink(sink, level, tag.tag());
        Logging::loggers_[tag] = sink;
        ret = true;
    }

    return ret;
}

bool Logging::add_std_log_logger(const LogTag& tag,
                                 boost::log::trivial::severity_level level) {
    bool ret = false;

    std::lock_guard<std::mutex> lock(Logging::loggers_mutex_);
    auto end_it = Logging::loggers_.end();
    auto tag_it = detail::find_sink(tag, Logging::loggers_);
    if (tag_it == end_it) {
        boost::shared_ptr<Logging::TextSink> sink = detail::make_sink();
        boost::shared_ptr<std::ostream> stream(&std::clog,
                                               boost::null_deleter());
        sink->locked_backend()->add_stream(stream);
        detail::configure_sink(sink, level, tag.tag());
        Logging::loggers_[tag] = sink;
        ret = true;
    }

    return ret;
}

bool Logging::add_file_logger(const LogTag& tag,
                              const boost::filesystem::path& log_path,
                              boost::log::trivial::severity_level level) {
    bool ret = false;

    std::lock_guard<std::mutex> lock(Logging::loggers_mutex_);
    auto end_it = Logging::loggers_.end();
    auto tag_it = detail::find_sink(tag, Logging::loggers_);
    if (tag_it == end_it) {
        boost::shared_ptr<Logging::TextSink> sink = detail::make_sink();
        sink->locked_backend()->add_stream(
                boost::make_shared<std::ofstream>(log_path.string()));
        detail::configure_sink(sink, level, tag.tag());
        Logging::loggers_[tag] = sink;
        ret = true;
    }

    return ret;
}

void Logging::write_severity_log(const LogTag& tag,
                                 boost::log::trivial::severity_level level,
                                 const std::string& message) {
    Logging::init();
    boost::log::sources::severity_logger<boost::log::trivial::severity_level>
            s_logger;

    BOOST_LOG_SCOPED_THREAD_TAG("Tag", tag.tag());
    BOOST_LOG_SEV(s_logger, level) << message;
    std::cout.flush();
    std::cerr.flush();
    std::clog.flush();
}

bool Logging::clear_all() {
    bool ret = true;

    std::lock_guard<std::mutex> lock(Logging::loggers_mutex_);

    std::for_each(Logging::loggers_.begin(),
                  Logging::loggers_.end(),
                  [&](std::unordered_map<LogTag, boost::weak_ptr<TextSink>>::
                              value_type& pair) {
                      boost::log::core::get()->remove_sink(pair.second.lock());
                      pair.second.reset();
                  });
    Logging::loggers_.clear();
    // Get rid of default sink.
    boost::log::core::get()->remove_all_sinks();
    ret = Logging::loggers_.empty();

    return ret;
}
}  // namespace com::dag_scheduler
