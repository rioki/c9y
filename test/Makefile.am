
check_PROGRAMS              = c9y_unit_test

c9y_unit_test_CXXFLAGS      = -I$(top_srcdir)/include $(SIGC_CFLAGS)
c9y_unit_test_LDADD         = $(top_builddir)/src/libc9y.la $(SIGC_LIBS) $(PTHREAD_LIBS) $(UNITTEST_LIBS)
c9y_unit_test_SOURCES       = c9y_unit_test.cpp ConditionTest.cpp MutexTest.cpp \
                              ThreadTest.cpp
                         
TESTS                       = $(check_PROGRAMS)
