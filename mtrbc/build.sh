#!/bin/bash

NCPUS=$(grep -c ^processor /proc/cpuinfo)
if [ ! -z ${NCPUS+x} ]; then
  MAKE_ARGS="-j${NCPUS}"
fi

BUILDDIR=build
[ ! -d ${BUILDDIR} ] && mkdir ${BUILDDIR}
cd ${BUILDDIR}
cmake .. && make ${MAKE_ARGS}

