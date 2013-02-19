{
  'variables': { 'target_arch%': 'ia32' },

  'target_defaults': {
    'default_configuration': 'Debug',
    'configurations': {
      'Debug': {
        'defines': [ 'DEBUG', '_DEBUG' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 1, # static debug
          },
        },
      },
      'Release': {
        'defines': [ 'NDEBUG' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 0, # static release
          },
        },
      }
    },
    'msvs_settings': {
      'VCCLCompilerTool': {
        'CompileAs': 'CompileAsCpp'
      },
      'VCLibrarianTool': {
      },
      'VCLinkerTool': {
        'GenerateDebugInformation': 'true',
      },
    },
    'defines': [
    ],
    'include_dirs': [
      '../proj/proj/src',
      '../geos/geos/capi',
      '../geos/geos/include',
      'config/<(OS)/<(target_arch)',
      'lwgeom'
    ],
    'conditions': [
      ['OS == "win"', {
        'msvs_settings': {
          'VCCLCompilerTool': {
            'CompileAs': 'CompileAsCpp',
          },
        },
        'defines': [
          'WIN32'
        ],
      }]
    ],
  },

  'targets': [
    {
      'target_name': 'lwgeom',
      'type': 'static_library',
      'msvs_settings': {
        'VCCLCompilerTool': {
          'AdditionalOptions': [ '/TP' ] # Compile as C++
        },
      },
      'direct_dependent_settings': {
        'include_dirs': [
          'config/<(OS)/<(target_arch)',
          'lwgeom'
        ],
        'defines': [
        ],
      },
      'sources': [
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
      ],
    },
  ]
}

