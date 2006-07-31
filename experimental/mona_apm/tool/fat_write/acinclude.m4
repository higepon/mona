AC_DEFUN([AM_CHECK_MONADIR],
 [dnl # 
  dnl # Check env MONADIR exists?
  dnl #
  AC_MSG_CHECKING([MONADIR...])
  if test "$MONADIR" != "" ; then
    AC_MSG_RESULT(yes $MONADIR)
  else
    MONADIR="/usr/local/mona"
    export MONADIR
    AC_MSG_WARN(MONADIR not found. use default $MONADIR)
  fi])

