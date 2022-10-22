bin_PROGRAMS = gtest_libdag_scheduler

gtest_libdag_scheduler_SOURCES = main.cxx \
  test_concurrent_task_queue.cxx \
  test_dag.cxx \
  test_dag_algorithms.cxx \
  test_dag_edge.cxx \
  test_dag_vertex.cxx \
  test_interruptible_task_thread.cxx \
  test_logging.cxx \
  test_stop_watch.cxx \
  test_task.cxx \
  test_task_scheduler.cxx \
  test_task_stage.cxx \
  test_uuid.cxx \
  utils/test_task.cxx \
  utils/test_task_stage.cxx

gtest_libdag_scheduler_CPPFLAGS = -Werror -Wall -pedantic \
	-I$(abs_top_srcdir)/include \
	-I$(abs_top_srcdir)/test/include \
	-I$(abs_top_srcdir)/deps/build/boost/include \
	-I$(abs_top_srcdir)/deps/build/openssl/include

gtest_libdag_scheduler_LDFLAGS = \
	-Wl,-rpath,$(abs_top_srcdir)/deps/build/boost/lib \
	-Wl,-rpath,$(abs_top_srcdir)/deps/build/openssl/lib \
	-lgtest -lgmock -lm -pthread
gtest_libdag_scheduler_LDADD = $(abs_top_builddir)/lib/libdag_scheduler.la
if LINUX
	gtest_libdag_scheduler_LDADD += -lrt
endif

CLEANFILES = *.o
CLEANDIRS = deps/ \
	.lib/

if LINUX
@VALGRIND_CHECK_RULES@
endif
