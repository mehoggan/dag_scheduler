AM_CPPFLAGS = -I$(srcdir)/include -I$(srcdir)/deps/boost \
							-I$(srcdir)/deps/openssl

lib_LTLIBRARIES = libdag_scheduler.la
libdag_scheduler_la_SOURCES = concurrent_task_queue.cxx \
    dag.cxx \
    dag_algorithms.cxx \
    dag_edge.cxx \
    dag_vertex.cxx \
    endpoints.cxx \
    https_session.cxx \
    interruptible_task_thread.cxx \
    logging.cxx \
    service_helpers.cxx \
    stop_watch.cxx \
    task.cxx \
    task_scheduler.cxx \
    task_stage.cxx \
    uuid.cxx \
    workflow_service.cxx


