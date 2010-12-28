# What is Mona?
MonaOS is a free operating system.
See detailed information on http://www.monaos.org.

# Building and Running Mona
## Requirements
- git
- Mingw
- nasm
- mkisofs
- QEMU
- mosh

## Building
    % cd mona
    % ./configure --mingw-prefix=/usr/bin/i586-mingw32msvc- 
    % make
    % cd ..
    % cd contrib
    % make
    % cd ../mona/tool/qemu
    % ./start.sh

## Building the cutting-edge Mona
    % git clone git://github.com/higepon/mona.git
    % cd mona
    % git submodule init
    % git submodule update
    % git checkout -b mona-port-new origin/mona-port-new
    % cd mona
    % ./configure --mingw-prefix=/usr/bin/i586-mingw32msvc- 
    % make
    % cd ..
    % cd contrib
    % make
    % cd ../mona/tool/qemu
    % ./start.sh

