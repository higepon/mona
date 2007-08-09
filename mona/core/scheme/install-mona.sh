#!/bin/bash
make -f Makefile.mona install && cd ../../tool/mkimg && make iso && ../qemu/start.sh
