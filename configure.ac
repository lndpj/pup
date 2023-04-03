# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([pup], [1.0], [vladimir@stupin.su])
AC_CONFIG_SRCDIR([main.c])
AM_INIT_AUTOMAKE([foreign -Wportability -Wall -Werror -Wsyntax])
AC_CONFIG_FILES([Makefile])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([ctype.h dirent.h errno.h fcntl.h limits.h stdarg.h stdint.h stdio.h stdlib.h string.h strings.h time.h unistd.h utime.h zlib.h sys/stat.h sys/types.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_MODE_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset mkdir strcasecmp strchr strrchr strspn strtoul utime])
AC_CHECK_LIB([z], [compress2, uncompress])

AC_OUTPUT
