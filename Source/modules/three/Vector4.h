// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __Vector4_h__
#define __Vector4_h__

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
    class Vector4;
}

namespace blink {

class Vector4 : public RefCountedWillBeGarbageCollectedFinalized <Vector4>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<Vector4> create();
    Vector4();
    static PassRefPtrWillBeRawPtr<Vector4> create(float arg0, float arg1, float arg2, float arg3);
    Vector4(float arg0, float arg1, float arg2, float arg3);
    
    PassRefPtrWillBeRawPtr<Vector4> normalize();
    
    PassRefPtrWillBeRawPtr<Vector4> clone();
    
    PassRefPtrWillBeRawPtr<Vector4> set(float arg0, float arg1, float arg2, float arg3);
    
    PassRefPtrWillBeRawPtr<Vector4> sub(Vector4* arg0, Vector4* arg1);
    
    PassRefPtrWillBeRawPtr<Vector4> addSelf(Vector4* arg0);
    
    PassRefPtrWillBeRawPtr<Vector4> lerpSelf(Vector4* arg0, float arg1);
    
    PassRefPtrWillBeRawPtr<Vector4> multiplyScalar(float arg0);
    
    float length();
    
    PassRefPtrWillBeRawPtr<Vector4> setAxisAngleFromQuaternion(Quaternion* arg0);
    
    PassRefPtrWillBeRawPtr<Vector4> divideScalar(float arg0);
    
    PassRefPtrWillBeRawPtr<Vector4> add(Vector4* arg0, Vector4* arg1);
    
    float lengthSq();
    
    float dot(Vector4* arg0);
    
    PassRefPtrWillBeRawPtr<Vector4> negate();
    
    bool equals(Vector4* arg0);
    PassRefPtrWillBeRawPtr<Vector4> copy(Vector4* arg0);
    PassRefPtrWillBeRawPtr<Vector4> copy(Vector3* arg0);
    
    PassRefPtrWillBeRawPtr<Vector4> setLength(float arg0);
    
    PassRefPtrWillBeRawPtr<Vector4> subSelf(Vector4* arg0);
    ~Vector4();
    std::shared_ptr<three::Vector4> getImpl() {
    	return m_impl;
    }

    void setImpl(std::shared_ptr<three::Vector4> impl) {
    	m_impl = impl;
    }

    void setImpl(three::Vector4& impl);

private:
    std::shared_ptr<three::Vector4> m_impl;
};

} // namespace blink

#endif
