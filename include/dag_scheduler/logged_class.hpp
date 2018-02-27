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
      logged_class(const DerivedType &d,
        boost::log::trivial::severity_level cout_level = DAG_SCHEDULER_INFO,
        boost::log::trivial::severity_level cerr_level = DAG_SCHEDULER_ERROR)
        : LOG_TAG(logging::log_tag_for_this(d))
      {
        logging::add_std_cout_logger(LOG_TAG);
        logging::add_std_cerr_logger(LOG_TAG);
      }

    protected:
      log_tag LOG_TAG;
    };
  }
}
#endif
