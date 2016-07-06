// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __Camera_h__
#define __Camera_h__

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"
#include "wtf/Uint8Array.h"
#include "wtf/text/WTFString.h"
#include "modules/three/three.hpp"

namespace RESIN {
    class Camera;
}

namespace blink {

class Camera : public RefCountedWillBeGarbageCollectedFinalized <Camera>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<Camera> create();
    Camera();
    
    static shared_ptr<three::Camera> createInternal();
    ~Camera();
    RESIN::Camera* getImpl() {
    	return m_impl;
    }

    void setImpl(RESIN::Camera* impl) {
    	m_impl = impl;
    }

    void setImpl(RESIN::Camera& impl) {
    	m_impl = &impl;
    }

private:
    RESIN::Camera* m_impl;
};

} // namespace blink

#endif
