
AC_INIT([c9y], [0.2])
AM_INIT_AUTOMAKE([foreign])

AM_PROG_LIBTOOL            
AC_PROG_CXX

AC_PROG_MAKE_SET

PKG_CHECK_MODULES(SIGC, [sigc++-2.0 >= 2.0])
AC_SUBST(SIGC_CFLAGS)
AC_SUBST(SIGC_LIBS)        

AC_CHECK_HEADER([pthread.h])
AC_CHECK_LIB([pthread], [pthread_create], [PTHREAD_LIBS="-lpthread"], 
    AC_ERROR("pthread not found."))
AC_SUBST(PTHREAD_LIBS)

AC_CHECK_HEADER([UnitTest/UnitTest++.h])
AC_CHECK_LIB([UnitTest++], [main], [UNITTEST_LIBS="-lUnitTest++"], 
    AC_WARN("UnitTest++ not found."))
AC_SUBST(UNITTEST_LIBS)

AC_OUTPUT([Makefile])

