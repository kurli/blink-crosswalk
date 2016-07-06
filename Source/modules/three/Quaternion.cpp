// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"

#include "core/three/three/three.hpp"

#include "modules/three/Quaternion.h"
#include "modules/three/three.h"

namespace blink {
    PassRefPtrWillBeRawPtr<Quaternion> Quaternion::normalize() {

        three::Quaternion ret_impl = ((three::Quaternion*)m_impl.get())->normalize();
        
        RefPtr<Quaternion> ret = adoptRefWillBeNoop(new Quaternion());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Quaternion> Quaternion::set(float arg0, float arg1, float arg2, float arg3) {

        three::Quaternion ret_impl = ((three::Quaternion*)m_impl.get())->set((float)(arg0), (float)(arg1), (float)(arg2), (float)(arg3));
        
        RefPtr<Quaternion> ret = adoptRefWillBeNoop(new Quaternion());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Quaternion> Quaternion::setFromAxisAngle(Vector3* arg0, float arg1) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Quaternion ret_impl = ((three::Quaternion*)m_impl.get())->setFromAxisAngle( *_arg0, (float)(arg1));
        
        RefPtr<Quaternion> ret = adoptRefWillBeNoop(new Quaternion());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Quaternion::multiplyVector3(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Vector3 ret_impl = ((three::Quaternion*)m_impl.get())->multiplyVector3( *_arg0);
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Quaternion> Quaternion::clone() {

        three::Quaternion ret_impl = ((three::Quaternion*)m_impl.get())->clone();
        
        RefPtr<Quaternion> ret = adoptRefWillBeNoop(new Quaternion());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Quaternion> Quaternion::multiplySelf(Quaternion* arg0) {
        three::Quaternion* _arg0 = (three::Quaternion*)(arg0->getImpl().get());

        three::Quaternion ret_impl = ((three::Quaternion*)m_impl.get())->multiplySelf( *_arg0);
        
        RefPtr<Quaternion> ret = adoptRefWillBeNoop(new Quaternion());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    float Quaternion::length() {

        float ret_impl = ((three::Quaternion*)m_impl.get())->length();
        
        
        return ret_impl;
    }

    PassRefPtrWillBeRawPtr<Quaternion> Quaternion::inverse() {

        three::Quaternion ret_impl = ((three::Quaternion*)m_impl.get())->inverse();
        
        RefPtr<Quaternion> ret = adoptRefWillBeNoop(new Quaternion());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Quaternion> Quaternion::slerpSelf(Quaternion* arg0, float arg1) {
        three::Quaternion* _arg0 = (three::Quaternion*)(arg0->getImpl().get());

        three::Quaternion ret_impl = ((three::Quaternion*)m_impl.get())->slerpSelf( *_arg0, (float)(arg1));
        
        RefPtr<Quaternion> ret = adoptRefWillBeNoop(new Quaternion());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Quaternion> Quaternion::setFromEuler(Vector3* arg0, int arg1) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Quaternion ret_impl = ((three::Quaternion*)m_impl.get())->setFromEuler( *_arg0, (three::THREE::Order)(arg1));
        
        RefPtr<Quaternion> ret = adoptRefWillBeNoop(new Quaternion());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Quaternion> Quaternion::multiply(Quaternion* arg0, Quaternion* arg1) {
        three::Quaternion* _arg0 = (three::Quaternion*)(arg0->getImpl().get());
        three::Quaternion* _arg1 = (three::Quaternion*)(arg1->getImpl().get());

        three::Quaternion ret_impl = ((three::Quaternion*)m_impl.get())->multiply( *_arg0,  *_arg1);
        
        RefPtr<Quaternion> ret = adoptRefWillBeNoop(new Quaternion());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Quaternion> Quaternion::calculateW() {

        three::Quaternion ret_impl = ((three::Quaternion*)m_impl.get())->calculateW();
        
        RefPtr<Quaternion> ret = adoptRefWillBeNoop(new Quaternion());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Quaternion> Quaternion::copy(Quaternion* arg0) {
        three::Quaternion* _arg0 = (three::Quaternion*)(arg0->getImpl().get());

        three::Quaternion ret_impl = ((three::Quaternion*)m_impl.get())->copy( *_arg0);
        
        RefPtr<Quaternion> ret = adoptRefWillBeNoop(new Quaternion());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Quaternion::getEuler(int arg0) {

        three::Vector3 ret_impl = ((three::Quaternion*)m_impl.get())->getEuler((three::THREE::Order)(arg0));
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }


    PassRefPtrWillBeRawPtr<Quaternion> Quaternion::create() {
        RefPtrWillBeRawPtr<Quaternion> ret = adoptRefWillBeNoop(new Quaternion());
        std::shared_ptr<three::Quaternion> impl = three::make_shared<three::Quaternion>();
        ret->setImpl(impl);
        return ret.release();
    }

    Quaternion::Quaternion() {
        //m_impl = new three::Quaternion();
    }

    Quaternion::Quaternion(float arg0, float arg1, float arg2, float arg3) {
    }

    PassRefPtrWillBeRawPtr<Quaternion> Quaternion::create(float arg0, float arg1, float arg2, float arg3) {
        RefPtrWillBeRawPtr<Quaternion> ret = adoptRefWillBeNoop(new Quaternion( arg0,  arg1,  arg2,  arg3));
        std::shared_ptr<three::Quaternion> impl = three::make_shared<three::Quaternion>( arg0,  arg1,  arg2,  arg3);
        //three::Quaternion* impl = new three::Quaternion( arg0,  arg1,  arg2,  arg3);
        ret->setImpl(impl);
        return ret.release();
    }

    Quaternion::~Quaternion() {
        m_impl.reset();
    }

    void Quaternion::setImpl(three::Quaternion& impl) {
        if (m_impl.get() != &impl)
           m_impl = std::make_shared<three::Quaternion>(impl);
    }
} // namespace blink
