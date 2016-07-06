// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"

#include "core/three/three/three.hpp"

#include "modules/three/Vector3.h"
#include "modules/three/three.h"

namespace blink {
    PassRefPtrWillBeRawPtr<Vector3> Vector3::set(float arg0, float arg1, float arg2) {

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->set((float)(arg0), (float)(arg1), (float)(arg2));
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::divideScalar(float arg0) {

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->divideScalar((float)(arg0));
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    float Vector3::lengthSq() {

        float ret_impl = ((three::Vector3*)m_impl.get())->lengthSq();
        
        
        return ret_impl;
    }

    float Vector3::lengthManhattan() {

        float ret_impl = ((three::Vector3*)m_impl.get())->lengthManhattan();
        
        
        return ret_impl;
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::negate() {

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->negate();
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::normalize() {

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->normalize();
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::subSelf(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->subSelf( *_arg0);
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::sub(Vector3* arg0, Vector3* arg1) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());
        three::Vector3* _arg1 = (three::Vector3*)(arg1->getImpl().get());

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->sub( *_arg0,  *_arg1);
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::addSelf(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->addSelf( *_arg0);
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::cross(Vector3* arg0, Vector3* arg1) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());
        three::Vector3* _arg1 = (three::Vector3*)(arg1->getImpl().get());

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->cross( *_arg0,  *_arg1);
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    float Vector3::distanceToSquared(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        float ret_impl = ((three::Vector3*)m_impl.get())->distanceToSquared( *_arg0);
        
        
        return ret_impl;
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::add(Vector3* arg0, Vector3* arg1) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());
        three::Vector3* _arg1 = (three::Vector3*)(arg1->getImpl().get());

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->add( *_arg0,  *_arg1);
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    bool Vector3::isZero() {

        bool ret_impl = ((three::Vector3*)m_impl.get())->isZero();
        
        
        return ret_impl;
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::clone() {

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->clone();
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::multiplyScalar(float arg0) {

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->multiplyScalar((float)(arg0));
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    bool Vector3::equals(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        bool ret_impl = ((three::Vector3*)m_impl.get())->equals( *_arg0);
        
        
        return ret_impl;
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::copy(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->copy( *_arg0);
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::setLength(float arg0) {

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->setLength((float)(arg0));
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::lerpSelf(Vector3* arg0, float arg1) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->lerpSelf( *_arg0, (float)(arg1));
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::crossSelf(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Vector3 ret_impl = ((three::Vector3*)m_impl.get())->crossSelf( *_arg0);
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    float Vector3::distanceTo(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        float ret_impl = ((three::Vector3*)m_impl.get())->distanceTo( *_arg0);
        
        
        return ret_impl;
    }

    float Vector3::length() {

        float ret_impl = ((three::Vector3*)m_impl.get())->length();
        
        
        return ret_impl;
    }

    float Vector3::dot(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        float ret_impl = ((three::Vector3*)m_impl.get())->dot( *_arg0);
        
        
        return ret_impl;
    }


    PassRefPtrWillBeRawPtr<Vector3> Vector3::create() {
        RefPtrWillBeRawPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        std::shared_ptr<three::Vector3> impl = three::make_shared<three::Vector3>();
        ret->setImpl(impl);
        return ret.release();
    }

    Vector3::Vector3() {
        //m_impl = new three::Vector3();
    }

    Vector3::Vector3(float arg0, float arg1, float arg2) {
    }

    PassRefPtrWillBeRawPtr<Vector3> Vector3::create(float arg0, float arg1, float arg2) {
        RefPtrWillBeRawPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3( arg0,  arg1,  arg2));
        std::shared_ptr<three::Vector3> impl = three::make_shared<three::Vector3>( arg0,  arg1,  arg2);
        //three::Vector3* impl = new three::Vector3( arg0,  arg1,  arg2);
        ret->setImpl(impl);
        return ret.release();
    }

    Vector3::~Vector3() {
        m_impl.reset();
    }

    void Vector3::setImpl(three::Vector3& impl) {
        if (m_impl.get() != &impl)
           m_impl = std::make_shared<three::Vector3>(impl);
    }
} // namespace blink
