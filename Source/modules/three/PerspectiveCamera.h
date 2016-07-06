// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __PerspectiveCamera_h__
#define __PerspectiveCamera_h__

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"
#include "wtf/Uint8Array.h"
#include "wtf/text/WTFString.h"
#include "modules/three/three.hpp"

namespace RESIN {
    class PerspectiveCamera;
}

namespace blink {

class PerspectiveCamera : public RefCountedWillBeGarbageCollectedFinalized <PerspectiveCamera>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<PerspectiveCamera> create();
    PerspectiveCamera();
    
    void updateProjectionMatrix();
    
    void setLens(float arg0, float arg1);
    
    void setViewOffset(float arg0, float arg1, float arg2, float arg3, float arg4, float arg5);
    
    static shared_ptr<three::PerspectiveCamera> createInternal(float arg0, float arg1, float arg2, float arg3);
    ~PerspectiveCamera();
    RESIN::PerspectiveCamera* getImpl() {
    	return m_impl;
    }

    void setImpl(RESIN::PerspectiveCamera* impl) {
    	m_impl = impl;
    }

    void setImpl(RESIN::PerspectiveCamera& impl) {
    	m_impl = &impl;
    }

private:
    RESIN::PerspectiveCamera* m_impl;
};

} // namespace blink

#endif
