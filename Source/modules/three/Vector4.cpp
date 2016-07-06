// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"

#include "core/three/three/three.hpp"

#include "modules/three/Vector4.h"
#include "modules/three/three.h"

namespace blink {
    PassRefPtrWillBeRawPtr<Vector4> Vector4::normalize() {

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->normalize();
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::clone() {

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->clone();
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::set(float arg0, float arg1, float arg2, float arg3) {

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->set((float)(arg0), (float)(arg1), (float)(arg2), (float)(arg3));
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::sub(Vector4* arg0, Vector4* arg1) {
        three::Vector4* _arg0 = (three::Vector4*)(arg0->getImpl().get());
        three::Vector4* _arg1 = (three::Vector4*)(arg1->getImpl().get());

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->sub( *_arg0,  *_arg1);
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::addSelf(Vector4* arg0) {
        three::Vector4* _arg0 = (three::Vector4*)(arg0->getImpl().get());

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->addSelf( *_arg0);
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::lerpSelf(Vector4* arg0, float arg1) {
        three::Vector4* _arg0 = (three::Vector4*)(arg0->getImpl().get());

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->lerpSelf( *_arg0, (float)(arg1));
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::multiplyScalar(float arg0) {

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->multiplyScalar((float)(arg0));
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    float Vector4::length() {

        float ret_impl = ((three::Vector4*)m_impl.get())->length();
        
        
        return ret_impl;
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::setAxisAngleFromQuaternion(Quaternion* arg0) {
        three::Quaternion* _arg0 = (three::Quaternion*)(arg0->getImpl().get());

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->setAxisAngleFromQuaternion( *_arg0);
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::divideScalar(float arg0) {

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->divideScalar((float)(arg0));
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::add(Vector4* arg0, Vector4* arg1) {
        three::Vector4* _arg0 = (three::Vector4*)(arg0->getImpl().get());
        three::Vector4* _arg1 = (three::Vector4*)(arg1->getImpl().get());

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->add( *_arg0,  *_arg1);
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    float Vector4::lengthSq() {

        float ret_impl = ((three::Vector4*)m_impl.get())->lengthSq();
        
        
        return ret_impl;
    }

    float Vector4::dot(Vector4* arg0) {
        three::Vector4* _arg0 = (three::Vector4*)(arg0->getImpl().get());

        float ret_impl = ((three::Vector4*)m_impl.get())->dot( *_arg0);
        
        
        return ret_impl;
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::negate() {

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->negate();
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    bool Vector4::equals(Vector4* arg0) {
        three::Vector4* _arg0 = (three::Vector4*)(arg0->getImpl().get());

        bool ret_impl = ((three::Vector4*)m_impl.get())->equals( *_arg0);
        
        
        return ret_impl;
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::copy(Vector4* arg0) {
        three::Vector4* _arg0 = (three::Vector4*)(arg0->getImpl().get());

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->copy( *_arg0);
        
        RefPtr<Vector4>  ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::copy(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->copy( *_arg0);
        
        RefPtr<Vector4>  ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::setLength(float arg0) {

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->setLength((float)(arg0));
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::subSelf(Vector4* arg0) {
        three::Vector4* _arg0 = (three::Vector4*)(arg0->getImpl().get());

        three::Vector4 ret_impl = ((three::Vector4*)m_impl.get())->subSelf( *_arg0);
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }


    PassRefPtrWillBeRawPtr<Vector4> Vector4::create() {
        RefPtrWillBeRawPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        std::shared_ptr<three::Vector4> impl = three::make_shared<three::Vector4>();
        ret->setImpl(impl);
        return ret.release();
    }

    Vector4::Vector4() {
        //m_impl = new three::Vector4();
    }

    Vector4::Vector4(float arg0, float arg1, float arg2, float arg3) {
    }

    PassRefPtrWillBeRawPtr<Vector4> Vector4::create(float arg0, float arg1, float arg2, float arg3) {
        RefPtrWillBeRawPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4( arg0,  arg1,  arg2,  arg3));
        std::shared_ptr<three::Vector4> impl = three::make_shared<three::Vector4>( arg0,  arg1,  arg2,  arg3);
        //three::Vector4* impl = new three::Vector4( arg0,  arg1,  arg2,  arg3);
        ret->setImpl(impl);
        return ret.release();
    }

    Vector4::~Vector4() {
        m_impl.reset();
    }

    void Vector4::setImpl(three::Vector4& impl) {
        if (m_impl.get() != &impl)
           m_impl = std::make_shared<three::Vector4>(impl);
    }
} // namespace blink
