#!/bin/sh
RLWRAP=`which rlwrap`
[ "x$RLWRAP" = x ] && ./scheme
if [ x$RLWRAP = x ]; then
  ./scheme
else
  rlwrap ./scheme
fi
