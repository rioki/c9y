
pkginclude_HEADERS      = include/c9y/Condition.h include/c9y/Lock.h  \
                          include/c9y/Mutex.h include/c9y/Thread.h  \
                          include/c9y/utility.h

lib_LTLIBRARIES         = libc9y.la

libc9y_la_CXXFLAGS      = -I$(srcdir)/include/c9y $(SIGC_CFLAGS)
libc9y_la_LDFLAGS       = $(SIGC_LIBS) $(PTHREAD_LIBS)
libc9y_la_SOURCES       = src/posix/Condition.cpp src/posix/Mutex.cpp \
                          src/posix/Thread.cpp  src/posix/utility.cpp

check_PROGRAMS          = c9y_unit_test

c9y_unit_test_CXXFLAGS  = -I$(srcdir)/include $(SIGC_CFLAGS)
c9y_unit_test_LDADD     = libc9y.la
c9y_unit_test_LDFLAGS   = $(SIGC_LIBS) $(PTHREAD_LIBS) $(UNITTEST_LIBS)
c9y_unit_test_SOURCES   = test/c9y_unit_test.cpp test/ConditionTest.cpp \
                         test/MutexTest.cpp  test/ThreadTest.cpp
                         
TESTS                   = $(check_PROGRAMS)

