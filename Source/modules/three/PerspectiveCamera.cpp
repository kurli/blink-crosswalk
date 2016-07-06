// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"

#include "core/resinlib/src/resin/resinlib.h"

#include "modules/three/PerspectiveCamera.h"
#include "modules/three/three.h"

namespace blink {
    void PerspectiveCamera::updateProjectionMatrix() {

        ((three::PerspectiveCamera*)m_impl)->updateProjectionMatrix();
    }

    void PerspectiveCamera::setLens(float arg0, float arg1) {

        ((three::PerspectiveCamera*)m_impl)->setLens((float)(arg0), (float)(arg1));
    }

    void PerspectiveCamera::setViewOffset(float arg0, float arg1, float arg2, float arg3, float arg4, float arg5) {

        ((three::PerspectiveCamera*)m_impl)->setViewOffset((float)(arg0), (float)(arg1), (float)(arg2), (float)(arg3), (float)(arg4), (float)(arg5));
    }

    shared_ptr<three::PerspectiveCamera> PerspectiveCamera::createInternal(float arg0, float arg1, float arg2, float arg3) {

        shared_ptr<three::PerspectiveCamera> ret_impl = three::PerspectiveCamera::create((float)(arg0), (float)(arg1), (float)(arg2), (float)(arg3));
        
        
        return ret_impl;
    }


    PassRefPtrWillBeRawPtr<PerspectiveCamera> PerspectiveCamera::create() {
        RefPtrWillBeRawPtr<PerspectiveCamera> ret = adoptRefWillBeNoop(new PerspectiveCamera());
        three::PerspectiveCamera* impl = new three::PerspectiveCamera();
        ret->setImpl(impl);
        return ret.release();
    }

    PerspectiveCamera::PerspectiveCamera() {
        //m_impl = new three::PerspectiveCamera();
    }


    PerspectiveCamera::~PerspectiveCamera() {
        if (m_impl != NULL) {
            delete m_impl;
        }
        m_impl = NULL;
    }
} // namespace blink
