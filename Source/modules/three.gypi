{
  'variables': {
    # Experimental hooks for embedder to provide extra IDL and source files.
    #
    # Note: this is not a supported API. If you rely on this, you will be broken
    # from time to time as the code generator changes in backward incompatible
    # ways.
    'three_modules_idl_files': [
      'three/Three.idl',
      'three/Object3D.idl',
      'three/Vector3.idl',
      'three/Vector4.idl',
      'three/Matrix4.idl',
      'three/Quaternion.idl'
    ],
    # 'partial interface' or target (right side of) 'implements'
    # interfaces that inherit from Event
    'three_modules_event_idl_files': [
    ],
    'three_generated_modules_files': [
      # .cpp files from make_modules_generated actions.
    ],
    'three_modules_files': [
      'three/DOMWindowThree.cpp',
      'three/DOMWindowThree.h',
      'three/Three.cpp',
      'three/Three.h',
      'three/Object3D.cpp',
      'three/Object3D.h',
      'three/Vector3.cpp',
      'three/Vector3.h',
      'three/Vector4.cpp',
      'three/Vector4.h',
      'three/Matrix4.cpp',
      'three/Matrix4.h',
      'three/Quaternion.cpp',
      'three/Quaternion.h',
      'three/CWGraphicsContext.h',
      'three/CWGraphicsContext.cpp'
    ],
    # 'partial interface' or target (right side of) 'implements'
    'cocos2d_modules_testing_dependency_idl_files' : [
    ],
    'cocos2d_modules_testing_files': [
    ],
    'cocos2d_modules_unittest_files': [
    ],
  },
}
