
check_PROGRAMS              = consumer_producer

noinst_HEADERS               = Consumer.h Producer.h Queue.h

consumer_producer_CXXFLAGS  = -I$(top_srcdir)/include $(SIGC_CFLAGS)
consumer_producer_LDADD     = $(top_builddir)/src/libc9y.la $(SIGC_LIBS)
consumer_producer_SOURCES   = main.cpp Consumer.cpp Producer.cpp Queue.cpp
