#ifndef LOGGED_CLASS_H_INCLUDED
#define LOGGED_CLASS_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/logging.h"

namespace com
{
  namespace dag_scheduler
  {
    /**
     * @brief A class that sets up stderror and stdout loggers for a derived
     *        class.
     *
     * If you inherrit from this class you will get a logger added to logging
     * for specific instance of this class.
     *
     * @tparam DerivedType Used in RTTI for an instance of \tref DerivedType.
     */
    template <typename DerivedType>
    class LoggedClass
    {
    public:
      /**
       * @brief ctor
       *
       * @param[in] d An instance of \tref DerivedType used to generate a
       *              \ref LogTag.
       * @param[in] cout_level The level at which to log to std::cout.
       * @param[in] cerr_level The level at which to log to std::cerr.
       */
      explicit LoggedClass(const DerivedType &d,
        boost::log::trivial::severity_level cout_level = DAG_SCHEDULER_INFO,
        boost::log::trivial::severity_level cerr_level = DAG_SCHEDULER_ERROR) :
        LOG_TAG(Logging::LogTag_for_this(d))
      {
        Logging::add_std_cout_logger(LOG_TAG, cout_level);
        Logging::add_std_cerr_logger(LOG_TAG, cerr_level);
      }

      /**
       * @brief dtor
       */
      virtual ~LoggedClass()
      {}

    protected:
      LogTag LOG_TAG;

    private:
      LoggedClass(const LoggedClass &other) = delete;
      LoggedClass &operator=(const LoggedClass &rhs) = delete;
      LoggedClass(LoggedClass &&rhs) = delete;
      LoggedClass &operator=(LoggedClass &&rhs) = delete;
    };
  }
}
#endif
