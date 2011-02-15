
pkginclude_HEADERS          = include/c9y/Condition.h include/c9y/Lock.h  \
                              include/c9y/Mutex.h include/c9y/Thread.h  \
                              include/c9y/utility.h

lib_LTLIBRARIES             = libc9y.la

libc9y_la_CXXFLAGS          = -I$(srcdir)/include/c9y $(SIGC_CFLAGS)
libc9y_la_LDFLAGS           = $(SIGC_LIBS) $(PTHREAD_LIBS)
libc9y_la_SOURCES           = src/posix/Condition.cpp src/posix/Mutex.cpp \
                              src/posix/Thread.cpp  src/posix/utility.cpp

check_PROGRAMS              = c9y_unit_test

c9y_unit_test_CXXFLAGS      = -I$(srcdir)/include $(SIGC_CFLAGS)
c9y_unit_test_LDADD         = libc9y.la $(SIGC_LIBS) $(PTHREAD_LIBS) $(UNITTEST_LIBS)
c9y_unit_test_SOURCES       = test/c9y_unit_test.cpp test/ConditionTest.cpp \
                              test/MutexTest.cpp  test/ThreadTest.cpp
                         
TESTS                       = $(check_PROGRAMS)


noinst_HEADERS              = examples/consumer_producer/Consumer.h \
                              examples/consumer_producer/Producer.h \
                              examples/consumer_producer/Queue.h


noinst_PROGRAMS             = consumer_producer

consumer_producer_CXXFLAGS  = -I$(srcdir)/include $(SIGC_CFLAGS)
consumer_producer_LDADD     = libc9y.la
consumer_producer_LDFLAGS   = $(SIGC_LIBS) $(PTHREAD_LIBS) $(UNITTEST_LIBS)
consumer_producer_SOURCES   = examples/consumer_producer/Consumer.cpp \
                              examples/consumer_producer/Producer.cpp \
                              examples/consumer_producer/main.cpp \
                              examples/consumer_producer/Queue.cpp

