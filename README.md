# liblwgeom
This is a version of liblwgeom extracted from PostGIS 2.0.2 and converted to valid C++ so it compiles with MSVC 2010. It also includes a `binding.gyp` file so it will build with `node-gyp`. The `lwgeom` directory is a copy from [here](https://github.com/postgis/postgis/tree/326a2c96572c0a4c2837546f0a191cc1c1a52d2c/liblwgeom). I converted this so liblwgeom can be built using gyp and used from native nodejs modules without needing to install system dependencies or needing a separate MinGW build step. In particular, SpatiaLite can be built with support for some neat functionality provided by `liblwgeom`. Since MSVC [doesn't support](http://herbsutter.com/2012/05/03/reader-qa-what-about-vc-and-c99/) C99, liblwgeom requires some fairly monotonous changes so it will compile as the "C subset of C++" that's supported when compiled as C++ with MSVC.

# Tested on
- Linux 32bit
- Linux 64bit
- OS X 64bit
- Windows 64bit
- Windows 32bit should work (pending testing)

# Prerequisites
- nodejs
- GEOS (located at `../geos/geos`)
- Proj4 (located at `../proj/proj`)

For Windows, there are more dependencies. Follow the Windows MSVC setup instructions from the `node-gyp` pages [here](https://github.com/TooTallNate/node-gyp)

# Building

    $ node-gyp configure build

# Notes
This is intended to be used within the context of another gyp project that contains GEOS and Proj4.
