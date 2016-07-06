// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __Quaternion_h__
#define __Quaternion_h__

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
    class Quaternion;
}

namespace blink {

class Quaternion : public RefCountedWillBeGarbageCollectedFinalized <Quaternion>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<Quaternion> create();
    Quaternion();
    static PassRefPtrWillBeRawPtr<Quaternion> create(float arg0, float arg1, float arg2, float arg3);
    Quaternion(float arg0, float arg1, float arg2, float arg3);
    
    PassRefPtrWillBeRawPtr<Quaternion> normalize();
    
    PassRefPtrWillBeRawPtr<Quaternion> set(float arg0, float arg1, float arg2, float arg3);
    
    PassRefPtrWillBeRawPtr<Quaternion> setFromAxisAngle(Vector3* arg0, float arg1);
    
    PassRefPtrWillBeRawPtr<Vector3> multiplyVector3(Vector3* arg0);
    
    PassRefPtrWillBeRawPtr<Quaternion> clone();
    
    PassRefPtrWillBeRawPtr<Quaternion> multiplySelf(Quaternion* arg0);
    
    float length();
    
    PassRefPtrWillBeRawPtr<Quaternion> inverse();
    
    PassRefPtrWillBeRawPtr<Quaternion> slerpSelf(Quaternion* arg0, float arg1);
    
    PassRefPtrWillBeRawPtr<Quaternion> setFromEuler(Vector3* arg0, int arg1);
    
    PassRefPtrWillBeRawPtr<Quaternion> multiply(Quaternion* arg0, Quaternion* arg1);
    
    PassRefPtrWillBeRawPtr<Quaternion> calculateW();
    
    PassRefPtrWillBeRawPtr<Quaternion> copy(Quaternion* arg0);
    
    PassRefPtrWillBeRawPtr<Vector3> getEuler(int arg0);
    ~Quaternion();
    std::shared_ptr<three::Quaternion> getImpl() {
    	return m_impl;
    }

    void setImpl(std::shared_ptr<three::Quaternion> impl) {
    	m_impl = impl;
    }

    void setImpl(three::Quaternion& impl);

private:
    std::shared_ptr<three::Quaternion> m_impl;
};

} // namespace blink

#endif
