// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DoubleStyleInterpolation_h
#define DoubleStyleInterpolation_h

#include "core/animation/StyleInterpolation.h"
#include "core/css/CSSPrimitiveValue.h"

namespace blink {

class DoubleStyleInterpolation : public StyleInterpolation {
public:
    static PassRefPtrWillBeRawPtr<DoubleStyleInterpolation> create(const CSSValue& start, const CSSValue& end, CSSPropertyID id, CSSPrimitiveValue::UnitType type, InterpolationRange clamp)
    {
        return adoptRefWillBeNoop(new DoubleStyleInterpolation(doubleToInterpolableValue(start), doubleToInterpolableValue(end), id, type == CSSPrimitiveValue::CSS_NUMBER, clamp, false));
    }

    static PassRefPtrWillBeRawPtr<DoubleStyleInterpolation> maybeCreateFromMotionRotation(const CSSValue& start, const CSSValue& end, CSSPropertyID);

    static bool canCreateFrom(const CSSValue&);

    virtual void apply(StyleResolverState&) const override;

    DECLARE_VIRTUAL_TRACE();

private:
    DoubleStyleInterpolation(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, CSSPropertyID id, bool isNumber, InterpolationRange clamp, bool flag)
        : StyleInterpolation(start, end, id)
        , m_isNumber(isNumber)
        , m_clamp(clamp)
        , m_flag(flag)
    { }

    static PassOwnPtrWillBeRawPtr<InterpolableValue> doubleToInterpolableValue(const CSSValue&);
    static PassOwnPtrWillBeRawPtr<InterpolableValue> motionRotationToInterpolableValue(const CSSValue&);

    static PassRefPtrWillBeRawPtr<CSSValue> interpolableValueToDouble(InterpolableValue*, bool, InterpolationRange);
    static PassRefPtrWillBeRawPtr<CSSValue> interpolableValueToMotionRotation(InterpolableValue*, bool);

    bool m_isNumber;
    InterpolationRange m_clamp;
    bool m_flag;

    friend class AnimationDoubleStyleInterpolationTest;
};
}
#endif
