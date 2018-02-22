#ifndef COM_VISUALIZATION_LOGGING_H_INCLUDED
#define COM_VISUALIZATION_LOGGING_H_INCLUDED

#define BOOST_LOG_DYN_LINK 1

#include "declspec.h"

#include <boost/log/trivial.hpp>
#include <boost/filesystem/path.hpp>

#include <atomic>
#include <sstream>
#include <string>
#include <unordered_set>
#include <thread>
#include <mutex>

#define DAG_SCHEDULER_TRACE boost::log::trivial::severity_level::trace
#define DAG_SCHEDULER_DEBUG boost::log::trivial::severity_level::debug
#define DAG_SCHEDULER_INFO boost::log::trivial::severity_level::info
#define DAG_SCHEDULER_WARN boost::log::trivial::severity_level::warning
#define DAG_SCHEDULER_ERROR boost::log::trivial::severity_level::error
#define DAG_SCHEDULER_FATAL boost::log::trivial::severity_level::fatal

namespace com
{
  namespace dag_scheduler
  {
    struct DLLSPEC_DAGTASKS log_tag
    {
    public:
      explicit log_tag(const::std::string &tag);

      const char *tag() const;

      friend std::ostream &operator<<(std::ostream &os, const log_tag &tag)
      {
        os << tag.tag();
        return os;
      }

      friend std::stringstream &operator<<(std::stringstream &ss,
        const log_tag &tag)
      {
        ss << tag.tag();
        return ss;
      }

      friend bool operator==(const log_tag &lhs, const log_tag &rhs)
      {
        return lhs.tag() == rhs.tag();
      }

    private:
      std::string tag_;
    };
  }
}

namespace std
{
  template<>
  struct hash<com::dag_scheduler::log_tag>
  {
    size_t
    operator()(const com::dag_scheduler::log_tag &obj) const
    {
      return hash<std::string>()(obj.tag());
    }
  };
}

namespace com
{
  namespace dag_scheduler
  {
    class DLLSPEC_DAGTASKS logging
    {
    private:
      template<typename T>
      static void log_string_builder(std::stringstream &ss, T &&arg)
      {
        ss << std::forward<T>(arg);
      }

      template<typename T, typename... Ts>
      static void log_string_builder(std::stringstream &ss, T &&arg,
        Ts&&... args)
      {
        ss << std::forward<T>(arg) << " ";
        log_string_builder(ss, args...);
      }

    public:
      static void add_std_cout_logger(const log_tag &tag,
        boost::log::trivial::severity_level level = DAG_SCHEDULER_INFO);
      static void add_std_cerr_logger(const log_tag &tag,
        boost::log::trivial::severity_level level = DAG_SCHEDULER_ERROR);
      static void add_std_log_logger(const log_tag &tag,
        boost::log::trivial::severity_level level = DAG_SCHEDULER_TRACE);
      static void add_file_logger(const log_tag &tag,
        const boost::filesystem::path &log_path,
        boost::log::trivial::severity_level level = DAG_SCHEDULER_INFO);

      template<typename T>
      static log_tag log_tag_for_this(T &instance)
      {
        std::stringstream ss;
        ss << typeid(T).name() << "_" << (&(instance));
        return log_tag(ss.str());
      }

    public:
      static bool init();

      static boost::filesystem::path mktmpdir(
        const boost::filesystem::path &model = "%%%%-%%%%-%%%%-%%%%");

      template<typename ...Ts>
      static void trace(const log_tag &tag, Ts&&... args)
      {
        std::stringstream ss;
        logging::log_string_builder(ss, args...);
        write_severity_log(tag, DAG_SCHEDULER_TRACE, ss.str());
      }

      template<typename ...Ts>
      static void debug(const log_tag &tag, Ts&&... args)
      {
        std::stringstream ss;
        logging::log_string_builder(ss, args...);
        write_severity_log(tag, DAG_SCHEDULER_DEBUG, ss.str());
      }

      template<typename ...Ts>
      static void info(const log_tag &tag, Ts&&... args)
      {
        std::stringstream ss;
        logging::log_string_builder(ss, args...);
        write_severity_log(tag, DAG_SCHEDULER_INFO, ss.str());
      }

      template<typename ...Ts>
      static void warn(const log_tag &tag, Ts&&... args)
      {
        std::stringstream ss;
        logging::log_string_builder(ss, args...);
        write_severity_log(tag, DAG_SCHEDULER_WARN, ss.str());
      }

      template<typename ...Ts>
      static void error(const log_tag &tag, Ts&&... args)
      {
        std::stringstream ss;
        logging::log_string_builder(ss, args...);
        write_severity_log(tag, DAG_SCHEDULER_ERROR, ss.str());
      }

      template<typename ...Ts>
      static void fatal(const log_tag &tag, Ts&&... args)
      {
        std::stringstream ss;
        logging::log_string_builder(ss, args...);
        write_severity_log(tag, DAG_SCHEDULER_FATAL, ss.str());
      }

    private:
      static void write_severity_log(
        const log_tag &tag,
        boost::log::trivial::severity_level level,
        const std::string &message);

    private:
      log_tag tag_;

    private:
      static std::atomic<bool> init_;
      static std::unordered_set<log_tag> loggers_;
      static std::mutex loggers_mutex_;
    };
  }
}
#endif
