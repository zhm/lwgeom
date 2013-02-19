#!/usr/bin/ruby

files = [
  'lwgeom/box2d.c',
  'lwgeom/g_box.c',
  'lwgeom/g_serialized.c',
  'lwgeom/g_util.c',
  'lwgeom/libtgeom.c',
  'lwgeom/lwalgorithm.c',
  'lwgeom/lwcircstring.c',
  'lwgeom/lwcollection.c',
  'lwgeom/lwcompound.c',
  'lwgeom/lwcurvepoly.c',
  'lwgeom/lwgeodetic.c',
  'lwgeom/lwgeom.c',
  'lwgeom/lwgeom_api.c',
  'lwgeom/lwgeom_debug.c',
  'lwgeom/lwgeom_geos.c',
  'lwgeom/lwgeom_geos_clean.c',
  'lwgeom/lwgeom_geos_node.c',
  'lwgeom/lwgeom_geos_split.c',
  'lwgeom/lwgeom_transform.c',
  'lwgeom/lwhomogenize.c',
  'lwgeom/lwin_wkb.c',
  'lwgeom/lwin_wkt.c',
  'lwgeom/lwin_wkt_lex.c',
  'lwgeom/lwin_wkt_parse.c',
  'lwgeom/lwline.c',
  'lwgeom/lwlinearreferencing.c',
  'lwgeom/lwmcurve.c',
  'lwgeom/lwmline.c',
  'lwgeom/lwmpoint.c',
  'lwgeom/lwmpoly.c',
  'lwgeom/lwmsurface.c',
  'lwgeom/lwout_geojson.c',
  'lwgeom/lwout_gml.c',
  'lwgeom/lwout_kml.c',
  'lwgeom/lwout_svg.c',
  'lwgeom/lwout_wkb.c',
  'lwgeom/lwout_wkt.c',
  'lwgeom/lwout_x3d.c',
  'lwgeom/lwpoint.c',
  'lwgeom/lwpoly.c',
  'lwgeom/lwprint.c',
  'lwgeom/lwpsurface.c',
  'lwgeom/lwsegmentize.c',
  'lwgeom/lwspheroid.c',
  'lwgeom/lwtin.c',
  'lwgeom/lwtree.c',
  'lwgeom/lwtriangle.c',
  'lwgeom/lwutil.c',
  'lwgeom/measures.c',
  'lwgeom/measures3d.c',
  'lwgeom/ptarray.c',
  'lwgeom/snprintf.c',
  'lwgeom/stringbuffer.c',
  'lwgeom/vsprintf.c'
]

INCLUDE_PATHS = [
  'config/win/x64',
  '/Users/zacmcc/Downloads/mingw-w64-bin_i686-darwin_20121031/mingw/include',
  '../geos/geos/include',
  '../geos/geos/capi',
  '../proj/proj/src'
].map {|p| "-I#{p}"}

files.each do |file|
  command = "gcc -g -O2 -fno-common -DPIC -Wall -Wmissing-prototypes #{INCLUDE_PATHS.join(' ')} -c #{file} -fno-common -DPIC -o .libs/#{file.gsub('.c', '')}.o"

  puts command
  system command
end
