#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([vmm], [v1.0], [tyrion.lannister.274al@gmail.com])
AM_INIT_AUTOMAKE
LT_PREREQ([2.4.2])
LT_INIT

AC_CONFIG_SRCDIR([application-layer/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O
AM_PROG_AS

# Checks for libraries.
have_pthreads=no
AC_SEARCH_LIBS([pthread_create], [pthread], [have_pthreads=yes])

if test "x${have_pthreads}" = xyes; then
    AC_CHECK_HEADERS([pthread.h], [], [have_pthreads=no])
fi

if test "x${have_pthreads}" = xno; then
    AC_MSG_ERROR([
        -------------------------------------------------------
               Unable to find pthreads on this system.
        -------------------------------------------------------])
fi
    
# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MMAP
AC_CHECK_FUNCS([munmap])

AC_CONFIG_FILES([Makefile
                 algorithm-layer/Makefile
                 algorithm-layer/loader/Makefile
                 algorithm-layer/accelerator/Makefile
                 algorithm-layer/virtualMachine/Makefile
                 algorithm-layer/helpers/Makefile
                 algorithm-layer/include/Makefile
                 application-layer/Makefile
                 test/Makefile
                 test/application-layer-testcase/Makefile
                 test/application-layer-testcase/test1/Makefile])
                 
AC_OUTPUT

echo \
"-------------------------------------------------------
${PACKAGE_NAME} Version ${PACKAGE_VERSION}
 
Prefix: '${prefix}'.
Compiler: '${CC} ${CFLAGS} ${CPPFLAGS}'
Libraries: ${LIBS}

Now type 'make @<:@<target>@:>@'
    where the optional <target> is:
        all             - build all binaries
        install         - install everything
-------------------------------------------------------"
