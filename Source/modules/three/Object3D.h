// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __Object3D_h__
#define __Object3D_h__

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
    class Object3D;
}

namespace blink {

class Object3D : public RefCountedWillBeGarbageCollectedFinalized <Object3D>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<Object3D> create();
    Object3D();
    
    void translateX(float arg0);
    
    void translateY(float arg0);
    
    void translateZ(float arg0);
    
    PassRefPtrWillBeRawPtr<Object3D> getChildByName(String arg0, bool arg1);
    
    void updateMatrix();
    
    void remove(Object3D* arg0);
    
    void add(Object3D* arg0);
    
    void lookAt(Vector3* arg0);
    
    void updateMatrixWorld(bool arg0);
    
    void applyMatrix(Matrix4* arg0);
    
    void translate(float arg0, Vector3* arg1);
    
    int type();
    
    static PassRefPtrWillBeRawPtr<Object3D> createInternal();
    ~Object3D();
    std::shared_ptr<three::Object3D> getImpl() {
    	return m_impl;
    }

    void setImpl(std::shared_ptr<three::Object3D> impl) {
    	m_impl = impl;
    }

    void setImpl(three::Object3D& impl);

private:
    std::shared_ptr<three::Object3D> m_impl;
};

} // namespace blink

#endif
