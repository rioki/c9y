
SUBDIRS = posix win32

lib_LTLIBRARIES     = libc9y.la

libc9y_la_CXXFLAGS  = -I$(top_srcdir)/include/c9y $(SIGC_CFLAGS)
libc9y_la_LIBADD    = $(SIGC_LIBS)

if WIN32
libc9y_la_LIBADD   +=  win32/libc9y-win32.la
else
libc9y_la_LIBADD   +=  win32/libc9y-posix.la
endif

libc9y_la_SOURCES   = 
