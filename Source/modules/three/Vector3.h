// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __Vector3_h__
#define __Vector3_h__

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
    class Vector3;
}

namespace blink {

class Vector3 : public RefCountedWillBeGarbageCollectedFinalized <Vector3>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<Vector3> create();
    Vector3();
    static PassRefPtrWillBeRawPtr<Vector3> create(float arg0, float arg1, float arg2);
    Vector3(float arg0, float arg1, float arg2);
    
    PassRefPtrWillBeRawPtr<Vector3> set(float arg0, float arg1, float arg2);
    
    PassRefPtrWillBeRawPtr<Vector3> divideScalar(float arg0);
    
    float lengthSq();
    
    float lengthManhattan();
    
    PassRefPtrWillBeRawPtr<Vector3> negate();
    
    PassRefPtrWillBeRawPtr<Vector3> normalize();
    
    PassRefPtrWillBeRawPtr<Vector3> subSelf(Vector3* arg0);
    
    PassRefPtrWillBeRawPtr<Vector3> sub(Vector3* arg0, Vector3* arg1);
    
    PassRefPtrWillBeRawPtr<Vector3> addSelf(Vector3* arg0);
    
    PassRefPtrWillBeRawPtr<Vector3> cross(Vector3* arg0, Vector3* arg1);
    
    float distanceToSquared(Vector3* arg0);
    
    PassRefPtrWillBeRawPtr<Vector3> add(Vector3* arg0, Vector3* arg1);
    
    bool isZero();
    
    PassRefPtrWillBeRawPtr<Vector3> clone();
    
    PassRefPtrWillBeRawPtr<Vector3> multiplyScalar(float arg0);
    
    bool equals(Vector3* arg0);
    
    PassRefPtrWillBeRawPtr<Vector3> copy(Vector3* arg0);
    
    PassRefPtrWillBeRawPtr<Vector3> setLength(float arg0);
    
    PassRefPtrWillBeRawPtr<Vector3> lerpSelf(Vector3* arg0, float arg1);
    
    PassRefPtrWillBeRawPtr<Vector3> crossSelf(Vector3* arg0);
    
    float distanceTo(Vector3* arg0);
    
    float length();
    
    float dot(Vector3* arg0);
    ~Vector3();
    std::shared_ptr<three::Vector3> getImpl() {
    	return m_impl;
    }

    void setImpl(std::shared_ptr<three::Vector3> impl) {
    	m_impl = impl;
    }

    void setImpl(three::Vector3& impl);

private:
    std::shared_ptr<three::Vector3> m_impl;
};

} // namespace blink

#endif
