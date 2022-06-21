
CONFIG_SITE=$PWD/depends/x86_64-apple-darwin/share/config.site ./configure --disable-option-checking --prefix=$PWD/depends/x86_64-apple-darwin --with-gui --enable-reduce-exports --disable-online-rust --disable-shared --with-pic --enable-module-recovery --disable-jni

CONFIG_SITE=$PWD/depends/x86_64-pc-linux-gnu/share/config.site ./configure --disable-option-checking --prefix=$PWD/depends/x86_64-pc-linux-gnu --disable-dependency-tracking --enable-zmq --with-gui --enable-glibc-back-compat --enable-reduce-exports --disable-online-rust --disable-shared --with-pic --enable-module-recovery --disable-jni

CONFIG_SITE=$PWD/depends/x86_64-w64-mingw32/share/config.site ./configure --disable-option-checking --prefix=$PWD/depends/x86_64-w64-mingw32 --disable-dependency-tracking --with-gui --enable-reduce-exports --disable-online-rust --disable-shared --with-pic --enable-module-recovery --disable-jni

sudo apt update sudo apt upgrade sudo apt install build-essential libtool autotools-dev automake pkg-config bsdmainutils curl git libbz2-dev

win:

sudo apt install g++-mingw-w64-x86-64

linux:

sudo apt-get install make automake cmake curl g++-multilib libtool binutils-gold bsdmainutils pkg-config python3 patch bison

mac:

sudo apt-get install curl bsdmainutils cmake libz-dev python3-setuptools libtinfo5 xorriso
