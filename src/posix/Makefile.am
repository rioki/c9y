
if ! WIN32
noinst_LTLIBRARIES          = libc9y-posix.la

libc9y_posix_la_CXXFLAGS    = -I$(top_srcdir)/include/c9y $(SIGC_CFLAGS)
libc9y_posix_la_LIBADD      = $(SIGC_LIBS) $(PTHREAD_LIBS)
libc9y_posix_la_SOURCES     = Condition.cpp Mutex.cpp Thread.cpp utility.cpp
endif
