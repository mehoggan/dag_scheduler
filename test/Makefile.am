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
    TestConcurrentTaskQueue.cxx \
    TestDag.cxx \
    TestDagAlgorithms.cxx \
    TestDagEdge.cxx \
		TestDagSerialization.cxx \
    TestDagVertex.cxx \
		TestDynamicLibraryRegistery.cxx \
    TestInterruptibleTaskThread.cxx \
    TestLogging.cxx \
    TestStopWatch.cxx \
    TestTask.cxx \
    TestTaskScheduler.cxx \
    TestTaskStage.cxx \
    TestUuid.cxx \
    utils/TestEnvironment.cxx \
    utils/TestTask.cxx \
    utils/TestTaskStage.cxx

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
if OSX
gtest_libdag_scheduler_LDFLAGS += \
		-Wl,-ld_classic
endif

CLEANFILES = *.o
CLEANDIRS = .lib/
