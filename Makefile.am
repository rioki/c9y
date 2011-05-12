
pkginclude_HEADERS          = c9y/Condition.h  c9y/config.h  c9y/Lock.h  \
                              c9y/Mutex.h      c9y/Thread.h  c9y/Timer.h \
                              c9y/utility.h


lib_LTLIBRARIES             = libc9y.la

libc9y_la_CXXFLAGS          = -I$(srcdir)/include/c9y $(SIGC_CFLAGS)
libc9y_la_LDFLAGS           = $(SIGC_LIBS) $(PTHREAD_LIBS)
libc9y_la_SOURCES           = c9y/Condition.cpp  c9y/Mutex.cpp \
                              c9y/Thread.cpp     c9y/Timer.cpp \
                              c9y/utility.cpp

check_PROGRAMS              = c9y_test

c9y_test_CXXFLAGS           = -I$(srcdir)/c9y $(SIGC_CFLAGS)
c9y_test_LDADD              = libc9y.la $(SIGC_LIBS) $(PTHREAD_LIBS) $(UNITTEST_LIBS)
c9y_test_SOURCES            = c9y_test/ConditionTest.cpp  c9y_test/main.cpp       \
                              c9y_test/MutexTest.cpp      c9y_test/ThreadTest.cpp \
                              c9y_test/TimerTest.cpp

TESTS                       = $(check_PROGRAMS)

EXTRA_DIST                  = README.txt COPYING.txt

