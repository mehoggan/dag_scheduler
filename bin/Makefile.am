bin_PROGRAMS = https_dag_scheduler_runner

https_dag_scheduler_runner_SOURCES = main.cpp

https_dag_scheduler_runner_CPPFLAGS = -Werror -Wall -pedantic \
	-I$(abs_top_srcdir)/include \
	-I$(abs_top_srcdir)/deps/build/boost/include \
	-I$(abs_top_srcdir)/deps/build/openssl/include

https_dag_scheduler_runner_LDFLAGS = \
	-Wl,-rpath,$(abs_top_srcdir)/deps/build/boost/lib \
	-Wl,-rpath,$(abs_top_srcdir)/deps/build/openssl/lib \
	-lyaml-cpp -lboost_program_options -lm -pthread
https_dag_scheduler_runner_LDADD = $(abs_top_builddir)/lib/libdag_scheduler.la
if LINUX
https_dag_scheduler_runner_LDADD += -lrt
endif

CLEANFILES = *.o
CLEANDIRS = deps/ \
  .lib/
