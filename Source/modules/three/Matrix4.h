// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __Matrix4_h__
#define __Matrix4_h__

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"
#include "wtf/Uint8Array.h"
#include "wtf/text/WTFString.h"
#include "modules/three/three.hpp"
#include <memory>

namespace three {
    class Matrix4;
}

namespace blink {

class Matrix4 : public RefCountedWillBeGarbageCollectedFinalized <Matrix4>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<Matrix4> create();
    Matrix4();
    static PassRefPtrWillBeRawPtr<Matrix4> create(float arg0, float arg1, float arg2, float arg3, float arg4, float arg5, float arg6, float arg7, float arg8, float arg9, float arg10, float arg11, float arg12, float arg13, float arg14, float arg15);
    Matrix4(float arg0, float arg1, float arg2, float arg3, float arg4, float arg5, float arg6, float arg7, float arg8, float arg9, float arg10, float arg11, float arg12, float arg13, float arg14, float arg15);
    
    PassRefPtrWillBeRawPtr<Matrix4> clone();
    
    PassRefPtrWillBeRawPtr<Matrix4> set(float arg0, float arg1, float arg2, float arg3, float arg4, float arg5, float arg6, float arg7, float arg8, float arg9, float arg10, float arg11, float arg12, float arg13, float arg14, float arg15);
    
    PassRefPtrWillBeRawPtr<Vector3> rotateAxis(Vector3* arg0);
    
    PassRefPtrWillBeRawPtr<Matrix4> makeFrustum(float arg0, float arg1, float arg2, float arg3, float arg4, float arg5);
    
    PassRefPtrWillBeRawPtr<Matrix4> extractRotation(Matrix4* arg0);
    
    PassRefPtrWillBeRawPtr<Matrix4> lookAt(Vector3* arg0, Vector3* arg1, Vector3* arg2);
    
    PassRefPtrWillBeRawPtr<Quaternion> getRotation();
    
    PassRefPtrWillBeRawPtr<Matrix4> makeScale(float arg0, float arg1, float arg2);
    
    PassRefPtrWillBeRawPtr<Matrix4> setPosition(Vector3* arg0);
    
    PassRefPtrWillBeRawPtr<Matrix4> compose(Vector3* arg0, Quaternion* arg1, Vector3* arg2);
    
    PassRefPtrWillBeRawPtr<Matrix4> makeTranslation(float arg0, float arg1, float arg2);
    
    PassRefPtrWillBeRawPtr<Matrix4> rotateByAxis(Vector3* arg0, float arg1);
    
    PassRefPtrWillBeRawPtr<Matrix4> rotateX(float arg0);
    
    PassRefPtrWillBeRawPtr<Matrix4> rotateY(float arg0);
    
    PassRefPtrWillBeRawPtr<Matrix4> rotateZ(float arg0);
    
    PassRefPtrWillBeRawPtr<Matrix4> identity();
    
    PassRefPtrWillBeRawPtr<Matrix4> setRotationFromQuaternion(Quaternion* arg0);
    
    PassRefPtrWillBeRawPtr<Matrix4> multiplyToArray(Matrix4* arg0, Matrix4* arg1, PassRefPtr<Uint8Array> arg2);
    
    PassRefPtrWillBeRawPtr<Matrix4> getInverse(Matrix4* arg0);
    
    PassRefPtrWillBeRawPtr<Matrix4> translate(Vector3* arg0);
    
    PassRefPtrWillBeRawPtr<Vector4> getAxisAngle();
    
    PassRefPtrWillBeRawPtr<Matrix4> makeRotationX(float arg0);
    
    PassRefPtrWillBeRawPtr<Matrix4> makeRotationZ(float arg0);
    
    PassRefPtrWillBeRawPtr<Matrix4> makeOrthographic(float arg0, float arg1, float arg2, float arg3, float arg4, float arg5);
    
    float determinant();
    
    PassRefPtrWillBeRawPtr<Matrix4> makeRotationAxis(Vector3* arg0, float arg1);
    
    PassRefPtrWillBeRawPtr<Vector4> crossVector(Vector4* arg0);
    
    PassRefPtrWillBeRawPtr<Matrix4> transpose();
    
    PassRefPtrWillBeRawPtr<Matrix4> multiplyScalar(float arg0);
    
    PassRefPtrWillBeRawPtr<Vector3> getColumnX();
    
    PassRefPtrWillBeRawPtr<Vector3> getColumnY();
    
    PassRefPtrWillBeRawPtr<Vector3> getColumnZ();
    
    PassRefPtrWillBeRawPtr<Matrix4> extractPosition(Matrix4* arg0);
    
    PassRefPtrWillBeRawPtr<Matrix4> scale(Vector3* arg0);
    
    PassRefPtrWillBeRawPtr<Matrix4> multiply(Matrix4* arg0, Matrix4* arg1);
    
    PassRefPtrWillBeRawPtr<Vector3> getScale();
    
    PassRefPtrWillBeRawPtr<Matrix4> copy(Matrix4* arg0);
    
    PassRefPtrWillBeRawPtr<Vector3> getPosition();
    
    PassRefPtrWillBeRawPtr<Matrix4> setRotationFromEuler(Vector3* arg0, int arg1);
    
    PassRefPtrWillBeRawPtr<Matrix4> makePerspective(float arg0, float arg1, float arg2, float arg3);
    
    PassRefPtrWillBeRawPtr<Matrix4> makeRotationY(float arg0);
    
    PassRefPtrWillBeRawPtr<Matrix4> multiplySelf(Matrix4* arg0);
    
    PassRefPtrWillBeRawPtr<Vector3> getEulerRotation(int arg0);
    
    float getMaxScaleOnAxis();
    
    void decompose(Vector3* arg0, Quaternion* arg1, Vector3* arg2);
    ~Matrix4();
    std::shared_ptr<three::Matrix4> getImpl() {
    	return m_impl;
    }

    void setImpl(std::shared_ptr<three::Matrix4> impl) {
    	m_impl = impl;
    }

    void setImpl(three::Matrix4& impl);

private:
    std::shared_ptr<three::Matrix4> m_impl;
};

} // namespace blink

#endif
