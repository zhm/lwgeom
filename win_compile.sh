#!/bin/sh
CC=/Users/zacmcc/Downloads/mingw-w64-bin_i686-darwin_20121031/mingw/bin/gcc
CXX=/Users/zacmcc/Downloads/mingw-w64-bin_i686-darwin_20121031/mingw/bin/g++
PATH=/Users/zacmcc/Downloads/mingw-w64-bin_i686-darwin_20121031/mingw/bin:$PATH
PATH=/Users/zacmcc/Downloads/mingw-w64-bin_i686-darwin_20121031/libexec/gcc/x86_64-w64-mingw32/4.8.0:$PATH
CFLAGS="-I/Users/zacmcc/Downloads/mingw-w64-bin_i686-darwin_20121031/mingw/include"
# MINGWFLAGS="-mwin32 -mconsole -march=pentium4 "
# CFLAGS="$MINGWFLAGS"
# CXXFLAGS="$MINGWFLAGS"
# ./configure CC=$CC CXX=$CXX --target=i586-mingw32msvc --host=i586
# echo make CC=$CC CXX=/opt/local/bin/i386-mingw32-g++ CFLAGS="$CFLAGS" CXXFLAGS="$CXXFLAGS"
# make CC=$CC CXX=/opt/local/bin/i386-mingw32-g++ CFLAGS="$CFLAGS" CXXFLAGS="$CXXFLAGS"

ruby gen_windows_compile.rb
