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

## Building and Running
    % git clone git://github.com/higepon/mona.git
    % cd mona/mona
    % git submodule init
    % git submodule update
    % cd mona
    % make
    % cd ..
    % cd contrib
    % make
    % cd ../mona/tool/qemu
    % ./start.sh

