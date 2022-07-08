# What is Mona?
MonaOS is a free operating system.
See detailed information on http://www.monaos.org.

![screenshot](http://www.monaos.org/img/0.3.4d.png)

# Building and Running Mona
## Requirements
- git
- Mingw (See building Mingw below)
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
    % cd mona
    % ./configure --mingw-prefix=/usr/bin/i586-mingw32msvc- 
    % make
    % cd ..
    % cd contrib
    % make
    % cd ../mona/tool/qemu
    % ./start.sh

## Building Mingw
If you use Ubuntu, you can get mingw as "apt-get install mingw32". If you use other OS or need the latest gcc, build as follows.
You need

- gcc-4.6.1.tar.bz2
- binutils-2.21.1.tar.gz
- w32api-3.17-2-mingw32-dev.tar.lzma
- mingwrt-3.18-mingw32-dev.tar.gz

### Build
    % export CC=clang    # For OSX only
    % export CXX=clang++ # For OSX only
    % export PATH=~/mona-mingw/bin/:$PATH
    % mkdir ~/mingw
    % cd ~/mingw
    % tar vxf ~/src/w32api-3.17-2-mingw32-dev.tar.lzma
    % tar vxf ~/src/mingwrt-3.18-mingw32-dev.tar.gz
    % mkdir -p ~/tmp/build-gcc
    % mkdir -p ~/tmp/build-binutils
    % cd ~/tmp
    % tar vxf ~/src/gcc-4.6.1.tar.bz2
    % tar vxf ~/src/binutils-2.21.1.tar.gz
    % cd ~/tmp/build-binutils
    % ../binutils-2.21.1/configure --prefix=~/mona-mingw --target=i586-mingw32msvc
    % make && make install
    % cd ~/tmp/build-gcc
    % ../gcc-4.6.1/configure --prefix=~/mona-mingw --target=i586-mingw32msvc --disable-nls \
      --disable-shared --disable-multilib --disable-decimal-float --disable-threads \
      --disable-libmudflap --disable-libssp --disable-libgomp --disable-libquadmath \
      --disable-target-libiberty --disable-target-zlib --enable-languages=c,c++ \ 
      --without-ppl --without-cloog  --with-headers=~/mingw/include --with-libs=~/mingw/lib \
      --with-as=~/mona-mingw/bin/i586-mingw32msvc-as --with-ld=~/mona-mingw/bin/i586-mingw32msvc-ld \
      --enable-sjlj-exceptions --enable-version-specific-runtime-libs

And then configure Mona:
`export PATH=~/mona-mingw/bin/:$PATH; ./configure --mingw-prefix=~/mona-mingw/bin/i586-mingw32msvc- && make`

Separate steps:
```
export PATH=~/mona-mingw/bin/:$PATH
./configure --mingw-prefix=~/mona-mingw/bin/i586-mingw32msvc-
make
```
