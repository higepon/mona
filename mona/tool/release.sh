#!/bin/bash
cd ~/Desktop
git clone git@github.com:higepon/mona.git mona-x.x.x
cd mona-x.x.x
git submodule init
git submodule update
rm -rf ./.git
rm -rf ./mona/core/mosh/.git
