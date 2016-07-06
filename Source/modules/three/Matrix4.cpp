// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"

#include "core/three/three/three.hpp"

#include "modules/three/Matrix4.h"
#include "modules/three/three.h"

namespace blink {
    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::clone() {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->clone();
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::set(float arg0, float arg1, float arg2, float arg3, float arg4, float arg5, float arg6, float arg7, float arg8, float arg9, float arg10, float arg11, float arg12, float arg13, float arg14, float arg15) {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->set((float)(arg0), (float)(arg1), (float)(arg2), (float)(arg3), (float)(arg4), (float)(arg5), (float)(arg6), (float)(arg7), (float)(arg8), (float)(arg9), (float)(arg10), (float)(arg11), (float)(arg12), (float)(arg13), (float)(arg14), (float)(arg15));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Matrix4::rotateAxis(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Vector3 ret_impl = ((three::Matrix4*)m_impl.get())->rotateAxis( *_arg0);
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::makeFrustum(float arg0, float arg1, float arg2, float arg3, float arg4, float arg5) {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->makeFrustum((float)(arg0), (float)(arg1), (float)(arg2), (float)(arg3), (float)(arg4), (float)(arg5));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::extractRotation(Matrix4* arg0) {
        three::Matrix4* _arg0 = (three::Matrix4*)(arg0->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->extractRotation( *_arg0);
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::lookAt(Vector3* arg0, Vector3* arg1, Vector3* arg2) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());
        three::Vector3* _arg1 = (three::Vector3*)(arg1->getImpl().get());
        three::Vector3* _arg2 = (three::Vector3*)(arg2->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->lookAt( *_arg0,  *_arg1,  *_arg2);
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Quaternion> Matrix4::getRotation() {

        three::Quaternion ret_impl = ((three::Matrix4*)m_impl.get())->getRotation();
        
        RefPtr<Quaternion> ret = adoptRefWillBeNoop(new Quaternion());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::makeScale(float arg0, float arg1, float arg2) {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->makeScale((float)(arg0), (float)(arg1), (float)(arg2));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::setPosition(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->setPosition( *_arg0);
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::compose(Vector3* arg0, Quaternion* arg1, Vector3* arg2) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());
        three::Quaternion* _arg1 = (three::Quaternion*)(arg1->getImpl().get());
        three::Vector3* _arg2 = (three::Vector3*)(arg2->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->compose( *_arg0,  *_arg1,  *_arg2);
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::makeTranslation(float arg0, float arg1, float arg2) {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->makeTranslation((float)(arg0), (float)(arg1), (float)(arg2));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::rotateByAxis(Vector3* arg0, float arg1) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->rotateByAxis( *_arg0, (float)(arg1));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::rotateX(float arg0) {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->rotateX((float)(arg0));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::rotateY(float arg0) {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->rotateY((float)(arg0));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::rotateZ(float arg0) {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->rotateZ((float)(arg0));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::identity() {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->identity();
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::setRotationFromQuaternion(Quaternion* arg0) {
        three::Quaternion* _arg0 = (three::Quaternion*)(arg0->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->setRotationFromQuaternion( *_arg0);
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::multiplyToArray(Matrix4* arg0, Matrix4* arg1, PassRefPtr<Uint8Array> arg2) {
        three::Matrix4* _arg0 = (three::Matrix4*)(arg0->getImpl().get());
        three::Matrix4* _arg1 = (three::Matrix4*)(arg1->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->multiplyToArray( *_arg0,  *_arg1, (float*)(arg2.get()->data()));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::getInverse(Matrix4* arg0) {
        three::Matrix4* _arg0 = (three::Matrix4*)(arg0->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->getInverse( *_arg0);
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::translate(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->translate( *_arg0);
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector4> Matrix4::getAxisAngle() {

        three::Vector4 ret_impl = ((three::Matrix4*)m_impl.get())->getAxisAngle();
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::makeRotationX(float arg0) {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->makeRotationX((float)(arg0));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::makeRotationZ(float arg0) {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->makeRotationZ((float)(arg0));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::makeOrthographic(float arg0, float arg1, float arg2, float arg3, float arg4, float arg5) {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->makeOrthographic((float)(arg0), (float)(arg1), (float)(arg2), (float)(arg3), (float)(arg4), (float)(arg5));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    float Matrix4::determinant() {

        float ret_impl = ((three::Matrix4*)m_impl.get())->determinant();
        
        
        return ret_impl;
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::makeRotationAxis(Vector3* arg0, float arg1) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->makeRotationAxis( *_arg0, (float)(arg1));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector4> Matrix4::crossVector(Vector4* arg0) {
        three::Vector4* _arg0 = (three::Vector4*)(arg0->getImpl().get());

        three::Vector4 ret_impl = ((three::Matrix4*)m_impl.get())->crossVector( *_arg0);
        
        RefPtr<Vector4> ret = adoptRefWillBeNoop(new Vector4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::transpose() {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->transpose();
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::multiplyScalar(float arg0) {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->multiplyScalar((float)(arg0));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Matrix4::getColumnX() {

        three::Vector3 ret_impl = ((three::Matrix4*)m_impl.get())->getColumnX();
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Matrix4::getColumnY() {

        three::Vector3 ret_impl = ((three::Matrix4*)m_impl.get())->getColumnY();
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Matrix4::getColumnZ() {

        three::Vector3 ret_impl = ((three::Matrix4*)m_impl.get())->getColumnZ();
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::extractPosition(Matrix4* arg0) {
        three::Matrix4* _arg0 = (three::Matrix4*)(arg0->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->extractPosition( *_arg0);
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::scale(Vector3* arg0) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->scale( *_arg0);
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::multiply(Matrix4* arg0, Matrix4* arg1) {
        three::Matrix4* _arg0 = (three::Matrix4*)(arg0->getImpl().get());
        three::Matrix4* _arg1 = (three::Matrix4*)(arg1->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->multiply( *_arg0,  *_arg1);
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Matrix4::getScale() {

        three::Vector3 ret_impl = ((three::Matrix4*)m_impl.get())->getScale();
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::copy(Matrix4* arg0) {
        three::Matrix4* _arg0 = (three::Matrix4*)(arg0->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->copy( *_arg0);
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Matrix4::getPosition() {

        three::Vector3 ret_impl = ((three::Matrix4*)m_impl.get())->getPosition();
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::setRotationFromEuler(Vector3* arg0, int arg1) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->setRotationFromEuler( *_arg0, (three::THREE::Order)(arg1));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::makePerspective(float arg0, float arg1, float arg2, float arg3) {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->makePerspective((float)(arg0), (float)(arg1), (float)(arg2), (float)(arg3));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::makeRotationY(float arg0) {

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->makeRotationY((float)(arg0));
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::multiplySelf(Matrix4* arg0) {
        three::Matrix4* _arg0 = (three::Matrix4*)(arg0->getImpl().get());

        three::Matrix4 ret_impl = ((three::Matrix4*)m_impl.get())->multiplySelf( *_arg0);
        
        RefPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    PassRefPtrWillBeRawPtr<Vector3> Matrix4::getEulerRotation(int arg0) {

        three::Vector3 ret_impl = ((three::Matrix4*)m_impl.get())->getEulerRotation((three::THREE::Order)(arg0));
        
        RefPtr<Vector3> ret = adoptRefWillBeNoop(new Vector3());
        ret->setImpl(ret_impl);
        return ret.release();
    }

    float Matrix4::getMaxScaleOnAxis() {

        float ret_impl = ((three::Matrix4*)m_impl.get())->getMaxScaleOnAxis();
        
        
        return ret_impl;
    }

    void Matrix4::decompose(Vector3* arg0, Quaternion* arg1, Vector3* arg2) {
        three::Vector3* _arg0 = (three::Vector3*)(arg0->getImpl().get());
        three::Quaternion* _arg1 = (three::Quaternion*)(arg1->getImpl().get());
        three::Vector3* _arg2 = (three::Vector3*)(arg2->getImpl().get());

        ((three::Matrix4*)m_impl.get())->decompose( *_arg0,  *_arg1,  *_arg2);
    }


    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::create() {
        RefPtrWillBeRawPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4());
        std::shared_ptr<three::Matrix4> impl = three::make_shared<three::Matrix4>();
        ret->setImpl(impl);
        return ret.release();
    }

    Matrix4::Matrix4() {
        //m_impl = new three::Matrix4();
    }

    Matrix4::Matrix4(float arg0, float arg1, float arg2, float arg3, float arg4, float arg5, float arg6, float arg7, float arg8, float arg9, float arg10, float arg11, float arg12, float arg13, float arg14, float arg15) {
    }

    PassRefPtrWillBeRawPtr<Matrix4> Matrix4::create(float arg0, float arg1, float arg2, float arg3, float arg4, float arg5, float arg6, float arg7, float arg8, float arg9, float arg10, float arg11, float arg12, float arg13, float arg14, float arg15) {
        RefPtrWillBeRawPtr<Matrix4> ret = adoptRefWillBeNoop(new Matrix4( arg0,  arg1,  arg2,  arg3,  arg4,  arg5,  arg6,  arg7,  arg8,  arg9,  arg10,  arg11,  arg12,  arg13,  arg14,  arg15));
        std::shared_ptr<three::Matrix4> impl = three::make_shared<three::Matrix4>( arg0,  arg1,  arg2,  arg3,  arg4,  arg5,  arg6,  arg7,  arg8,  arg9,  arg10,  arg11,  arg12,  arg13,  arg14,  arg15);
        //three::Matrix4* impl = new three::Matrix4( arg0,  arg1,  arg2,  arg3,  arg4,  arg5,  arg6,  arg7,  arg8,  arg9,  arg10,  arg11,  arg12,  arg13,  arg14,  arg15);
        ret->setImpl(impl);
        return ret.release();
    }

    Matrix4::~Matrix4() {
        m_impl.reset();
    }

    void Matrix4::setImpl(three::Matrix4& impl) {
        if (m_impl.get() != &impl)
           m_impl = std::make_shared<three::Matrix4>(impl);
    }
} // namespace blink
