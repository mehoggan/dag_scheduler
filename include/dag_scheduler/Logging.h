////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2025 Directed Acyclic Graph Scheduler
// All rights reserved.
//
// Contact: mehoggan@gmail.com
//
// This software is licensed under the terms of the Your License.
// See the LICENSE file in the top-level directory.
/////////////////////////////////////////////////////////////////////////
#ifndef DAG_SCHEDULER_LOGGING_H
#define DAG_SCHEDULER_LOGGING_H

#define BOOST_LOG_DYN_LINK 1

#include <gtest/gtest_prod.h>

#include <atomic>
#include <boost/filesystem/path.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/trivial.hpp>
#include <boost/weak_ptr.hpp>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>

#define DAG_SCHEDULER_TRACE boost::log::trivial::severity_level::trace
#define DAG_SCHEDULER_DEBUG boost::log::trivial::severity_level::debug
#define DAG_SCHEDULER_INFO boost::log::trivial::severity_level::info
#define DAG_SCHEDULER_WARN boost::log::trivial::severity_level::warning
#define DAG_SCHEDULER_ERROR boost::log::trivial::severity_level::error
#define DAG_SCHEDULER_FATAL boost::log::trivial::severity_level::fatal

namespace com::dag_scheduler {
/**
 * @brief A class used to uniquely identify a specific log.
 *
 * A generic class used to identify a specific log and its source.
 */
struct LogTag {
public:
    /**
     * @brief ctor for a \ref LogTag.
     *
     * @param[in] tag A string used in the logs to identify the source and
     *                to uniquely identify a log.
     */
    explicit LogTag(const std::string& tag);

    /**
     * @brief A getter for the string used to represent the \ref LogTag.
     *
     * A member getter function of \ref LogTag that returns the string
     * representation of the \ref LogTag.
     *
     * @return A null terminated string representation of \ref LogTag.
     */
    const char* tag() const;

    /**
     * @brief A utility output operator.
     *
     * A utility output operator for a \ref LogTag.
     *
     * @param[in] os The stream to write to.
     * @param[in] tag The \ref LogTag to write to \os.
     *
     * @return The stream written to.
     */
    friend std::ostream& operator<<(std::ostream& stream, const LogTag& tag) {
        stream << tag.tag();
        return stream;
    }

    /**
     * @brief A utility output operator.
     *
     * @param[in] ss The stream to write to.
     * @param[in] tag The \ref LogTag to write to \p ss.
     *
     * @return The stream \p ss after being written to.
     */
    friend std::stringstream& operator<<(std::stringstream& sstream,
                                         const LogTag& tag) {
        sstream << tag.tag();
        return sstream;
    }

    /**
     * @brief A utility operator to test for equality between \ref LogTag.
     *
     * @param[in] lhs The \ref LogTag on the left hand side of the '==`.
     * @param[in] rhs The \ref LogTag on the right hand side of the '=='.
     *
     * @return true if \p lhs tag_ is a string equivalent to \p rhs tag_.
     */
    friend bool operator==(const LogTag& lhs, const LogTag& rhs) {
        return lhs.tag() == rhs.tag();
    }

private:
    std::string tag_;
};
}  // namespace com::dag_scheduler

namespace std {
template <> struct hash<com::dag_scheduler::LogTag> {
    size_t operator()(const com::dag_scheduler::LogTag& obj) const {
        return hash<std::string>{}(obj.tag());
    }
};
}  // namespace std

namespace com::dag_scheduler {
/**
 * @brief A class used to log to various sinks or sources.based on a tag.
 *
 * A generic class used to log to various sinks or sources based of a
 * \ref LogTag.
 */
class Logging {
private:
    template <typename T>
    static void logStringBuilder(std::stringstream& sstream, T&& arg) {
        sstream << std::forward<T>(arg);
    }

    template <typename T, typename... Ts>
    static void logStringBuilder(std::stringstream& sstream,
                                 T&& arg,
                                 Ts&&... args) {
        sstream << std::forward<T>(arg) << " ";
        logStringBuilder(sstream, args...);
    }

public:
    typedef boost::log::sinks::synchronous_sink<
            boost::log::sinks::text_ostream_backend>
            TextSink;
    typedef std::unordered_map<LogTag, boost::weak_ptr<TextSink>> Dict;

    /**
     * @brief Used to create a stdout source tied to a \ref LogTag.
     *
     * A static function used to create a stdout source tied to a tag.
     *
     * @param[in] tag The \ref LogTag used to identify logs going to
                      stdout stream.
     * @param[in] level A filter for a give sink and the \ref tag.
     *
     * @return true if \p tag was not already added to logger, false
     *         otherwise.
     */
    static bool addStdCoutLogger(
            const LogTag& tag,
            boost::log::trivial::severity_level level = DAG_SCHEDULER_INFO);

    /**
     * @brief Used to create a stderr source tied to a \ref LogTag.
     *
     * A static function used to create a stderr source tied to a tag.
     *
     * @param[in] tag The \ref LogTag used to identify logs going to
                  stderr stream.
     * @param[in] level A filter for a give sink and the \ref tag.
     *
     * @return true if \p tag was not already added to logger, false
     *         otherwise.
     */
    static bool addStdCerrLogger(
            const LogTag& tag,
            boost::log::trivial::severity_level level = DAG_SCHEDULER_ERROR);

    /**
     * @brief Used to create a stdlog source tied to a \ref LogTag.
     *
     * @param[in] tag The \ref LogTag used to identify logs going to
                      stdlog stream.
     * @param[in] level A filter for a give sink and the \ref tag.
     *
     * @return true if \p tag was not already added to logger, false
     *         otherwise.
     */
    static bool addStdLogLogger(
            const LogTag& tag,
            boost::log::trivial::severity_level level = DAG_SCHEDULER_TRACE);

    /**
     * @brief Used to create a file source tied to a \ref LogTag.
     *
     * A static function used to create a file source tied to a tag.
     *
     * @param[in] tag The \ref LogTag used to identify logs going to
                      a specific file stream.
     * @param[in] log_path The path to the file to log to.
     * @param[in] level A filter for a give sink and the \ref tag.
     *
     * @return
     */
    static bool addFileLogger(
            const LogTag& tag,
            const boost::filesystem::path& log_path,
            boost::log::trivial::severity_level level = DAG_SCHEDULER_INFO);

    /**
     * @brief A utility function used to generate a log tag for a specific
     *        class.
     *
     * @tparam T The type for the instance to generate a \ref LogTag for.
     * @param[in] instance The instance to use RTTI on for the \ref LogTag.
     *
     * @return A \ref LogTag generated by RTTI on \p instance.
     */
    template <typename T> static LogTag logTagForThis(const T& instance) {
        std::stringstream sstream;
        sstream << typeid(T).name() << "_" << (&(instance));
        return LogTag(sstream.str());
    }

    /**
     * @brief If you want to log without a empty \ref LogTag call this.
     *
     * Used to set up common attributes for loggers to enable Logging
     * without a \ref LogTag.
     * @return true if Logging setup or on first call, false otherwise.
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
            const boost::filesystem::path& model = "%%%%-%%%%-%%%%-%%%%");

    /**
     * @brief A key function for Logging at the \ref DAG_SCHEDULER_TRACE
     *        level.
     *
     * This static function logs any user defined type that can be written
     * to a \ref std::ostream and will use the tag for filtering based on
     * the call to \ref Logging::add_std_cout_logger.
     *
     * @tparam ...Ts A list of mixed types which all must have support for
     *               writing to a string stream.
     * @param[in] tag A \ref LogTag used in filtering.
     * @param[in] args A variadic list of args that can be written to a
                       \ref std::ostream.
     */
    template <typename... Ts>
    static void trace(const LogTag& tag, Ts&&... args) {
        std::stringstream sstream;
        Logging::logStringBuilder(sstream, args...);
        writeSeverityLog(tag, DAG_SCHEDULER_TRACE, sstream.str());
    }

    /**
     * @brief A key function for Logging at the \ref DAG_SCHEDULER_DEBUG
     *        level.
     *
     * This static function logs any user defined type that can be written
     * to a \ref std::ostream and will use the tag for filtering based on
     * the call to \ref Logging::add_std_cout_logger.
     *
     * @tparam ...Ts A list of mixed types which all must have support for
     *               writing to a string stream.
     * @param[in] tag A \ref LogTag used in filtering.
     * @param[in] args A variadic list of args that can be written to a
     *                 \ref std::ostream.
     */
    template <typename... Ts>
    static void debug(const LogTag& tag, Ts&&... args) {
        std::stringstream sstream;
        Logging::logStringBuilder(sstream, args...);
        writeSeverityLog(tag, DAG_SCHEDULER_DEBUG, sstream.str());
    }

    /**
     * @brief A key function for Logging at the \ref DAG_SCHEDULER_INFO
     *        level.
     *
     * This static function logs any user defined type that can be written
     * to a \ref std::ostream and will use the tag for filtering based on
     * the call to \ref Logging::add_std_cout_logger.
     *
     * @tparam ...Ts A list of mixed types which all must have support for
     *               writing to a string stream.
     * @param[in] tag A \ref LogTag used in filtering.
     * @param[in] args A variadic list of args that can be written to a
     *                 \ref std::ostream.
     */
    template <typename... Ts>
    static void info(const LogTag& tag, Ts&&... args) {
        std::stringstream sstream;
        Logging::logStringBuilder(sstream, args...);
        writeSeverityLog(tag, DAG_SCHEDULER_INFO, sstream.str());
    }

    /**
     * @brief A key function for Logging at the \ref DAG_SCHEDULER_WARN
     *        level.
     *
     * This static function logs any user defined type that can be written
     * to a \ref std::ostream and will use the tag for filtering based on
     * the call to \ref Logging::add_std_cout_logger.
     *
     * @tparam ...Ts A list of mixed types which all must have support for
     *               writing to a string stream.
     * @param[in] tag A \ref LogTag used in filtering.
     * @param[in] args A variadic list of args that can be written to a
     *                 \ref std::ostream.
     */
    template <typename... Ts>
    static void warn(const LogTag& tag, Ts&&... args) {
        std::stringstream sstream;
        Logging::logStringBuilder(sstream, args...);
        writeSeverityLog(tag, DAG_SCHEDULER_WARN, sstream.str());
    }

    /**
     * @brief A key function for Logging at the \ref DAG_SCHEDULER_ERROR
     *        level.
     *
     * This static function logs any user defined type that can be written
     * to a \ref std::ostream and will use the tag for filtering based on
     * the call to \ref Logging::add_std_cerr_logger.
     *
     * @tparam ...Ts A list of mixed types which all must have support for
     *               writing to a string stream.
     * @param[in] tag A \ref LogTag used in filtering.
     * @param[in] args A variadic list of args that can be written to a
     *             \ref std::ostream.
     */
    template <typename... Ts>
    static void error(const LogTag& tag, Ts&&... args) {
        std::stringstream sstream;
        Logging::logStringBuilder(sstream, args...);
        writeSeverityLog(tag, DAG_SCHEDULER_ERROR, sstream.str());
    }

    /**
     * @brief A key function for Logging at the \ref DAG_SCHEDULER_FATAL
     *        level.
     *
     * This static function logs any user defined type that can be written
     * to a \ref std::ostream and will use the tag for filtering based on
     * the call to \ref Logging::add_std_cerr_logger.
     *
     * @tparam ...Ts A list of mixed types which all must have support for
     *               writing to a string stream.
     * @param tag A \ref LogTag used in filtering.
     * @param args A variadic list of args that can be written to a
     *             \ref std::ostream.
     */
    template <typename... Ts>
    static void fatal(const LogTag& tag, Ts&&... args) {
        std::stringstream sstream;
        Logging::logStringBuilder(sstream, args...);
        writeSeverityLog(tag, DAG_SCHEDULER_FATAL, sstream.str());
    }

    /**
     * @brief Removes all sinks and clears the tags.
     *
     * @return true if successful, false otherwise.
     */
    static bool clearAll();

private:
    static void writeSeverityLog(const LogTag& tag,
                                 boost::log::trivial::severity_level level,
                                 const std::string& message);

private:
    LogTag tag_;

private:
    static std::atomic<bool> s_init;
    static Dict s_loggers;
    static std::mutex s_loggers_mutex;

private:
    FRIEND_TEST(TestLogging, testClearAll);
};

/**
 * @brief Adds a logger for a type \t T to the logger.
 *
 * A template function that takes a type T and generates a unique string
 * to be used in a LogTag.
 *
 * @tparam T The type for \p t for which RTTI is used on to get a
 *           \ref LogTag.
 * @param[in] t An instance of \tref T used to generate a \ref LogTag.
 * @param[in] cout_level The level at which to log to std::cout.
 * @param[in] cerr_level The level at which to log to std::cerr.
 *
 * @return A \ref LogTag generated using RTTI for \p t.
 */
template <class T>
LogTag addStdCoutStdErrLoggerForTypeInstance(
        const T& type,
        boost::log::trivial::severity_level cout_level = DAG_SCHEDULER_INFO,
        boost::log::trivial::severity_level cerr_level = DAG_SCHEDULER_ERROR) {
    LogTag log_tag = Logging::logTagForThis(type);
    Logging::addStdCoutLogger(log_tag, cout_level);
    Logging::addStdCerrLogger(log_tag, cerr_level);
    return log_tag;
}
}  // namespace com::dag_scheduler
#endif
