bin_PROGRAMS = https_dag_scheduler_runner dag_serializer_runner

#####
# Start of https_dag_scheduler_runner
#####
https_dag_scheduler_runner_SOURCES = ServiceMain.cpp

https_dag_scheduler_runner_CPPFLAGS = -Werror -Wall -pedantic \
		-Wno-enum-constexpr-conversion \
    -I$(abs_top_srcdir)/include

https_dag_scheduler_runner_LDADD = \
    $(abs_top_builddir)/lib/libdag_scheduler.la
if LINUX
https_dag_scheduler_runner_LDADD += \
 		-lrt
endif

https_dag_scheduler_runner_LDADD += \
    -luuid -lyaml-cpp -lboost_program_options

if LINUX
https_dag_scheduler_runner_LDFLAGS = \
		-lssl -lcrypto -pthread
else
https_dag_scheduler_runner_LDFLAGS = \
		-Wl,-ld_classic \
		-pthread
endif
https_dag_scheduler_runner_LDADD += -lm

#####
# Start of dag_serializer_runner
#####
dag_serializer_runner_SOURCES = DagSerializerMain.cpp

dag_serializer_runner_CPPFLAGS = -Werror -Wall -pedantic \
		-Wno-enum-constexpr-conversion \
    -I$(abs_top_srcdir)/include

dag_serializer_runner_LDADD = \
    $(abs_top_builddir)/lib/libdag_scheduler.la
if LINUX
dag_serializer_runner_LDADD += -lrt
endif

dag_serializer_runner_LDADD += \
    -luuid -lyaml-cpp -lboost_program_options

if LINUX
dag_serializer_runner_LDFLAGS = -lssl -lcrypto -pthread
else
dag_serializer_runner_LDFLAGS = \
		-Wl,-ld_classic \
    -pthread
endif
dag_serializer_runner_LDADD += -lm

CLEANFILES = *.o
CLEANDIRS = .lib/
