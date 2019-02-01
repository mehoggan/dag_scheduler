#ifndef LOGGED_CLASS_H_INCLUDED
#define LOGGED_CLASS_H_INCLUDED

#include "declspec.h"

#include "dag_scheduler/logging.h"

namespace com
{
  namespace dag_scheduler
  {
    //! A class that sets up stderror and stdout loggers for a derived class.
    /*!
      If you inherrit from this class you will get a logger added to logging
      for specific instance of this class.
    */
    template <typename DerivedType>
    class logged_class
    {
    public:
      explicit logged_class(const DerivedType &d,
        boost::log::trivial::severity_level coutLevel = DAG_SCHEDULER_INFO,
        boost::log::trivial::severity_level cerrLevel = DAG_SCHEDULER_ERROR) :
        LOG_TAG(logging::log_tag_for_this(d))
      {
        logging::add_std_cout_logger(LOG_TAG, coutLevel);
        logging::add_std_cerr_logger(LOG_TAG, cerrLevel);
      }

      virtual ~logged_class()
      {}

    protected:
      log_tag LOG_TAG;

    private:
      logged_class(const logged_class &other) = delete;
      logged_class &operator=(const logged_class &rhs) = delete;
      logged_class(logged_class &&rhs) = delete;
      logged_class &operator=(logged_class &&rhs) = delete;
    };
  }
}
#endif
