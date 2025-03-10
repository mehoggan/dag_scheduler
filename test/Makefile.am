TESTS = gtest_libdag_scheduler
check_PROGRAMS = gtest_libdag_scheduler

gtest_libdag_scheduler_CPPFLAGS = -Werror -Wall -pedantic \
		-Wno-deprecated-declarations \
		-Wno-enum-constexpr-conversion \
	  -I$(abs_top_srcdir)/include \
	  -I$(abs_top_srcdir)/stages_lib/include \
	  -I$(abs_top_srcdir)/test/include

if OSX
gtest_libdag_scheduler_CPPFLAGS += -I$(HOMEBREW_PREFIX)/include
endif

gtest_libdag_scheduler_SOURCES = main.cxx \
    test_concurrent_task_queue.cxx \
    test_dag.cxx \
    test_dag_algorithms.cxx \
    test_dag_edge.cxx \
		test_dag_serialization.cxx \
    test_dag_vertex.cxx \
		test_dynamic_library_registery.cxx \
    test_interruptible_task_thread.cxx \
    test_logging.cxx \
    test_stop_watch.cxx \
    test_task.cxx \
    test_task_scheduler.cxx \
    test_task_stage.cxx \
    test_uuid.cxx \
    utils/test_environment.cxx \
    utils/test_task.cxx \
    utils/test_task_stage.cxx

gtest_libdag_scheduler_LDADD = \
		$(abs_top_builddir)/lib/libdag_scheduler.la

gtest_libdag_scheduler_LDADD += \
    -lgtest -lgmock -luuid -lyaml-cpp -lboost_program_options \
		-lboost_filesystem

if LINUX
gtest_libdag_scheduler_LDADD += \
    -lssl -lcrypto
endif
gtest_libdag_scheduler_LDADD += -lm
gtest_libdag_scheduler_LDFLAGS = -pthread

CLEANFILES = *.o
CLEANDIRS = .lib/
