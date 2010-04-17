# What is Mona?
MonaOS is a free operating system.
See detailed information on http://www.monaos.org.

# Building and Running Mona
## Requirements
- git
- Mingw
- QEMU

Get a release of Mona from [github](http://github.com/higepon/mona).

    % git clone git://github.com/higepon/mona.git
    % cd mona/mona
    % ./configure --mingw-prefix=/usr/bin/i586-mingw32msvc-
    % make
    % cd ..
    % cd contrib
    % make
    % cd ../mona/tool/qemu
    % ./start.sh

