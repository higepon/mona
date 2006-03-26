#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(fat_write, 0.0.1, higepon@users.sourceforge.jp)
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([fat.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_CHECK_FUNCS([memset mkdir])

# Checks for MONADIR
AM_CHECK_MONADIR
AC_SUBST(MONADIR)

# Checks for ENDIAN
AC_C_BIGENDIAN

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
