// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"

#include "core/resinlib/src/resin/resinlib.h"

#include "modules/three/Camera.h"
#include "modules/three/three.h"

namespace blink {
    shared_ptr<three::Camera> Camera::createInternal() {

        shared_ptr<three::Camera> ret_impl = three::Camera::create();
        
        
        return ret_impl;
    }


    PassRefPtrWillBeRawPtr<Camera> Camera::create() {
        RefPtrWillBeRawPtr<Camera> ret = adoptRefWillBeNoop(new Camera());
        three::Camera* impl = new three::Camera();
        ret->setImpl(impl);
        return ret.release();
    }

    Camera::Camera() {
        //m_impl = new three::Camera();
    }


    Camera::~Camera() {
        if (m_impl != NULL) {
            delete m_impl;
        }
        m_impl = NULL;
    }
} // namespace blink
