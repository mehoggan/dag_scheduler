lib_LTLIBRARIES = libstages_lib.la

libstages_lib_la_CPPFLAGS = -Werror -Wall -pedantic \
		-Wno-enum-constexpr-conversion \
    -I$(abs_top_srcdir)/include/ \
    -I$(abs_top_srcdir)/stages_lib/include

libstages_lib_la_SOURCES = print_stage.cxx

libstages_lib_la_LDFLAGS = -pthread

if LINUX
libstages_lib_la_LIBADD = $(INIT_LIBS) -lm
else
libstages_lib_la_LIBADD = $(INIT_LIBS)
endif
libstages_lib_la_LIBADD += $(INIT_LIBS) -lboost_filesystem -lboost_log
	-lboost_system -lboost_thread -lssl -lcrypto

libstages_lib_la_LIBADD += $(abs_top_builddir)/lib/libdag_scheduler.la
