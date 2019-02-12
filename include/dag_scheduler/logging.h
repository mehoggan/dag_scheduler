#ifndef COM_VISUALIZATION_LOGGING_H_INCLUDED
#define COM_VISUALIZATION_LOGGING_H_INCLUDED

#define BOOST_LOG_DYN_LINK 1

#include "declspec.h"

#include <boost/filesystem/path.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/trivial.hpp>
#include <boost/weak_ptr.hpp>

#include <gtest/gtest_prod.h>

#include <atomic>
#include <sstream>
#include <string>
#include <unordered_map>
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
    /**
     * @brief A class used to uniqlly identify a specific log.
     *
     * A generic class used to identify a specific log and its source.
     */
    struct log_tag
    {
    public:
      /**
       * @brief ctor for a \ref log_tag.
       *
       * @param[in] tag A string used in the logs to identify the source and
       *                to uniquly identify a log.
       */
      explicit log_tag(const std::string &tag);

      /**
       * @brief A getter for the string used to represent the \ref log_tag.
       *
       * A member getter function of \ref log_tag that returns the string
       * representation of the \ref log_tag.
       *
       * @return A null terminated string representation of \ref log_tag. 
       */
      const char *tag() const;

      /**
       * @brief A utility output operator.
       *
       * A utility output operator for a \ref log_tag.
       *
       * @param[in] os The stream to write to.
       * @param[in] tag The \ref log_tag to write to \os.
       *
       * @return The stream written to. 
       */
      friend std::ostream &operator<<(std::ostream &os, const log_tag &tag)
      {
        os << tag.tag();
        return os;
      }

      /**
       * @brief A utility output operator.
       *
       * @param[in] ss The stream to write to.
       * @param[in] tag The \ref log_tag to write to \p ss.
       *
       * @return The stream \p ss after being written to. 
       */
      friend std::stringstream &operator<<(std::stringstream &ss,
        const log_tag &tag)
      {
        ss << tag.tag();
        return ss;
      }

      /**
       * @brief A utility operator to test for equality between \ref log_tag.
       *
       * @param[in] lhs The \ref log_tag on the left hand side of the '==`.
       * @param[in] rhs The \ref log_tag on the right hand side of the '=='.
       *
       * @return true if \p lhs tag_ is a string equivalent to \p rhs tag_. 
       */
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
    size_t operator()(const com::dag_scheduler::log_tag &obj) const
    {
      return hash<std::string> {}(obj.tag());
    }
  };
}

namespace com
{
  namespace dag_scheduler
  {
    /**
     * @brief A class used to log to various sinks or sources.based on a tag.
     *
     * A generic class used to log to various sinks or sources based of a
     * \ref log_tag.
     */
    class logging
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
      typedef boost::log::sinks::synchronous_sink<
        boost::log::sinks::text_ostream_backend> TextSink;
      typedef std::unordered_map<log_tag, boost::weak_ptr<TextSink>> Dict;

      /**
       * @brief Used to create a stdout source tied to a \ref log_tag.
       *
       * A static function used to create a stdout source tied to a tag.
       *
       * @param[in] tag The \ref log_tag used to identify logs going to
                        stdout stream.
       * @param[in] level A filter for a give sink and the \ref tag.
       *
       * @return true if \p tag was not already added to logger, false
       *         otherwise.
       */
      static bool add_std_cout_logger(const log_tag &tag,
        boost::log::trivial::severity_level level = DAG_SCHEDULER_INFO);

      /**
       * @brief Used to create a stderr source tied to a \ref log_tag.
       *
       * A static function used to create a stderr source tied to a tag.
       *
       * @param[in] tag The \ref log_tag used to identify logs going to
                    stderr stream.
       * @param[in] level A filter for a give sink and the \ref tag.
       *
       * @return true if \p tag was not already added to logger, false
       *         otherwise.
       */
      static bool add_std_cerr_logger(const log_tag &tag,
        boost::log::trivial::severity_level level = DAG_SCHEDULER_ERROR);

      /**
       * @brief Used to create a stdlog source tied to a \ref log_tag.
       *
       * @param[in] tag The \ref log_tag used to identify logs going to
                        stdlog stream.
       * @param[in] level A filter for a give sink and the \ref tag.
       *
       * @return true if \p tag was not already added to logger, false
       *         otherwise.
       */
      static bool add_std_log_logger(const log_tag &tag,
        boost::log::trivial::severity_level level = DAG_SCHEDULER_TRACE);

      /**
       * @brief Used to create a file source tied to a \ref log_tag.
       *
       * A static function used to create a file source tied to a tag.
       *
       * @param[in] tag The \ref log_tag used to identify logs going to
                        a specific file stream.
       * @param[in] log_path The path to the file to log to.
       * @param[in] level A filter for a give sink and the \ref tag.
       *
       * @return 
       */
      static bool add_file_logger(const log_tag &tag,
        const boost::filesystem::path &log_path,
        boost::log::trivial::severity_level level = DAG_SCHEDULER_INFO);

      /**
       * @brief A utility function used to generate a log tag for a specific
       *        class. 
       *
       * @tparam T The type for the instance to generate a \ref log_tag for.
       * @param[in] instance The instance to use RTTI on for the \ref log_tag.
       *
       * @return A \ref log_tag generated by RTTI on \p instance.
       */
      template<typename T>
      static log_tag log_tag_for_this(const T &instance)
      {
        std::stringstream ss;
        ss << typeid(T).name() << "_" << (&(instance));
        return log_tag(ss.str());
      }

      /**
       * @brief If you want to log without a empty \ref log_tag call this.
       *
       * Used to set up common attributes for loggers to enable logging
       * without a \ref log_tag.
       * @return true if logging setup or on first call, false otherwise. 
       */
      static bool init();

      /**
       * @brief Utility function for creating a tmp directory for where to
       *        write log files.
       *
       * @param[in] model The format to be used in creating a tmp directory
       *
       * @return A \ref boost::filesystem::path to where the tmp directory
       *         was created on the local filesystem.
       */
      static boost::filesystem::path mktmpdir(
        const boost::filesystem::path &model = "%%%%-%%%%-%%%%-%%%%");

      /**
       * @brief A key function for logging at the \ref DAG_SCHEDULER_TRACE
       *        level.
       *
       * This static function logs any user defined type that can be written
       * to a \ref std::ostream and will use the tag for filtering based on
       * the call to \ref logging::add_std_cout_logger.
       *
       * @tparam ...Ts A list of mixed types which all must have support for
       *               writting to a string stream.
       * @param[in] tag A \ref log_tag used in filtering.
       * @param[in] args A variadic list of args that can be written to a
                         \ref std::ostream.
       */
      template<typename ...Ts>
      static void trace(const log_tag &tag, Ts&&... args)
      {
        std::stringstream ss;
        logging::log_string_builder(ss, args...);
        write_severity_log(tag, DAG_SCHEDULER_TRACE, ss.str());
      }

      /**
       * @brief A key function for logging at the \ref DAG_SCHEDULER_DEBUG
       *        level.
       * 
       * This static function logs any user defined type that can be written
       * to a \ref std::ostream and will use the tag for filtering based on
       * the call to \ref logging::add_std_cout_logger.
       *
       * @tparam ...Ts A list of mixed types which all must have support for
       *               writting to a string stream.
       * @param[in] tag A \ref log_tag used in filtering.
       * @param[in] args A variadic list of args that can be written to a
       *                 \ref std::ostream.
       */
      template<typename ...Ts>
      static void debug(const log_tag &tag, Ts&&... args)
      {
        std::stringstream ss;
        logging::log_string_builder(ss, args...);
        write_severity_log(tag, DAG_SCHEDULER_DEBUG, ss.str());
      }

      /**
       * @brief A key function for logging at the \ref DAG_SCHEDULER_INFO
       *        level.
       *
       * This static function logs any user defined type that can be written
       * to a \ref std::ostream and will use the tag for filtering based on
       * the call to \ref logging::add_std_cout_logger.
       *
       * @tparam ...Ts A list of mixed types which all must have support for
       *               writting to a string stream.
       * @param[in] tag A \ref log_tag used in filtering.
       * @param[in] args A variadic list of args that can be written to a
       *                 \ref std::ostream.
       */
      template<typename ...Ts>
      static void info(const log_tag &tag, Ts&&... args)
      {
        std::stringstream ss;
        logging::log_string_builder(ss, args...);
        write_severity_log(tag, DAG_SCHEDULER_INFO, ss.str());
      }

      /**
       * @brief A key function for logging at the \ref DAG_SCHEDULER_WARN
       *        level.
       *
       * This static function logs any user defined type that can be written
       * to a \ref std::ostream and will use the tag for filtering based on
       * the call to \ref logging::add_std_cout_logger.
       *
       * @tparam ...Ts A list of mixed types which all must have support for
       *               writting to a string stream.
       * @param[in] tag A \ref log_tag used in filtering.
       * @param[in] args A variadic list of args that can be written to a
       *                 \ref std::ostream.
       */
      template<typename ...Ts>
      static void warn(const log_tag &tag, Ts&&... args)
      {
        std::stringstream ss;
        logging::log_string_builder(ss, args...);
        write_severity_log(tag, DAG_SCHEDULER_WARN, ss.str());
      }

      /**
       * @brief A key function for logging at the \ref DAG_SCHEDULER_ERROR
       *        level.
       *
       * This static function logs any user defined type that can be written
       * to a \ref std::ostream and will use the tag for filtering based on
       * the call to \ref logging::add_std_cerr_logger.
       *
       * @tparam ...Ts A list of mixed types which all must have support for
       *               writting to a string stream.
       * @param[in] tag A \ref log_tag used in filtering.
       * @param[in] args A variadic list of args that can be written to a
       *             \ref std::ostream.
       */
      template<typename ...Ts>
      static void error(const log_tag &tag, Ts&&... args)
      {
        std::stringstream ss;
        logging::log_string_builder(ss, args...);
        write_severity_log(tag, DAG_SCHEDULER_ERROR, ss.str());
      }

      /**
       * @brief A key function for logging at the \ref DAG_SCHEDULER_FATAL
       *        level.
       *
       * This static function logs any user defined type that can be written
       * to a \ref std::ostream and will use the tag for filtering based on
       * the call to \ref logging::add_std_cerr_logger.
       *
       * @tparam ...Ts A list of mixed types which all must have support for
       *               writting to a string stream.
       * @param tag A \ref log_tag used in filtering.
       * @param args A variadic list of args that can be written to a
       *             \ref std::ostream.
       */
      template<typename ...Ts>
      static void fatal(const log_tag &tag, Ts&&... args)
      {
        std::stringstream ss;
        logging::log_string_builder(ss, args...);
        write_severity_log(tag, DAG_SCHEDULER_FATAL, ss.str());
      }

      /**
       * @brief Removes all sinks and clears the tags. 
       *
       * @return true if successful, false otherwise. 
       */
      static bool clear_all();

    private:
      static void write_severity_log(const log_tag &tag,
        boost::log::trivial::severity_level level,
        const std::string &message);

    private:
      log_tag tag_;

    private:
      static std::atomic<bool> init_;
      static Dict loggers_;
      static std::mutex loggers_mutex_;

    private:
      FRIEND_TEST(TestLogging, test_clear_all);
    };
 
    /**
     * @brief Adds a logger for a type \t T to the logger.
     *
     * A template function that takes a type T and generates a unique string
     * to be used in a log_tag.
     *
     * @tparam T The type for \p t for which RTTI is used on to get a
     *           \ref log_tag.
     * @param[in] t An instance of \tref T used to generate a \ref log_tag.
     * @param[in] cout_level The level at which to log to std::cout.
     * @param[in] cerr_level The level at which to log to std::cerr.
     *
     * @return A \ref log_tag generated using RTTI for \p t.
     */
    template <class T>
    log_tag add_std_cout_std_err_logger_for_type_instance(const T &t,
      boost::log::trivial::severity_level cout_level = DAG_SCHEDULER_INFO,
      boost::log::trivial::severity_level cerr_level = DAG_SCHEDULER_ERROR)
    {
      log_tag LOG_TAG = logging::log_tag_for_this(t);
      logging::add_std_cout_logger(LOG_TAG, cout_level);
      logging::add_std_cerr_logger(LOG_TAG, cerr_level);
      return LOG_TAG;
    }
  }
}
#endif
