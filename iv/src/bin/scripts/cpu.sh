#!/bin/sh

#
# If CPU is defined in the environment, output its value.  Otherwise,
# preprocess $1/IVarch.c to define the cpu type and output its value.
#

if [ x$CPU != x ]
then
    echo $CPU
else
    cc -E $1/IVarch.c | sed -e '/# arch/!d' -e 's/^[ ]*# architecture:  //'
fi
