// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"

#include "core/three/three/three.hpp"

#include "modules/three/Object3D.h"
#include "modules/three/three.h"

namespace blink {
    void Object3D::translateX(float arg0) {

        ((three::Object3D*)m_impl.get())->translateX((float)(arg0));
    }

    void Object3D::translateY(float arg0) {

        ((three::Object3D*)m_impl.get())->translateY((float)(arg0));
    }

    void Object3D::translateZ(float arg0) {

        ((three::Object3D*)m_impl.get())->translateZ((float)(arg0));
    }

    PassRefPtrWillBeRawPtr<Object3D> Object3D::getChildByName(String arg0_wrapper, bool arg1) {
        std::string arg0 = std::string(arg0_wrapper.utf8().data());
        std::shared_ptr<three::Object3D> ret_impl = ((three::Object3D*)m_impl.get())->getChildByName((std::string)(arg0), (bool)(arg1));
        
        RefPtr<Object3D> ret = adoptRefWillBeNoop(new Object3D());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    void Object3D::updateMatrix() {

        ((three::Object3D*)m_impl.get())->updateMatrix();
    }

    void Object3D::remove(Object3D* arg0) {
        const std::shared_ptr<three::Object3D>& _arg0 = arg0->getImpl();

        ((three::Object3D*)m_impl.get())->remove( _arg0);
    }

    void Object3D::add(Object3D* arg0) {
        const std::shared_ptr<three::Object3D>& _arg0 = arg0->getImpl();

        ((three::Object3D*)m_impl.get())->add( _arg0);
    }

    void Object3D::lookAt(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        ((three::Object3D*)m_impl.get())->lookAt( *_arg0);
    }

    void Object3D::updateMatrixWorld(bool arg0) {

        ((three::Object3D*)m_impl.get())->updateMatrixWorld((bool)(arg0));
    }

    void Object3D::applyMatrix(Matrix4* arg0) {
        three::Matrix4* _arg0 = (three::Matrix4*)(arg0->getImpl().get());

        ((three::Object3D*)m_impl.get())->applyMatrix( *_arg0);
    }

    void Object3D::translate(float arg0, Vector3* arg1) {
        three::Vector3* _arg1 = (three::Vector3*)(arg1->getImpl().get());

        ((three::Object3D*)m_impl.get())->translate((float)(arg0),  *_arg1);
    }

    int Object3D::type() {

        int ret_impl = ((three::Object3D*)m_impl.get())->type();
        
        
        return ret_impl;
    }

    PassRefPtrWillBeRawPtr<Object3D> Object3D::createInternal() {

        std::shared_ptr<three::Object3D> ret_impl = three::Object3D::create();
        
        RefPtr<Object3D> ret = adoptRefWillBeNoop(new Object3D());
        ret->setImpl(ret_impl);
        return ret.release();
    }


    PassRefPtrWillBeRawPtr<Object3D> Object3D::create() {
        RefPtrWillBeRawPtr<Object3D> ret = adoptRefWillBeNoop(new Object3D());
        std::shared_ptr<three::Object3D> impl = three::make_shared<three::Object3D>();
        ret->setImpl(impl);
        return ret.release();
    }

    Object3D::Object3D() {
        //m_impl = new three::Object3D();
    }


    Object3D::~Object3D() {
        m_impl.reset();
    }

    void Object3D::setImpl(three::Object3D& impl) {
        if (m_impl.get() != &impl)
           m_impl = std::make_shared<three::Object3D>(impl);
    }
} // namespace blink
