lib_LTLIBRARIES = libdag_scheduler.la

libdag_scheduler_la_CPPFLAGS = -Werror -Wall -pedantic \
		-Wno-deprecated-declarations \
		-Wno-enum-constexpr-conversion \
		-I$(abs_top_srcdir)/include

if OSX
libdag_scheduler_la_CPPFLAGS += -I$(HOMEBREW_PREFIX)/include
endif

libdag_scheduler_la_SOURCES = BaseTaskStage.cxx \
		ConcurrentTaskQueue.cxx \
    Dag.cxx \
    DagAlgorithms.cxx \
    DagEdge.cxx \
		DagSerialization.cxx \
    DagVertex.cxx \
		DynamicLibraryRegistry.cxx \
    Endpoints.cxx \
    HttpsSession.cxx \
    InterruptibleTaskThread.cxx \
    Logging.cxx \
    ServiceHelpers.cxx \
    StopWatch.cxx \
    Task.cxx \
		TaskCallbackPlugin.cxx \
    TaskScheduler.cxx \
    TaskStage.cxx \
    Uuid.cxx \
    WorkflowService.cxx

libdag_scheduler_la_LIBADD = -lboost_filesystem -lboost_log -lboost_system \
	  -lboost_thread -lssl -lcrypto

if LINUX
libdag_scheduler_la_LIBADD += $(INIT_LIBS) -lm
libdag_scheduler_la_LDFLAGS = -pthread
else
libdag_scheduler_la_LIBADD += $(INIT_LIBS)
libdag_scheduler_la_LDFLAGS = -Wl,-ld_classic -pthread
endif

