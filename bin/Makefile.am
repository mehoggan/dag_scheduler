bin_PROGRAMS = https_dag_scheduler_runner dag_serializer_runner

#####
# Start of https_dag_scheduler_runner
#####
https_dag_scheduler_runner_SOURCES = service_main.cpp

https_dag_scheduler_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include \
    -I$(abs_top_srcdir)/deps/build/boost/include \
    -I$(abs_top_srcdir)/deps/build/openssl/include

https_dag_scheduler_runner_LDADD = \
    $(abs_top_builddir)/lib/libdag_scheduler.la \
    -L$(abs_top_srcdir)/deps/build/boost/lib
https_dag_scheduler_runner_LDFLAGS = \
    -Wl,-rpath,$(abs_top_srcdir)/deps/build/boost/lib
if LINUX
https_dag_scheduler_runner_LDADD += \
    -L$(abs_top_srcdir)/deps/build/openssl/lib64 \
 		-lrt
https_dag_scheduler_runner_LDFLAGS += \
    -Wl,-rpath,$(abs_top_srcdir)/deps/build/openssl/lib64
else
https_dag_scheduler_runner_LDADD += \
    -L$(abs_top_srcdir)/deps/build/openssl/lib
https_dag_scheduler_runner_LDFLAGS += \
    -Wl,-rpath,$(abs_top_srcdir)/deps/build/openssl/lib
endif

https_dag_scheduler_runner_LDADD += \
    -luuid -lyaml-cpp -lboost_program_options

if LINUX
https_dag_scheduler_runner_LDFLAGS += \
    -lssl -lcrypto
endif
https_dag_scheduler_runner_LDADD += -lm
https_dag_scheduler_runner_LDFLAGS += -pthread

#####
# Start of dag_serializer_runner
#####
dag_serializer_runner_SOURCES = dag_serializer_main.cpp

dag_serializer_runner_CPPFLAGS = -Werror -Wall -pedantic \
    -I$(abs_top_srcdir)/include \
    -I$(abs_top_srcdir)/deps/build/boost/include \
    -I$(abs_top_srcdir)/deps/build/openssl/include

dag_serializer_runner_LDADD = \
    $(abs_top_builddir)/lib/libdag_scheduler.la \
    -L$(abs_top_srcdir)/deps/build/boost/lib
dag_serializer_runner_LDFLAGS = \
    -Wl,-rpath,$(abs_top_srcdir)/deps/build/boost/lib
if LINUX
dag_serializer_runner_LDADD += \
    -L$(abs_top_srcdir)/deps/build/openssl/lib64 \
 		-lrt
dag_serializer_runner_LDFLAGS += \
    -Wl,-rpath,$(abs_top_srcdir)/deps/build/openssl/lib64
else
dag_serializer_runner_LDADD += \
    -L$(abs_top_srcdir)/deps/build/openssl/lib
dag_serializer_runner_LDFLAGS += \
    -Wl,-rpath,$(abs_top_srcdir)/deps/build/openssl/lib
endif

dag_serializer_runner_LDADD += \
    -luuid -lyaml-cpp -lboost_program_options

if LINUX
dag_serializer_runner_LDFLAGS += \
    -lssl -lcrypto
endif
dag_serializer_runner_LDADD += -lm
dag_serializer_runner_LDFLAGS += -pthread

CLEANFILES = *.o
CLEANDIRS = deps/ .lib/
