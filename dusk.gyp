# Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
# for details. All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.

{
  'includes': [
  ],
  'variables': {
    'dart_runtime_dir': 'third_party/dart/sdk/runtime',

    'dart_sdk_gen_source_dir': '<(SHARED_INTERMEDIATE_DIR)',

    'dart_sdk_resources_cc_file': '<(dart_sdk_gen_source_dir)/resources_gen.cc',
    'dart_sdk_snapshot_cc_file': '<(dart_sdk_gen_source_dir)/snapshot_gen.cc',
  },
  'targets': [
    {
      'target_name': 'dusk',
      'type': 'executable',
      'dependencies': [
        '<(dart_runtime_dir)/dart-runtime.gyp:libdart',
        '<(dart_runtime_dir)/dart-runtime.gyp:libdart_builtin',
        '<(dart_runtime_dir)/dart-runtime.gyp:libdart_io',
        '<(dart_runtime_dir)/dart-runtime.gyp:build_observatory',
        '<(dart_runtime_dir)/dart-runtime.gyp:generate_snapshot_file',
        '<(dart_runtime_dir)/dart-runtime.gyp:generate_resources_cc_file',
      ],
      'include_dirs': [
        '.',
        'third_party',
        'third_party/love-as-lib/src',
        'third_party/love-as-lib/src/modules',
        'third_party/love-as-lib/src/modules',
        'third_party/sdl2/include',
        'third_party/freetype-2.5.0.1/include',
        '<(dart_runtime_dir)',
      ],
      'sources': [
        'dusk/main.cc',
        'dusk/dusk.h',
        'dusk/dusk.cc',
        'dusk/utils/clock.h',
        'dusk/utils/clock.cc',
        'dusk/vm/host.h',
        'dusk/vm/host.cc',
        'dusk/vm/libregistry.h',
        'dusk/vm/libregistry.cc',
        'dusk/vm/native.h',
        'dusk/vm/nativeutils.h',
        'dusk/vm/nativeutils.cc',
        'dusk/filesystem/filesystem_native.h',
        'dusk/filesystem/filesystem_native.cc',
        'dusk/filesystem/file_native.h',
        'dusk/filesystem/file_native.cc',
        'dusk/window/window_native.h',
        'dusk/window/window_native.cc',
        'dusk/window/window_source.h',
        'dusk/font/font_native.h',
        'dusk/font/font_native.cc',
        'dusk/graphics/graphics_native.h',
        'dusk/graphics/graphics_native.cc',
        'dusk/graphics/graphics_source.h',
        '<(dart_runtime_dir)/bin/builtin_common.cc',
        '<(dart_runtime_dir)/bin/builtin_natives.cc',
        '<(dart_runtime_dir)/bin/builtin_nolib.cc',
        '<(dart_runtime_dir)/bin/builtin.h',
        '<(dart_runtime_dir)/bin/io_natives.h',
        '<(dart_runtime_dir)/bin/vmservice_impl.cc',
        '<(dart_runtime_dir)/bin/vmservice_impl.h',
        '<(dart_sdk_snapshot_cc_file)',
        '<(dart_sdk_resources_cc_file)',
      ],
      'conditions': [
        ['OS=="win"', {
          'link_settings': {
            'libraries': [
              '-lws2_32.lib',
              '-lRpcrt4.lib',
              '-lwinmm.lib',
              '-lopengl32.lib',
              '-lUser32.lib',
              '-llove.lib',
            ],
          },
          'msvs_settings': {
            'VCLinkerTool': {
              'AdditionalLibraryDirectories': [
                'third_party/love-as-lib/lib',
                'third_party/sdl2/lib/x86',
              ],
              'SubSystem': '1'
            },
          }
        }],
      ],
    },
  ],
}
