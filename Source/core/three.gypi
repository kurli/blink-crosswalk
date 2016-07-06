# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {

        'resinlib_include_dirs': [
          'three/',
          'three/rapidjson/include/'
        ],

        'resinlib_sources': [
          'three/three/three.cpp'
        ],
  },
        'targets': [
        {
          'target_name': 'three',
          'type': 'static_library',
          'dependencies': [
          ],
          'include_dirs': [
            '<@(android_ndk_root)/sources/cxx-stl/system/include',
            '<@(resinlib_include_dirs)',
          ],
          'defines': [
            '__ANDROID__',
            '__STDC_CONSTANT_MACROS',
            '__GXX_EXPERIMENTAL_CXX0X__',
            'THREE_GLES'
          ],
          'sources': [
            '<@(resinlib_sources)',
          ],
          'cflags_cc!': [
            '-fno-rtti'
          ],
          'cflags': [
            '-std=c++11',
            '-Wno-multichar',
            '-Werror',
            '-fexceptions',
            '-frtti',
            '-Wno-format-security',
          ],
        }],

}
