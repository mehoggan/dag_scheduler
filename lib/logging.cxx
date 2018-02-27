#include "dag_scheduler/logging.h"

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
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>
#include <fstream>

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity",
  boost::log::trivial::severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)

namespace com
{
  namespace dag_scheduler
  {
    std::atomic<bool> logging::init_(false);
    std::unordered_map<log_tag, boost::shared_ptr<logging::text_sink>>
      logging::loggers_;
    std::mutex logging::loggers_mutex_;

    namespace detail
    {
      boost::log::formatter fmt = boost::log::expressions::stream
        << "(" << std::setw(10) << std::setfill('0') << line_id << ")"
        << boost::log::expressions::if_(
          boost::log::expressions::has_attr(tag_attr))[
          boost::log::expressions::stream << "[" << tag_attr << "]"]
        << "<" << boost::log::expressions::attr<
        boost::log::attributes::current_thread_id::value_type>("ThreadID")
        << ">"
        << "[" << boost::log::expressions::format_date_time<
        boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S") << "]"
        << "[" << std::setw(5) << std::setfill(' ') << severity << "]"
        << " -- \'" << boost::log::expressions::smessage << "\'";


      boost::shared_ptr<logging::text_sink> make_sink()
      {
        boost::shared_ptr<logging::text_sink> sink =
          boost::make_shared<logging::text_sink>();
        return sink;
      }

      void configure_sink(boost::shared_ptr<logging::text_sink> sink,
        boost::log::trivial::severity_level level,
        const std::string &tag)
      {
        sink->set_formatter(detail::fmt);
        boost::log::core::get()->add_sink(sink);
        sink->set_filter(severity >= level &&
          (boost::log::expressions::has_attr(tag_attr) && tag_attr == tag));
      }
    }

    log_tag::log_tag(const ::std::string &tag) :
      tag_(tag)
    {}

    const char *log_tag::tag() const
    {
      return tag_.c_str();
    }

    bool logging::init()
    {
      if (!init_.load()) {
        boost::log::add_common_attributes();
        init_.store(true);
      }
      return !init_.load();
    }

    boost::filesystem::path logging::mktmpdir(
      const boost::filesystem::path &model)
    {
      boost::filesystem::path tmppath =
        boost::filesystem::unique_path(model);
      boost::filesystem::create_directories(tmppath);
      return tmppath;
    }

    bool logging::add_std_cout_logger(const log_tag &tag,
      boost::log::trivial::severity_level level)
    {
      bool ret = false;

      std::lock_guard<std::mutex> lock(logging::loggers_mutex_);
      if (logging::loggers_.find(tag) == logging::loggers_.end()) {
        boost::shared_ptr<logging::text_sink> sink = detail::make_sink();
        boost::shared_ptr<std::ostream> stream(&std::cout,
          boost::null_deleter());
        sink->locked_backend()->add_stream(stream);
        detail::configure_sink(sink, level, tag.tag());
        logging::loggers_[tag] = sink;
        ret = true;
      }

      return ret;
    }

    bool logging::add_std_cerr_logger(const log_tag &tag,
      boost::log::trivial::severity_level level)
    {
      bool ret = false;

      std::lock_guard<std::mutex> lock(logging::loggers_mutex_);
      if (logging::loggers_.find(tag) == logging::loggers_.end()) {
        boost::shared_ptr<logging::text_sink> sink = detail::make_sink();
        boost::shared_ptr<std::ostream> stream(&std::cerr,
          boost::null_deleter());
        sink->locked_backend()->add_stream(stream);
        detail::configure_sink(sink, level, tag.tag());
        logging::loggers_[tag] = sink;
        ret = true;
      }

      return ret;
    }

    bool logging::add_std_log_logger(const log_tag &tag,
      boost::log::trivial::severity_level level)
    {
      bool ret = false;

      std::lock_guard<std::mutex> lock(logging::loggers_mutex_);
      if (logging::loggers_.find(tag) == logging::loggers_.end()) {
        boost::shared_ptr<logging::text_sink> sink = detail::make_sink();
        boost::shared_ptr<std::ostream> stream(&std::clog,
          boost::null_deleter());
        sink->locked_backend()->add_stream(stream);
        detail::configure_sink(sink, level, tag.tag());
        logging::loggers_[tag] = sink;
        ret = true;
      }

      return ret;
    }

    bool logging::add_file_logger(const log_tag &tag,
      const boost::filesystem::path &log_path,
      boost::log::trivial::severity_level level)
    {
      bool ret = false;

      std::lock_guard<std::mutex> lock(logging::loggers_mutex_);
      if (logging::loggers_.find(tag) == logging::loggers_.end()) {
        boost::shared_ptr<logging::text_sink> sink = detail::make_sink();
        sink->locked_backend()->add_stream(
          boost::make_shared<std::ofstream>(log_path.string()));
        detail::configure_sink(sink, level, tag.tag());
        logging::loggers_[tag] = sink;
        ret = true;
      }

      return ret;
    }

    void logging::write_severity_log(
      const log_tag &tag,
      boost::log::trivial::severity_level level,
      const std::string &message)
    {
      logging::init();
      boost::log::sources::severity_logger<
        boost::log::trivial::severity_level> s_logger;

      BOOST_LOG_SCOPED_THREAD_TAG("Tag", tag.tag());
      BOOST_LOG_SEV(s_logger, level) << message;
      std::cout.flush();
      std::cerr.flush();
      std::clog.flush();
    }

    bool logging::clear_all()
    {
      bool ret = true;

      std::lock_guard<std::mutex> lock(logging::loggers_mutex_);

      std::for_each(logging::loggers_.begin(), logging::loggers_.end(),
        [&](std::unordered_map<log_tag,
          boost::shared_ptr<text_sink>>::value_type &pair) {
            std::cout << "Removing " << pair.first << " from backend."
              << std::endl;
            std::cout.flush();
            boost::log::core::get()->remove_sink(pair.second);
            pair.second.reset();
          });
      logging::loggers_.clear();
      ret = logging::loggers_.empty();

      return ret;
    }
  }
}
