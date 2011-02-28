
if WIN32
noinst_LTLIBRARIES          = libc9y-win32.la

libc9y_win32_la_CXXFLAGS    = -I$(top_srcdir)/include/c9y $(SIGC_CFLAGS)
libc9y_win32_la_LIBADD      = $(SIGC_LIBS)
libc9y_win32_la_SOURCES     = Condition.cpp Mutex.cpp Thread.cpp utility.cpp
endif
