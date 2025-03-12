lib_LTLIBRARIES = libdag_scheduler.la

libdag_scheduler_la_CPPFLAGS = -Werror -Wall -pedantic \
		-Wno-deprecated-declarations \
		-Wno-enum-constexpr-conversion \
		-I$(abs_top_srcdir)/include

if OSX
libdag_scheduler_la_CPPFLAGS += -I$(HOMEBREW_PREFIX)/include
endif

libdag_scheduler_la_SOURCES = concurrent_task_queue.cxx \
		base_task_stage.cxx \
    dag.cxx \
    dag_algorithms.cxx \
    dag_edge.cxx \
		dag_serialization.cxx \
    dag_vertex.cxx \
		dynamic_library_registry.cxx \
    endpoints.cxx \
    https_session.cxx \
    interruptible_task_thread.cxx \
    logging.cxx \
    service_helpers.cxx \
    stop_watch.cxx \
    task.cxx \
		task_callback_plugin.cxx \
    task_scheduler.cxx \
    task_stage.cxx \
    uuid.cxx \
    workflow_service.cxx

libdag_scheduler_la_LIBADD = -lboost_filesystem -lboost_log -lboost_system -lboost_thread \
		-lssl -lcrypto

if LINUX
libdag_scheduler_la_LIBADD += $(INIT_LIBS) -lm
else
libdag_scheduler_la_LIBADD += $(INIT_LIBS)
endif

libdag_scheduler_la_LDFLAGS = -Wl,-ld_classic -pthread
