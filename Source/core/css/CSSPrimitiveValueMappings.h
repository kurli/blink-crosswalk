/*
 * Copyright (C) 2007 Alexey Proskuryakov <ap@nypop.com>.
 * Copyright (C) 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2009 Jeff Schiller <codedread@gmail.com>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CSSPrimitiveValueMappings_h
#define CSSPrimitiveValueMappings_h

#include "core/CSSValueKeywords.h"
#include "core/css/CSSCalculationValue.h"
#include "core/css/CSSPrimitiveValue.h"
#include "core/css/CSSReflectionDirection.h"
#include "core/css/CSSToLengthConversionData.h"
#include "core/style/SVGComputedStyleDefs.h"
#include "core/style/LineClampValue.h"
#include "core/style/ComputedStyleConstants.h"
#include "platform/Length.h"
#include "platform/ThemeTypes.h"
#include "platform/fonts/FontDescription.h"
#include "platform/fonts/FontSmoothingMode.h"
#include "platform/fonts/TextRenderingMode.h"
#include "platform/graphics/GraphicsTypes.h"
#include "platform/graphics/Path.h"
#include "platform/scroll/ScrollableArea.h"
#include "platform/text/TextDirection.h"
#include "platform/text/TextRun.h"
#include "platform/text/UnicodeBidi.h"
#include "platform/text/WritingMode.h"
#include "public/platform/WebScrollBlocksOn.h"
#include "wtf/MathExtras.h"

namespace blink {

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(short i)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_NUMBER;
    m_value.num = static_cast<double>(i);
}

template<> inline CSSPrimitiveValue::operator short() const
{
    ASSERT(isNumber());
    return clampTo<short>(getDoubleValue());
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(unsigned short i)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_NUMBER;
    m_value.num = static_cast<double>(i);
}

template<> inline CSSPrimitiveValue::operator unsigned short() const
{
    ASSERT(isNumber());
    return clampTo<unsigned short>(getDoubleValue());
}

template<> inline CSSPrimitiveValue::operator int() const
{
    ASSERT(isNumber());
    return clampTo<int>(getDoubleValue());
}

template<> inline CSSPrimitiveValue::operator unsigned() const
{
    ASSERT(isNumber());
    return clampTo<unsigned>(getDoubleValue());
}


template<> inline CSSPrimitiveValue::CSSPrimitiveValue(float i)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_NUMBER;
    m_value.num = static_cast<double>(i);
}

template<> inline CSSPrimitiveValue::operator float() const
{
    ASSERT(isNumber());
    return clampTo<float>(getDoubleValue());
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(LineClampValue i)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = i.isPercentage() ? CSS_PERCENTAGE : CSS_NUMBER;
    m_value.num = static_cast<double>(i.value());
}

template<> inline CSSPrimitiveValue::operator LineClampValue() const
{
    if (m_primitiveUnitType == CSS_NUMBER)
        return LineClampValue(clampTo<int>(m_value.num), LineClampLineCount);

    if (m_primitiveUnitType == CSS_PERCENTAGE)
        return LineClampValue(clampTo<int>(m_value.num), LineClampPercentage);

    ASSERT_NOT_REACHED();
    return LineClampValue();
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(CSSReflectionDirection e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case ReflectionAbove:
        m_value.valueID = CSSValueAbove;
        break;
    case ReflectionBelow:
        m_value.valueID = CSSValueBelow;
        break;
    case ReflectionLeft:
        m_value.valueID = CSSValueLeft;
        break;
    case ReflectionRight:
        m_value.valueID = CSSValueRight;
    }
}

template<> inline CSSPrimitiveValue::operator CSSReflectionDirection() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAbove:
        return ReflectionAbove;
    case CSSValueBelow:
        return ReflectionBelow;
    case CSSValueLeft:
        return ReflectionLeft;
    case CSSValueRight:
        return ReflectionRight;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return ReflectionBelow;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ColumnFill columnFill)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (columnFill) {
    case ColumnFillAuto:
        m_value.valueID = CSSValueAuto;
        break;
    case ColumnFillBalance:
        m_value.valueID = CSSValueBalance;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ColumnFill() const
{
    if (m_primitiveUnitType == CSS_VALUE_ID) {
        if (m_value.valueID == CSSValueBalance)
            return ColumnFillBalance;
        if (m_value.valueID == CSSValueAuto)
            return ColumnFillAuto;
    }
    ASSERT_NOT_REACHED();
    return ColumnFillBalance;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ColumnSpan columnSpan)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (columnSpan) {
    case ColumnSpanAll:
        m_value.valueID = CSSValueAll;
        break;
    case ColumnSpanNone:
        m_value.valueID = CSSValueNone;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ColumnSpan() const
{
    // Map 1 to none for compatibility reasons.
    if (m_primitiveUnitType == CSS_NUMBER && m_value.num == 1)
        return ColumnSpanNone;

    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAll:
        return ColumnSpanAll;
    case CSSValueNone:
        return ColumnSpanNone;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return ColumnSpanNone;
}


template<> inline CSSPrimitiveValue::CSSPrimitiveValue(PrintColorAdjust value)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (value) {
    case PrintColorAdjustExact:
        m_value.valueID = CSSValueExact;
        break;
    case PrintColorAdjustEconomy:
        m_value.valueID = CSSValueEconomy;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator PrintColorAdjust() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueEconomy:
        return PrintColorAdjustEconomy;
    case CSSValueExact:
        return PrintColorAdjustExact;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return PrintColorAdjustEconomy;
}


template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EBorderStyle e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case BNONE:
        m_value.valueID = CSSValueNone;
        break;
    case BHIDDEN:
        m_value.valueID = CSSValueHidden;
        break;
    case INSET:
        m_value.valueID = CSSValueInset;
        break;
    case GROOVE:
        m_value.valueID = CSSValueGroove;
        break;
    case RIDGE:
        m_value.valueID = CSSValueRidge;
        break;
    case OUTSET:
        m_value.valueID = CSSValueOutset;
        break;
    case DOTTED:
        m_value.valueID = CSSValueDotted;
        break;
    case DASHED:
        m_value.valueID = CSSValueDashed;
        break;
    case SOLID:
        m_value.valueID = CSSValueSolid;
        break;
    case DOUBLE:
        m_value.valueID = CSSValueDouble;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EBorderStyle() const
{
    ASSERT(isValueID());
    if (m_value.valueID == CSSValueAuto) // Valid for CSS outline-style
        return DOTTED;
    return (EBorderStyle)(m_value.valueID - CSSValueNone);
}

template<> inline CSSPrimitiveValue::operator OutlineIsAuto() const
{
    if (m_value.valueID == CSSValueAuto)
        return AUTO_ON;
    return AUTO_OFF;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(CompositeOperator e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case CompositeClear:
        m_value.valueID = CSSValueClear;
        break;
    case CompositeCopy:
        m_value.valueID = CSSValueCopy;
        break;
    case CompositeSourceOver:
        m_value.valueID = CSSValueSourceOver;
        break;
    case CompositeSourceIn:
        m_value.valueID = CSSValueSourceIn;
        break;
    case CompositeSourceOut:
        m_value.valueID = CSSValueSourceOut;
        break;
    case CompositeSourceAtop:
        m_value.valueID = CSSValueSourceAtop;
        break;
    case CompositeDestinationOver:
        m_value.valueID = CSSValueDestinationOver;
        break;
    case CompositeDestinationIn:
        m_value.valueID = CSSValueDestinationIn;
        break;
    case CompositeDestinationOut:
        m_value.valueID = CSSValueDestinationOut;
        break;
    case CompositeDestinationAtop:
        m_value.valueID = CSSValueDestinationAtop;
        break;
    case CompositeXOR:
        m_value.valueID = CSSValueXor;
        break;
    case CompositePlusLighter:
        m_value.valueID = CSSValuePlusLighter;
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }
}

template<> inline CSSPrimitiveValue::operator CompositeOperator() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueClear:
        return CompositeClear;
    case CSSValueCopy:
        return CompositeCopy;
    case CSSValueSourceOver:
        return CompositeSourceOver;
    case CSSValueSourceIn:
        return CompositeSourceIn;
    case CSSValueSourceOut:
        return CompositeSourceOut;
    case CSSValueSourceAtop:
        return CompositeSourceAtop;
    case CSSValueDestinationOver:
        return CompositeDestinationOver;
    case CSSValueDestinationIn:
        return CompositeDestinationIn;
    case CSSValueDestinationOut:
        return CompositeDestinationOut;
    case CSSValueDestinationAtop:
        return CompositeDestinationAtop;
    case CSSValueXor:
        return CompositeXOR;
    case CSSValuePlusLighter:
        return CompositePlusLighter;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return CompositeClear;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ControlPart e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case NoControlPart:
        m_value.valueID = CSSValueNone;
        break;
    case CheckboxPart:
        m_value.valueID = CSSValueCheckbox;
        break;
    case RadioPart:
        m_value.valueID = CSSValueRadio;
        break;
    case PushButtonPart:
        m_value.valueID = CSSValuePushButton;
        break;
    case SquareButtonPart:
        m_value.valueID = CSSValueSquareButton;
        break;
    case ButtonPart:
        m_value.valueID = CSSValueButton;
        break;
    case ButtonBevelPart:
        m_value.valueID = CSSValueButtonBevel;
        break;
    case InnerSpinButtonPart:
        m_value.valueID = CSSValueInnerSpinButton;
        break;
    case ListboxPart:
        m_value.valueID = CSSValueListbox;
        break;
    case ListItemPart:
        m_value.valueID = CSSValueListitem;
        break;
    case MediaEnterFullscreenButtonPart:
        m_value.valueID = CSSValueMediaEnterFullscreenButton;
        break;
    case MediaExitFullscreenButtonPart:
        m_value.valueID = CSSValueMediaExitFullscreenButton;
        break;
    case MediaPlayButtonPart:
        m_value.valueID = CSSValueMediaPlayButton;
        break;
    case MediaOverlayPlayButtonPart:
        m_value.valueID = CSSValueMediaOverlayPlayButton;
        break;
    case MediaMuteButtonPart:
        m_value.valueID = CSSValueMediaMuteButton;
        break;
    case MediaToggleClosedCaptionsButtonPart:
        m_value.valueID = CSSValueMediaToggleClosedCaptionsButton;
        break;
    case MediaCastOffButtonPart:
        m_value.valueID = CSSValueInternalMediaCastOffButton;
        break;
    case MediaOverlayCastOffButtonPart:
        m_value.valueID = CSSValueInternalMediaOverlayCastOffButton;
        break;
    case MediaSliderPart:
        m_value.valueID = CSSValueMediaSlider;
        break;
    case MediaSliderThumbPart:
        m_value.valueID = CSSValueMediaSliderthumb;
        break;
    case MediaVolumeSliderContainerPart:
        m_value.valueID = CSSValueMediaVolumeSliderContainer;
        break;
    case MediaVolumeSliderPart:
        m_value.valueID = CSSValueMediaVolumeSlider;
        break;
    case MediaVolumeSliderThumbPart:
        m_value.valueID = CSSValueMediaVolumeSliderthumb;
        break;
    case MediaControlsBackgroundPart:
        m_value.valueID = CSSValueMediaControlsBackground;
        break;
    case MediaControlsFullscreenBackgroundPart:
        m_value.valueID = CSSValueMediaControlsFullscreenBackground;
        break;
    case MediaFullScreenVolumeSliderPart:
        m_value.valueID = CSSValueMediaFullscreenVolumeSlider;
        break;
    case MediaFullScreenVolumeSliderThumbPart:
        m_value.valueID = CSSValueMediaFullscreenVolumeSliderThumb;
        break;
    case MediaCurrentTimePart:
        m_value.valueID = CSSValueMediaCurrentTimeDisplay;
        break;
    case MediaTimeRemainingPart:
        m_value.valueID = CSSValueMediaTimeRemainingDisplay;
        break;
    case MenulistPart:
        m_value.valueID = CSSValueMenulist;
        break;
    case MenulistButtonPart:
        m_value.valueID = CSSValueMenulistButton;
        break;
    case MenulistTextPart:
        m_value.valueID = CSSValueMenulistText;
        break;
    case MenulistTextFieldPart:
        m_value.valueID = CSSValueMenulistTextfield;
        break;
    case MeterPart:
        m_value.valueID = CSSValueMeter;
        break;
    case RelevancyLevelIndicatorPart:
        m_value.valueID = CSSValueRelevancyLevelIndicator;
        break;
    case ContinuousCapacityLevelIndicatorPart:
        m_value.valueID = CSSValueContinuousCapacityLevelIndicator;
        break;
    case DiscreteCapacityLevelIndicatorPart:
        m_value.valueID = CSSValueDiscreteCapacityLevelIndicator;
        break;
    case RatingLevelIndicatorPart:
        m_value.valueID = CSSValueRatingLevelIndicator;
        break;
    case ProgressBarPart:
        m_value.valueID = CSSValueProgressBar;
        break;
    case ProgressBarValuePart:
        m_value.valueID = CSSValueProgressBarValue;
        break;
    case SliderHorizontalPart:
        m_value.valueID = CSSValueSliderHorizontal;
        break;
    case SliderVerticalPart:
        m_value.valueID = CSSValueSliderVertical;
        break;
    case SliderThumbHorizontalPart:
        m_value.valueID = CSSValueSliderthumbHorizontal;
        break;
    case SliderThumbVerticalPart:
        m_value.valueID = CSSValueSliderthumbVertical;
        break;
    case CaretPart:
        m_value.valueID = CSSValueCaret;
        break;
    case SearchFieldPart:
        m_value.valueID = CSSValueSearchfield;
        break;
    case SearchFieldDecorationPart:
        m_value.valueID = CSSValueSearchfieldDecoration;
        break;
    case SearchFieldResultsDecorationPart:
        m_value.valueID = CSSValueSearchfieldResultsDecoration;
        break;
    case SearchFieldCancelButtonPart:
        m_value.valueID = CSSValueSearchfieldCancelButton;
        break;
    case TextFieldPart:
        m_value.valueID = CSSValueTextfield;
        break;
    case TextAreaPart:
        m_value.valueID = CSSValueTextarea;
        break;
    case CapsLockIndicatorPart:
        m_value.valueID = CSSValueCapsLockIndicator;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ControlPart() const
{
    ASSERT(isValueID());
    if (m_value.valueID == CSSValueNone)
        return NoControlPart;
    return ControlPart(m_value.valueID - CSSValueCheckbox + 1);
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EBackfaceVisibility e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case BackfaceVisibilityVisible:
        m_value.valueID = CSSValueVisible;
        break;
    case BackfaceVisibilityHidden:
        m_value.valueID = CSSValueHidden;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EBackfaceVisibility() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueVisible:
        return BackfaceVisibilityVisible;
    case CSSValueHidden:
        return BackfaceVisibilityHidden;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return BackfaceVisibilityHidden;
}


template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EFillAttachment e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case ScrollBackgroundAttachment:
        m_value.valueID = CSSValueScroll;
        break;
    case LocalBackgroundAttachment:
        m_value.valueID = CSSValueLocal;
        break;
    case FixedBackgroundAttachment:
        m_value.valueID = CSSValueFixed;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EFillAttachment() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueScroll:
        return ScrollBackgroundAttachment;
    case CSSValueLocal:
        return LocalBackgroundAttachment;
    case CSSValueFixed:
        return FixedBackgroundAttachment;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return ScrollBackgroundAttachment;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EFillBox e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case BorderFillBox:
        m_value.valueID = CSSValueBorderBox;
        break;
    case PaddingFillBox:
        m_value.valueID = CSSValuePaddingBox;
        break;
    case ContentFillBox:
        m_value.valueID = CSSValueContentBox;
        break;
    case TextFillBox:
        m_value.valueID = CSSValueText;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EFillBox() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueBorder:
    case CSSValueBorderBox:
        return BorderFillBox;
    case CSSValuePadding:
    case CSSValuePaddingBox:
        return PaddingFillBox;
    case CSSValueContent:
    case CSSValueContentBox:
        return ContentFillBox;
    case CSSValueText:
    case CSSValueWebkitText:
        return TextFillBox;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return BorderFillBox;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EFillRepeat e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case RepeatFill:
        m_value.valueID = CSSValueRepeat;
        break;
    case NoRepeatFill:
        m_value.valueID = CSSValueNoRepeat;
        break;
    case RoundFill:
        m_value.valueID = CSSValueRound;
        break;
    case SpaceFill:
        m_value.valueID = CSSValueSpace;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EFillRepeat() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueRepeat:
        return RepeatFill;
    case CSSValueNoRepeat:
        return NoRepeatFill;
    case CSSValueRound:
        return RoundFill;
    case CSSValueSpace:
        return SpaceFill;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return RepeatFill;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EBoxPack e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case Start:
        m_value.valueID = CSSValueStart;
        break;
    case Center:
        m_value.valueID = CSSValueCenter;
        break;
    case End:
        m_value.valueID = CSSValueEnd;
        break;
    case Justify:
        m_value.valueID = CSSValueJustify;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EBoxPack() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueStart:
        return Start;
    case CSSValueEnd:
        return End;
    case CSSValueCenter:
        return Center;
    case CSSValueJustify:
        return Justify;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return Justify;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EBoxAlignment e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case BSTRETCH:
        m_value.valueID = CSSValueStretch;
        break;
    case BSTART:
        m_value.valueID = CSSValueStart;
        break;
    case BCENTER:
        m_value.valueID = CSSValueCenter;
        break;
    case BEND:
        m_value.valueID = CSSValueEnd;
        break;
    case BBASELINE:
        m_value.valueID = CSSValueBaseline;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EBoxAlignment() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueStretch:
        return BSTRETCH;
    case CSSValueStart:
        return BSTART;
    case CSSValueEnd:
        return BEND;
    case CSSValueCenter:
        return BCENTER;
    case CSSValueBaseline:
        return BBASELINE;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return BSTRETCH;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EBoxDecorationBreak e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case DSLICE:
        m_value.valueID = CSSValueSlice;
        break;
    case DCLONE:
        m_value.valueID = CSSValueClone;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EBoxDecorationBreak() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueSlice:
        return DSLICE;
    case CSSValueClone:
        return DCLONE;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return DSLICE;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(BackgroundEdgeOrigin e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case TopEdge:
        m_value.valueID = CSSValueTop;
        break;
    case RightEdge:
        m_value.valueID = CSSValueRight;
        break;
    case BottomEdge:
        m_value.valueID = CSSValueBottom;
        break;
    case LeftEdge:
        m_value.valueID = CSSValueLeft;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator BackgroundEdgeOrigin() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueTop:
        return TopEdge;
    case CSSValueRight:
        return RightEdge;
    case CSSValueBottom:
        return BottomEdge;
    case CSSValueLeft:
        return LeftEdge;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TopEdge;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EBoxSizing e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case BORDER_BOX:
        m_value.valueID = CSSValueBorderBox;
        break;
    case CONTENT_BOX:
        m_value.valueID = CSSValueContentBox;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EBoxSizing() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueBorderBox:
        return BORDER_BOX;
    case CSSValueContentBox:
        return CONTENT_BOX;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return BORDER_BOX;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EBoxDirection e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case BNORMAL:
        m_value.valueID = CSSValueNormal;
        break;
    case BREVERSE:
        m_value.valueID = CSSValueReverse;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EBoxDirection() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueNormal:
        return BNORMAL;
    case CSSValueReverse:
        return BREVERSE;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return BNORMAL;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EBoxLines e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case SINGLE:
        m_value.valueID = CSSValueSingle;
        break;
    case MULTIPLE:
        m_value.valueID = CSSValueMultiple;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EBoxLines() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueSingle:
        return SINGLE;
    case CSSValueMultiple:
        return MULTIPLE;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return SINGLE;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EBoxOrient e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case HORIZONTAL:
        m_value.valueID = CSSValueHorizontal;
        break;
    case VERTICAL:
        m_value.valueID = CSSValueVertical;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EBoxOrient() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueHorizontal:
    case CSSValueInlineAxis:
        return HORIZONTAL;
    case CSSValueVertical:
    case CSSValueBlockAxis:
        return VERTICAL;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return HORIZONTAL;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ECaptionSide e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case CAPLEFT:
        m_value.valueID = CSSValueLeft;
        break;
    case CAPRIGHT:
        m_value.valueID = CSSValueRight;
        break;
    case CAPTOP:
        m_value.valueID = CSSValueTop;
        break;
    case CAPBOTTOM:
        m_value.valueID = CSSValueBottom;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ECaptionSide() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueLeft:
        return CAPLEFT;
    case CSSValueRight:
        return CAPRIGHT;
    case CSSValueTop:
        return CAPTOP;
    case CSSValueBottom:
        return CAPBOTTOM;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return CAPTOP;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EClear e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case CNONE:
        m_value.valueID = CSSValueNone;
        break;
    case CLEFT:
        m_value.valueID = CSSValueLeft;
        break;
    case CRIGHT:
        m_value.valueID = CSSValueRight;
        break;
    case CBOTH:
        m_value.valueID = CSSValueBoth;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EClear() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueNone:
        return CNONE;
    case CSSValueLeft:
        return CLEFT;
    case CSSValueRight:
        return CRIGHT;
    case CSSValueBoth:
        return CBOTH;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return CNONE;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ECursor e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case CURSOR_AUTO:
        m_value.valueID = CSSValueAuto;
        break;
    case CURSOR_CROSS:
        m_value.valueID = CSSValueCrosshair;
        break;
    case CURSOR_DEFAULT:
        m_value.valueID = CSSValueDefault;
        break;
    case CURSOR_POINTER:
        m_value.valueID = CSSValuePointer;
        break;
    case CURSOR_MOVE:
        m_value.valueID = CSSValueMove;
        break;
    case CURSOR_CELL:
        m_value.valueID = CSSValueCell;
        break;
    case CURSOR_VERTICAL_TEXT:
        m_value.valueID = CSSValueVerticalText;
        break;
    case CURSOR_CONTEXT_MENU:
        m_value.valueID = CSSValueContextMenu;
        break;
    case CURSOR_ALIAS:
        m_value.valueID = CSSValueAlias;
        break;
    case CURSOR_COPY:
        m_value.valueID = CSSValueCopy;
        break;
    case CURSOR_NONE:
        m_value.valueID = CSSValueNone;
        break;
    case CURSOR_PROGRESS:
        m_value.valueID = CSSValueProgress;
        break;
    case CURSOR_NO_DROP:
        m_value.valueID = CSSValueNoDrop;
        break;
    case CURSOR_NOT_ALLOWED:
        m_value.valueID = CSSValueNotAllowed;
        break;
    case CURSOR_ZOOM_IN:
        m_value.valueID = CSSValueZoomIn;
        break;
    case CURSOR_ZOOM_OUT:
        m_value.valueID = CSSValueZoomOut;
        break;
    case CURSOR_E_RESIZE:
        m_value.valueID = CSSValueEResize;
        break;
    case CURSOR_NE_RESIZE:
        m_value.valueID = CSSValueNeResize;
        break;
    case CURSOR_NW_RESIZE:
        m_value.valueID = CSSValueNwResize;
        break;
    case CURSOR_N_RESIZE:
        m_value.valueID = CSSValueNResize;
        break;
    case CURSOR_SE_RESIZE:
        m_value.valueID = CSSValueSeResize;
        break;
    case CURSOR_SW_RESIZE:
        m_value.valueID = CSSValueSwResize;
        break;
    case CURSOR_S_RESIZE:
        m_value.valueID = CSSValueSResize;
        break;
    case CURSOR_W_RESIZE:
        m_value.valueID = CSSValueWResize;
        break;
    case CURSOR_EW_RESIZE:
        m_value.valueID = CSSValueEwResize;
        break;
    case CURSOR_NS_RESIZE:
        m_value.valueID = CSSValueNsResize;
        break;
    case CURSOR_NESW_RESIZE:
        m_value.valueID = CSSValueNeswResize;
        break;
    case CURSOR_NWSE_RESIZE:
        m_value.valueID = CSSValueNwseResize;
        break;
    case CURSOR_COL_RESIZE:
        m_value.valueID = CSSValueColResize;
        break;
    case CURSOR_ROW_RESIZE:
        m_value.valueID = CSSValueRowResize;
        break;
    case CURSOR_TEXT:
        m_value.valueID = CSSValueText;
        break;
    case CURSOR_WAIT:
        m_value.valueID = CSSValueWait;
        break;
    case CURSOR_HELP:
        m_value.valueID = CSSValueHelp;
        break;
    case CURSOR_ALL_SCROLL:
        m_value.valueID = CSSValueAllScroll;
        break;
    case CURSOR_WEBKIT_GRAB:
        m_value.valueID = CSSValueWebkitGrab;
        break;
    case CURSOR_WEBKIT_GRABBING:
        m_value.valueID = CSSValueWebkitGrabbing;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ECursor() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueCopy:
        return CURSOR_COPY;
    case CSSValueWebkitZoomIn:
        return CURSOR_ZOOM_IN;
    case CSSValueWebkitZoomOut:
        return CURSOR_ZOOM_OUT;
    case CSSValueNone:
        return CURSOR_NONE;
    default:
        return static_cast<ECursor>(m_value.valueID - CSSValueAuto);
    }
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EDisplay e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case INLINE:
        m_value.valueID = CSSValueInline;
        break;
    case BLOCK:
        m_value.valueID = CSSValueBlock;
        break;
    case LIST_ITEM:
        m_value.valueID = CSSValueListItem;
        break;
    case INLINE_BLOCK:
        m_value.valueID = CSSValueInlineBlock;
        break;
    case TABLE:
        m_value.valueID = CSSValueTable;
        break;
    case INLINE_TABLE:
        m_value.valueID = CSSValueInlineTable;
        break;
    case TABLE_ROW_GROUP:
        m_value.valueID = CSSValueTableRowGroup;
        break;
    case TABLE_HEADER_GROUP:
        m_value.valueID = CSSValueTableHeaderGroup;
        break;
    case TABLE_FOOTER_GROUP:
        m_value.valueID = CSSValueTableFooterGroup;
        break;
    case TABLE_ROW:
        m_value.valueID = CSSValueTableRow;
        break;
    case TABLE_COLUMN_GROUP:
        m_value.valueID = CSSValueTableColumnGroup;
        break;
    case TABLE_COLUMN:
        m_value.valueID = CSSValueTableColumn;
        break;
    case TABLE_CELL:
        m_value.valueID = CSSValueTableCell;
        break;
    case TABLE_CAPTION:
        m_value.valueID = CSSValueTableCaption;
        break;
    case BOX:
        m_value.valueID = CSSValueWebkitBox;
        break;
    case INLINE_BOX:
        m_value.valueID = CSSValueWebkitInlineBox;
        break;
    case FLEX:
        m_value.valueID = CSSValueFlex;
        break;
    case INLINE_FLEX:
        m_value.valueID = CSSValueInlineFlex;
        break;
    case GRID:
        m_value.valueID = CSSValueGrid;
        break;
    case INLINE_GRID:
        m_value.valueID = CSSValueInlineGrid;
        break;
    case NONE:
        m_value.valueID = CSSValueNone;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EDisplay() const
{
    ASSERT(isValueID());
    if (m_value.valueID == CSSValueNone)
        return NONE;

    if (m_value.valueID == CSSValueWebkitFlex)
        return FLEX;
    if (m_value.valueID == CSSValueWebkitInlineFlex)
        return INLINE_FLEX;

    EDisplay display = static_cast<EDisplay>(m_value.valueID - CSSValueInline);
    ASSERT(display >= INLINE && display <= NONE);
    return display;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EEmptyCell e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case SHOW:
        m_value.valueID = CSSValueShow;
        break;
    case HIDE:
        m_value.valueID = CSSValueHide;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EEmptyCell() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueShow:
        return SHOW;
    case CSSValueHide:
        return HIDE;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return SHOW;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EFlexDirection e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case FlowRow:
        m_value.valueID = CSSValueRow;
        break;
    case FlowRowReverse:
        m_value.valueID = CSSValueRowReverse;
        break;
    case FlowColumn:
        m_value.valueID = CSSValueColumn;
        break;
    case FlowColumnReverse:
        m_value.valueID = CSSValueColumnReverse;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EFlexDirection() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueRow:
        return FlowRow;
    case CSSValueRowReverse:
        return FlowRowReverse;
    case CSSValueColumn:
        return FlowColumn;
    case CSSValueColumnReverse:
        return FlowColumnReverse;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return FlowRow;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EFlexWrap e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case FlexNoWrap:
        m_value.valueID = CSSValueNowrap;
        break;
    case FlexWrap:
        m_value.valueID = CSSValueWrap;
        break;
    case FlexWrapReverse:
        m_value.valueID = CSSValueWrapReverse;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EFlexWrap() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueNowrap:
        return FlexNoWrap;
    case CSSValueWrap:
        return FlexWrap;
    case CSSValueWrapReverse:
        return FlexWrapReverse;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return FlexNoWrap;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EFloat e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case NoFloat:
        m_value.valueID = CSSValueNone;
        break;
    case LeftFloat:
        m_value.valueID = CSSValueLeft;
        break;
    case RightFloat:
        m_value.valueID = CSSValueRight;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EFloat() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueLeft:
        return LeftFloat;
    case CSSValueRight:
        return RightFloat;
    case CSSValueNone:
        return NoFloat;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return NoFloat;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(LineBreak e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case LineBreakAuto:
        m_value.valueID = CSSValueAuto;
        break;
    case LineBreakLoose:
        m_value.valueID = CSSValueLoose;
        break;
    case LineBreakNormal:
        m_value.valueID = CSSValueNormal;
        break;
    case LineBreakStrict:
        m_value.valueID = CSSValueStrict;
        break;
    case LineBreakAfterWhiteSpace:
        m_value.valueID = CSSValueAfterWhiteSpace;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator LineBreak() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return LineBreakAuto;
    case CSSValueLoose:
        return LineBreakLoose;
    case CSSValueNormal:
        return LineBreakNormal;
    case CSSValueStrict:
        return LineBreakStrict;
    case CSSValueAfterWhiteSpace:
        return LineBreakAfterWhiteSpace;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return LineBreakAuto;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EListStylePosition e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case OUTSIDE:
        m_value.valueID = CSSValueOutside;
        break;
    case INSIDE:
        m_value.valueID = CSSValueInside;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EListStylePosition() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueOutside:
        return OUTSIDE;
    case CSSValueInside:
        return INSIDE;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return OUTSIDE;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EListStyleType e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case Afar:
        m_value.valueID = CSSValueAfar;
        break;
    case Amharic:
        m_value.valueID = CSSValueAmharic;
        break;
    case AmharicAbegede:
        m_value.valueID = CSSValueAmharicAbegede;
        break;
    case ArabicIndic:
        m_value.valueID = CSSValueArabicIndic;
        break;
    case Armenian:
        m_value.valueID = CSSValueArmenian;
        break;
    case Asterisks:
        m_value.valueID = CSSValueAsterisks;
        break;
    case BinaryListStyle:
        m_value.valueID = CSSValueBinary;
        break;
    case Bengali:
        m_value.valueID = CSSValueBengali;
        break;
    case Cambodian:
        m_value.valueID = CSSValueCambodian;
        break;
    case Circle:
        m_value.valueID = CSSValueCircle;
        break;
    case CjkEarthlyBranch:
        m_value.valueID = CSSValueCjkEarthlyBranch;
        break;
    case CjkHeavenlyStem:
        m_value.valueID = CSSValueCjkHeavenlyStem;
        break;
    case CJKIdeographic:
        m_value.valueID = CSSValueCjkIdeographic;
        break;
    case DecimalLeadingZero:
        m_value.valueID = CSSValueDecimalLeadingZero;
        break;
    case DecimalListStyle:
        m_value.valueID = CSSValueDecimal;
        break;
    case Devanagari:
        m_value.valueID = CSSValueDevanagari;
        break;
    case Disc:
        m_value.valueID = CSSValueDisc;
        break;
    case Ethiopic:
        m_value.valueID = CSSValueEthiopic;
        break;
    case EthiopicAbegede:
        m_value.valueID = CSSValueEthiopicAbegede;
        break;
    case EthiopicAbegedeAmEt:
        m_value.valueID = CSSValueEthiopicAbegedeAmEt;
        break;
    case EthiopicAbegedeGez:
        m_value.valueID = CSSValueEthiopicAbegedeGez;
        break;
    case EthiopicAbegedeTiEr:
        m_value.valueID = CSSValueEthiopicAbegedeTiEr;
        break;
    case EthiopicAbegedeTiEt:
        m_value.valueID = CSSValueEthiopicAbegedeTiEt;
        break;
    case EthiopicHalehameAaEr:
        m_value.valueID = CSSValueEthiopicHalehameAaEr;
        break;
    case EthiopicHalehameAaEt:
        m_value.valueID = CSSValueEthiopicHalehameAaEt;
        break;
    case EthiopicHalehameAmEt:
        m_value.valueID = CSSValueEthiopicHalehameAmEt;
        break;
    case EthiopicHalehameGez:
        m_value.valueID = CSSValueEthiopicHalehameGez;
        break;
    case EthiopicHalehameOmEt:
        m_value.valueID = CSSValueEthiopicHalehameOmEt;
        break;
    case EthiopicHalehameSidEt:
        m_value.valueID = CSSValueEthiopicHalehameSidEt;
        break;
    case EthiopicHalehameSoEt:
        m_value.valueID = CSSValueEthiopicHalehameSoEt;
        break;
    case EthiopicHalehameTiEr:
        m_value.valueID = CSSValueEthiopicHalehameTiEr;
        break;
    case EthiopicHalehameTiEt:
        m_value.valueID = CSSValueEthiopicHalehameTiEt;
        break;
    case EthiopicHalehameTig:
        m_value.valueID = CSSValueEthiopicHalehameTig;
        break;
    case Footnotes:
        m_value.valueID = CSSValueFootnotes;
        break;
    case Georgian:
        m_value.valueID = CSSValueGeorgian;
        break;
    case Gujarati:
        m_value.valueID = CSSValueGujarati;
        break;
    case Gurmukhi:
        m_value.valueID = CSSValueGurmukhi;
        break;
    case Hangul:
        m_value.valueID = CSSValueHangul;
        break;
    case HangulConsonant:
        m_value.valueID = CSSValueHangulConsonant;
        break;
    case Hebrew:
        m_value.valueID = CSSValueHebrew;
        break;
    case Hiragana:
        m_value.valueID = CSSValueHiragana;
        break;
    case HiraganaIroha:
        m_value.valueID = CSSValueHiraganaIroha;
        break;
    case Kannada:
        m_value.valueID = CSSValueKannada;
        break;
    case Katakana:
        m_value.valueID = CSSValueKatakana;
        break;
    case KatakanaIroha:
        m_value.valueID = CSSValueKatakanaIroha;
        break;
    case Khmer:
        m_value.valueID = CSSValueKhmer;
        break;
    case Lao:
        m_value.valueID = CSSValueLao;
        break;
    case LowerAlpha:
        m_value.valueID = CSSValueLowerAlpha;
        break;
    case LowerArmenian:
        m_value.valueID = CSSValueLowerArmenian;
        break;
    case LowerGreek:
        m_value.valueID = CSSValueLowerGreek;
        break;
    case LowerHexadecimal:
        m_value.valueID = CSSValueLowerHexadecimal;
        break;
    case LowerLatin:
        m_value.valueID = CSSValueLowerLatin;
        break;
    case LowerNorwegian:
        m_value.valueID = CSSValueLowerNorwegian;
        break;
    case LowerRoman:
        m_value.valueID = CSSValueLowerRoman;
        break;
    case Malayalam:
        m_value.valueID = CSSValueMalayalam;
        break;
    case Mongolian:
        m_value.valueID = CSSValueMongolian;
        break;
    case Myanmar:
        m_value.valueID = CSSValueMyanmar;
        break;
    case NoneListStyle:
        m_value.valueID = CSSValueNone;
        break;
    case Octal:
        m_value.valueID = CSSValueOctal;
        break;
    case Oriya:
        m_value.valueID = CSSValueOriya;
        break;
    case Oromo:
        m_value.valueID = CSSValueOromo;
        break;
    case Persian:
        m_value.valueID = CSSValuePersian;
        break;
    case Sidama:
        m_value.valueID = CSSValueSidama;
        break;
    case Somali:
        m_value.valueID = CSSValueSomali;
        break;
    case Square:
        m_value.valueID = CSSValueSquare;
        break;
    case Telugu:
        m_value.valueID = CSSValueTelugu;
        break;
    case Thai:
        m_value.valueID = CSSValueThai;
        break;
    case Tibetan:
        m_value.valueID = CSSValueTibetan;
        break;
    case Tigre:
        m_value.valueID = CSSValueTigre;
        break;
    case TigrinyaEr:
        m_value.valueID = CSSValueTigrinyaEr;
        break;
    case TigrinyaErAbegede:
        m_value.valueID = CSSValueTigrinyaErAbegede;
        break;
    case TigrinyaEt:
        m_value.valueID = CSSValueTigrinyaEt;
        break;
    case TigrinyaEtAbegede:
        m_value.valueID = CSSValueTigrinyaEtAbegede;
        break;
    case UpperAlpha:
        m_value.valueID = CSSValueUpperAlpha;
        break;
    case UpperArmenian:
        m_value.valueID = CSSValueUpperArmenian;
        break;
    case UpperGreek:
        m_value.valueID = CSSValueUpperGreek;
        break;
    case UpperHexadecimal:
        m_value.valueID = CSSValueUpperHexadecimal;
        break;
    case UpperLatin:
        m_value.valueID = CSSValueUpperLatin;
        break;
    case UpperNorwegian:
        m_value.valueID = CSSValueUpperNorwegian;
        break;
    case UpperRoman:
        m_value.valueID = CSSValueUpperRoman;
        break;
    case Urdu:
        m_value.valueID = CSSValueUrdu;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EListStyleType() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueNone:
        return NoneListStyle;
    default:
        return static_cast<EListStyleType>(m_value.valueID - CSSValueDisc);
    }
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EMarginCollapse e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case MCOLLAPSE:
        m_value.valueID = CSSValueCollapse;
        break;
    case MSEPARATE:
        m_value.valueID = CSSValueSeparate;
        break;
    case MDISCARD:
        m_value.valueID = CSSValueDiscard;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EMarginCollapse() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueCollapse:
        return MCOLLAPSE;
    case CSSValueSeparate:
        return MSEPARATE;
    case CSSValueDiscard:
        return MDISCARD;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return MCOLLAPSE;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EOverflow e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case OVISIBLE:
        m_value.valueID = CSSValueVisible;
        break;
    case OHIDDEN:
        m_value.valueID = CSSValueHidden;
        break;
    case OSCROLL:
        m_value.valueID = CSSValueScroll;
        break;
    case OAUTO:
        m_value.valueID = CSSValueAuto;
        break;
    case OOVERLAY:
        m_value.valueID = CSSValueOverlay;
        break;
    case OPAGEDX:
        m_value.valueID = CSSValueWebkitPagedX;
        break;
    case OPAGEDY:
        m_value.valueID = CSSValueWebkitPagedY;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EOverflow() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueVisible:
        return OVISIBLE;
    case CSSValueHidden:
        return OHIDDEN;
    case CSSValueScroll:
        return OSCROLL;
    case CSSValueAuto:
        return OAUTO;
    case CSSValueOverlay:
        return OOVERLAY;
    case CSSValueWebkitPagedX:
        return OPAGEDX;
    case CSSValueWebkitPagedY:
        return OPAGEDY;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return OVISIBLE;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EPageBreak e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case PBAUTO:
        m_value.valueID = CSSValueAuto;
        break;
    case PBALWAYS:
        m_value.valueID = CSSValueAlways;
        break;
    case PBAVOID:
        m_value.valueID = CSSValueAvoid;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EPageBreak() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return PBAUTO;
    case CSSValueLeft:
    case CSSValueRight:
    case CSSValueAlways:
        return PBALWAYS; // CSS2.1: "Conforming user agents may map left/right to always."
    case CSSValueAvoid:
        return PBAVOID;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return PBAUTO;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EPosition e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case StaticPosition:
        m_value.valueID = CSSValueStatic;
        break;
    case RelativePosition:
        m_value.valueID = CSSValueRelative;
        break;
    case AbsolutePosition:
        m_value.valueID = CSSValueAbsolute;
        break;
    case FixedPosition:
        m_value.valueID = CSSValueFixed;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EPosition() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueStatic:
        return StaticPosition;
    case CSSValueRelative:
        return RelativePosition;
    case CSSValueAbsolute:
        return AbsolutePosition;
    case CSSValueFixed:
        return FixedPosition;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return StaticPosition;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EResize e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case RESIZE_BOTH:
        m_value.valueID = CSSValueBoth;
        break;
    case RESIZE_HORIZONTAL:
        m_value.valueID = CSSValueHorizontal;
        break;
    case RESIZE_VERTICAL:
        m_value.valueID = CSSValueVertical;
        break;
    case RESIZE_NONE:
        m_value.valueID = CSSValueNone;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EResize() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueBoth:
        return RESIZE_BOTH;
    case CSSValueHorizontal:
        return RESIZE_HORIZONTAL;
    case CSSValueVertical:
        return RESIZE_VERTICAL;
    case CSSValueAuto:
        ASSERT_NOT_REACHED(); // Depends on settings, thus should be handled by the caller.
        return RESIZE_NONE;
    case CSSValueNone:
        return RESIZE_NONE;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return RESIZE_NONE;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ETableLayout e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case TAUTO:
        m_value.valueID = CSSValueAuto;
        break;
    case TFIXED:
        m_value.valueID = CSSValueFixed;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ETableLayout() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueFixed:
        return TFIXED;
    case CSSValueAuto:
        return TAUTO;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TAUTO;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ETextAlign e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case TASTART:
        m_value.valueID = CSSValueStart;
        break;
    case TAEND:
        m_value.valueID = CSSValueEnd;
        break;
    case LEFT:
        m_value.valueID = CSSValueLeft;
        break;
    case RIGHT:
        m_value.valueID = CSSValueRight;
        break;
    case CENTER:
        m_value.valueID = CSSValueCenter;
        break;
    case JUSTIFY:
        m_value.valueID = CSSValueJustify;
        break;
    case WEBKIT_LEFT:
        m_value.valueID = CSSValueWebkitLeft;
        break;
    case WEBKIT_RIGHT:
        m_value.valueID = CSSValueWebkitRight;
        break;
    case WEBKIT_CENTER:
        m_value.valueID = CSSValueWebkitCenter;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ETextAlign() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueWebkitAuto: // Legacy -webkit-auto. Eqiuvalent to start.
    case CSSValueStart:
        return TASTART;
    case CSSValueEnd:
        return TAEND;
    default:
        return static_cast<ETextAlign>(m_value.valueID - CSSValueLeft);
    }
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(TextAlignLast e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case TextAlignLastStart:
        m_value.valueID = CSSValueStart;
        break;
    case TextAlignLastEnd:
        m_value.valueID = CSSValueEnd;
        break;
    case TextAlignLastLeft:
        m_value.valueID = CSSValueLeft;
        break;
    case TextAlignLastRight:
        m_value.valueID = CSSValueRight;
        break;
    case TextAlignLastCenter:
        m_value.valueID = CSSValueCenter;
        break;
    case TextAlignLastJustify:
        m_value.valueID = CSSValueJustify;
        break;
    case TextAlignLastAuto:
        m_value.valueID = CSSValueAuto;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator TextAlignLast() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return TextAlignLastAuto;
    case CSSValueStart:
        return TextAlignLastStart;
    case CSSValueEnd:
        return TextAlignLastEnd;
    case CSSValueLeft:
        return TextAlignLastLeft;
    case CSSValueRight:
        return TextAlignLastRight;
    case CSSValueCenter:
        return TextAlignLastCenter;
    case CSSValueJustify:
        return TextAlignLastJustify;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TextAlignLastAuto;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(TextJustify e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case TextJustifyAuto:
        m_value.valueID = CSSValueAuto;
        break;
    case TextJustifyNone:
        m_value.valueID = CSSValueNone;
        break;
    case TextJustifyInterWord:
        m_value.valueID = CSSValueInterWord;
        break;
    case TextJustifyDistribute:
        m_value.valueID = CSSValueDistribute;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator TextJustify() const
{
    switch (m_value.valueID) {
    case CSSValueAuto:
        return TextJustifyAuto;
    case CSSValueNone:
        return TextJustifyNone;
    case CSSValueInterWord:
        return TextJustifyInterWord;
    case CSSValueDistribute:
        return TextJustifyDistribute;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TextJustifyAuto;
}

template<> inline CSSPrimitiveValue::operator TextDecoration() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueNone:
        return TextDecorationNone;
    case CSSValueUnderline:
        return TextDecorationUnderline;
    case CSSValueOverline:
        return TextDecorationOverline;
    case CSSValueLineThrough:
        return TextDecorationLineThrough;
    case CSSValueBlink:
        return TextDecorationBlink;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TextDecorationNone;
}

template<> inline CSSPrimitiveValue::operator TextDecorationStyle() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueSolid:
        return TextDecorationStyleSolid;
    case CSSValueDouble:
        return TextDecorationStyleDouble;
    case CSSValueDotted:
        return TextDecorationStyleDotted;
    case CSSValueDashed:
        return TextDecorationStyleDashed;
    case CSSValueWavy:
        return TextDecorationStyleWavy;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TextDecorationStyleSolid;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(TextUnderlinePosition e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case TextUnderlinePositionAuto:
        m_value.valueID = CSSValueAuto;
        break;
    case TextUnderlinePositionUnder:
        m_value.valueID = CSSValueUnder;
        break;
    }

    // FIXME: Implement support for 'under left' and 'under right' values.
}

template<> inline CSSPrimitiveValue::operator TextUnderlinePosition() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return TextUnderlinePositionAuto;
    case CSSValueUnder:
        return TextUnderlinePositionUnder;
    default:
        break;
    }

    // FIXME: Implement support for 'under left' and 'under right' values.

    ASSERT_NOT_REACHED();
    return TextUnderlinePositionAuto;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ETextSecurity e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case TSNONE:
        m_value.valueID = CSSValueNone;
        break;
    case TSDISC:
        m_value.valueID = CSSValueDisc;
        break;
    case TSCIRCLE:
        m_value.valueID = CSSValueCircle;
        break;
    case TSSQUARE:
        m_value.valueID = CSSValueSquare;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ETextSecurity() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueNone:
        return TSNONE;
    case CSSValueDisc:
        return TSDISC;
    case CSSValueCircle:
        return TSCIRCLE;
    case CSSValueSquare:
        return TSSQUARE;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TSNONE;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ETextTransform e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case CAPITALIZE:
        m_value.valueID = CSSValueCapitalize;
        break;
    case UPPERCASE:
        m_value.valueID = CSSValueUppercase;
        break;
    case LOWERCASE:
        m_value.valueID = CSSValueLowercase;
        break;
    case TTNONE:
        m_value.valueID = CSSValueNone;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ETextTransform() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueCapitalize:
        return CAPITALIZE;
    case CSSValueUppercase:
        return UPPERCASE;
    case CSSValueLowercase:
        return LOWERCASE;
    case CSSValueNone:
        return TTNONE;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TTNONE;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EUnicodeBidi e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case UBNormal:
        m_value.valueID = CSSValueNormal;
        break;
    case Embed:
        m_value.valueID = CSSValueEmbed;
        break;
    case Override:
        m_value.valueID = CSSValueBidiOverride;
        break;
    case Isolate:
        m_value.valueID = CSSValueWebkitIsolate;
        break;
    case IsolateOverride:
        m_value.valueID = CSSValueWebkitIsolateOverride;
        break;
    case Plaintext:
        m_value.valueID = CSSValueWebkitPlaintext;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EUnicodeBidi() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueNormal:
        return UBNormal;
    case CSSValueEmbed:
        return Embed;
    case CSSValueBidiOverride:
        return Override;
    case CSSValueWebkitIsolate:
        return Isolate;
    case CSSValueWebkitIsolateOverride:
        return IsolateOverride;
    case CSSValueWebkitPlaintext:
        return Plaintext;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return UBNormal;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EUserDrag e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case DRAG_AUTO:
        m_value.valueID = CSSValueAuto;
        break;
    case DRAG_NONE:
        m_value.valueID = CSSValueNone;
        break;
    case DRAG_ELEMENT:
        m_value.valueID = CSSValueElement;
        break;
    default:
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EUserDrag() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return DRAG_AUTO;
    case CSSValueNone:
        return DRAG_NONE;
    case CSSValueElement:
        return DRAG_ELEMENT;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return DRAG_AUTO;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EUserModify e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case READ_ONLY:
        m_value.valueID = CSSValueReadOnly;
        break;
    case READ_WRITE:
        m_value.valueID = CSSValueReadWrite;
        break;
    case READ_WRITE_PLAINTEXT_ONLY:
        m_value.valueID = CSSValueReadWritePlaintextOnly;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EUserModify() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueReadOnly:
        return READ_ONLY;
    case CSSValueReadWrite:
        return READ_WRITE;
    case CSSValueReadWritePlaintextOnly:
        return READ_WRITE_PLAINTEXT_ONLY;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return READ_ONLY;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EUserSelect e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case SELECT_NONE:
        m_value.valueID = CSSValueNone;
        break;
    case SELECT_TEXT:
        m_value.valueID = CSSValueText;
        break;
    case SELECT_ALL:
        m_value.valueID = CSSValueAll;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EUserSelect() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return SELECT_TEXT;
    case CSSValueNone:
        return SELECT_NONE;
    case CSSValueText:
        return SELECT_TEXT;
    case CSSValueAll:
        return SELECT_ALL;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return SELECT_TEXT;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EVerticalAlign a)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (a) {
    case TOP:
        m_value.valueID = CSSValueTop;
        break;
    case BOTTOM:
        m_value.valueID = CSSValueBottom;
        break;
    case MIDDLE:
        m_value.valueID = CSSValueMiddle;
        break;
    case BASELINE:
        m_value.valueID = CSSValueBaseline;
        break;
    case TEXT_BOTTOM:
        m_value.valueID = CSSValueTextBottom;
        break;
    case TEXT_TOP:
        m_value.valueID = CSSValueTextTop;
        break;
    case SUB:
        m_value.valueID = CSSValueSub;
        break;
    case SUPER:
        m_value.valueID = CSSValueSuper;
        break;
    case BASELINE_MIDDLE:
        m_value.valueID = CSSValueWebkitBaselineMiddle;
        break;
    case LENGTH:
        m_value.valueID = CSSValueInvalid;
    }
}

template<> inline CSSPrimitiveValue::operator EVerticalAlign() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueTop:
        return TOP;
    case CSSValueBottom:
        return BOTTOM;
    case CSSValueMiddle:
        return MIDDLE;
    case CSSValueBaseline:
        return BASELINE;
    case CSSValueTextBottom:
        return TEXT_BOTTOM;
    case CSSValueTextTop:
        return TEXT_TOP;
    case CSSValueSub:
        return SUB;
    case CSSValueSuper:
        return SUPER;
    case CSSValueWebkitBaselineMiddle:
        return BASELINE_MIDDLE;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TOP;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EVisibility e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case VISIBLE:
        m_value.valueID = CSSValueVisible;
        break;
    case HIDDEN:
        m_value.valueID = CSSValueHidden;
        break;
    case COLLAPSE:
        m_value.valueID = CSSValueCollapse;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EVisibility() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueHidden:
        return HIDDEN;
    case CSSValueVisible:
        return VISIBLE;
    case CSSValueCollapse:
        return COLLAPSE;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return VISIBLE;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EWhiteSpace e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case NORMAL:
        m_value.valueID = CSSValueNormal;
        break;
    case PRE:
        m_value.valueID = CSSValuePre;
        break;
    case PRE_WRAP:
        m_value.valueID = CSSValuePreWrap;
        break;
    case PRE_LINE:
        m_value.valueID = CSSValuePreLine;
        break;
    case NOWRAP:
        m_value.valueID = CSSValueNowrap;
        break;
    case KHTML_NOWRAP:
        m_value.valueID = CSSValueWebkitNowrap;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EWhiteSpace() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueWebkitNowrap:
        return KHTML_NOWRAP;
    case CSSValueNowrap:
        return NOWRAP;
    case CSSValuePre:
        return PRE;
    case CSSValuePreWrap:
        return PRE_WRAP;
    case CSSValuePreLine:
        return PRE_LINE;
    case CSSValueNormal:
        return NORMAL;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return NORMAL;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EWordBreak e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case NormalWordBreak:
        m_value.valueID = CSSValueNormal;
        break;
    case BreakAllWordBreak:
        m_value.valueID = CSSValueBreakAll;
        break;
    case BreakWordBreak:
        m_value.valueID = CSSValueBreakWord;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EWordBreak() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueBreakAll:
        return BreakAllWordBreak;
    case CSSValueBreakWord:
        return BreakWordBreak;
    case CSSValueNormal:
        return NormalWordBreak;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return NormalWordBreak;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EOverflowWrap e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case NormalOverflowWrap:
        m_value.valueID = CSSValueNormal;
        break;
    case BreakOverflowWrap:
        m_value.valueID = CSSValueBreakWord;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EOverflowWrap() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueBreakWord:
        return BreakOverflowWrap;
    case CSSValueNormal:
        return NormalOverflowWrap;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return NormalOverflowWrap;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(TextDirection e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case LTR:
        m_value.valueID = CSSValueLtr;
        break;
    case RTL:
        m_value.valueID = CSSValueRtl;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator TextDirection() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueLtr:
        return LTR;
    case CSSValueRtl:
        return RTL;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return LTR;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(WritingMode e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case TopToBottomWritingMode:
        m_value.valueID = CSSValueHorizontalTb;
        break;
    case RightToLeftWritingMode:
        m_value.valueID = CSSValueVerticalRl;
        break;
    case LeftToRightWritingMode:
        m_value.valueID = CSSValueVerticalLr;
        break;
    case BottomToTopWritingMode:
        m_value.valueID = CSSValueHorizontalBt;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator WritingMode() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueHorizontalTb:
        return TopToBottomWritingMode;
    case CSSValueVerticalRl:
        return RightToLeftWritingMode;
    case CSSValueVerticalLr:
        return LeftToRightWritingMode;
    case CSSValueHorizontalBt:
        return BottomToTopWritingMode;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TopToBottomWritingMode;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(TextCombine e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case TextCombineNone:
        m_value.valueID = CSSValueNone;
        break;
    case TextCombineHorizontal:
        m_value.valueID = CSSValueHorizontal;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator TextCombine() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueNone:
        return TextCombineNone;
    case CSSValueHorizontal:
        return TextCombineHorizontal;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TextCombineNone;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(RubyPosition position)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (position) {
    case RubyPositionBefore:
        m_value.valueID = CSSValueBefore;
        break;
    case RubyPositionAfter:
        m_value.valueID = CSSValueAfter;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator RubyPosition() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueBefore:
        return RubyPositionBefore;
    case CSSValueAfter:
        return RubyPositionAfter;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return RubyPositionBefore;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(TextEmphasisPosition position)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (position) {
    case TextEmphasisPositionOver:
        m_value.valueID = CSSValueOver;
        break;
    case TextEmphasisPositionUnder:
        m_value.valueID = CSSValueUnder;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator TextEmphasisPosition() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueOver:
        return TextEmphasisPositionOver;
    case CSSValueUnder:
        return TextEmphasisPositionUnder;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TextEmphasisPositionOver;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(TextOverflow overflow)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (overflow) {
    case TextOverflowClip:
        m_value.valueID = CSSValueClip;
        break;
    case TextOverflowEllipsis:
        m_value.valueID = CSSValueEllipsis;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator TextOverflow() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueClip:
        return TextOverflowClip;
    case CSSValueEllipsis:
        return TextOverflowEllipsis;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TextOverflowClip;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(TextEmphasisFill fill)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (fill) {
    case TextEmphasisFillFilled:
        m_value.valueID = CSSValueFilled;
        break;
    case TextEmphasisFillOpen:
        m_value.valueID = CSSValueOpen;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator TextEmphasisFill() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueFilled:
        return TextEmphasisFillFilled;
    case CSSValueOpen:
        return TextEmphasisFillOpen;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TextEmphasisFillFilled;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(TextEmphasisMark mark)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (mark) {
    case TextEmphasisMarkDot:
        m_value.valueID = CSSValueDot;
        break;
    case TextEmphasisMarkCircle:
        m_value.valueID = CSSValueCircle;
        break;
    case TextEmphasisMarkDoubleCircle:
        m_value.valueID = CSSValueDoubleCircle;
        break;
    case TextEmphasisMarkTriangle:
        m_value.valueID = CSSValueTriangle;
        break;
    case TextEmphasisMarkSesame:
        m_value.valueID = CSSValueSesame;
        break;
    case TextEmphasisMarkNone:
    case TextEmphasisMarkAuto:
    case TextEmphasisMarkCustom:
        ASSERT_NOT_REACHED();
        m_value.valueID = CSSValueNone;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator TextEmphasisMark() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueNone:
        return TextEmphasisMarkNone;
    case CSSValueDot:
        return TextEmphasisMarkDot;
    case CSSValueCircle:
        return TextEmphasisMarkCircle;
    case CSSValueDoubleCircle:
        return TextEmphasisMarkDoubleCircle;
    case CSSValueTriangle:
        return TextEmphasisMarkTriangle;
    case CSSValueSesame:
        return TextEmphasisMarkSesame;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TextEmphasisMarkNone;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(TextOrientation e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case TextOrientationSideways:
        m_value.valueID = CSSValueSideways;
        break;
    case TextOrientationSidewaysRight:
        m_value.valueID = CSSValueSidewaysRight;
        break;
    case TextOrientationVerticalRight:
        m_value.valueID = CSSValueVerticalRight;
        break;
    case TextOrientationUpright:
        m_value.valueID = CSSValueUpright;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator TextOrientation() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueSideways:
        return TextOrientationSideways;
    case CSSValueSidewaysRight:
        return TextOrientationSidewaysRight;
    case CSSValueVerticalRight:
        return TextOrientationVerticalRight;
    case CSSValueUpright:
        return TextOrientationUpright;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TextOrientationVerticalRight;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EPointerEvents e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case PE_NONE:
        m_value.valueID = CSSValueNone;
        break;
    case PE_STROKE:
        m_value.valueID = CSSValueStroke;
        break;
    case PE_FILL:
        m_value.valueID = CSSValueFill;
        break;
    case PE_PAINTED:
        m_value.valueID = CSSValuePainted;
        break;
    case PE_VISIBLE:
        m_value.valueID = CSSValueVisible;
        break;
    case PE_VISIBLE_STROKE:
        m_value.valueID = CSSValueVisibleStroke;
        break;
    case PE_VISIBLE_FILL:
        m_value.valueID = CSSValueVisibleFill;
        break;
    case PE_VISIBLE_PAINTED:
        m_value.valueID = CSSValueVisiblePainted;
        break;
    case PE_AUTO:
        m_value.valueID = CSSValueAuto;
        break;
    case PE_ALL:
        m_value.valueID = CSSValueAll;
        break;
    case PE_BOUNDINGBOX:
        m_value.valueID = CSSValueBoundingBox;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EPointerEvents() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAll:
        return PE_ALL;
    case CSSValueAuto:
        return PE_AUTO;
    case CSSValueNone:
        return PE_NONE;
    case CSSValueVisiblePainted:
        return PE_VISIBLE_PAINTED;
    case CSSValueVisibleFill:
        return PE_VISIBLE_FILL;
    case CSSValueVisibleStroke:
        return PE_VISIBLE_STROKE;
    case CSSValueVisible:
        return PE_VISIBLE;
    case CSSValuePainted:
        return PE_PAINTED;
    case CSSValueFill:
        return PE_FILL;
    case CSSValueStroke:
        return PE_STROKE;
    case CSSValueBoundingBox:
        return PE_BOUNDINGBOX;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return PE_ALL;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(FontDescription::Kerning kerning)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (kerning) {
    case FontDescription::AutoKerning:
        m_value.valueID = CSSValueAuto;
        return;
    case FontDescription::NormalKerning:
        m_value.valueID = CSSValueNormal;
        return;
    case FontDescription::NoneKerning:
        m_value.valueID = CSSValueNone;
        return;
    }

    ASSERT_NOT_REACHED();
    m_value.valueID = CSSValueAuto;
}

template<> inline CSSPrimitiveValue::operator FontDescription::Kerning() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return FontDescription::AutoKerning;
    case CSSValueNormal:
        return FontDescription::NormalKerning;
    case CSSValueNone:
        return FontDescription::NoneKerning;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return FontDescription::AutoKerning;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ObjectFit fit)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (fit) {
    case ObjectFitFill:
        m_value.valueID = CSSValueFill;
        break;
    case ObjectFitContain:
        m_value.valueID = CSSValueContain;
        break;
    case ObjectFitCover:
        m_value.valueID = CSSValueCover;
        break;
    case ObjectFitNone:
        m_value.valueID = CSSValueNone;
        break;
    case ObjectFitScaleDown:
        m_value.valueID = CSSValueScaleDown;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ObjectFit() const
{
    switch (m_value.valueID) {
    case CSSValueFill:
        return ObjectFitFill;
    case CSSValueContain:
        return ObjectFitContain;
    case CSSValueCover:
        return ObjectFitCover;
    case CSSValueNone:
        return ObjectFitNone;
    case CSSValueScaleDown:
        return ObjectFitScaleDown;
    default:
        ASSERT_NOT_REACHED();
        return ObjectFitFill;
    }
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EFillSizeType fillSize)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (fillSize) {
    case Contain:
        m_value.valueID = CSSValueContain;
        break;
    case Cover:
        m_value.valueID = CSSValueCover;
        break;
    case SizeNone:
        m_value.valueID = CSSValueNone;
        break;
    case SizeLength:
    default:
        ASSERT_NOT_REACHED();
    }
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(FontSmoothingMode smoothing)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (smoothing) {
    case AutoSmoothing:
        m_value.valueID = CSSValueAuto;
        return;
    case NoSmoothing:
        m_value.valueID = CSSValueNone;
        return;
    case Antialiased:
        m_value.valueID = CSSValueAntialiased;
        return;
    case SubpixelAntialiased:
        m_value.valueID = CSSValueSubpixelAntialiased;
        return;
    }

    ASSERT_NOT_REACHED();
    m_value.valueID = CSSValueAuto;
}

template<> inline CSSPrimitiveValue::operator FontSmoothingMode() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return AutoSmoothing;
    case CSSValueNone:
        return NoSmoothing;
    case CSSValueAntialiased:
        return Antialiased;
    case CSSValueSubpixelAntialiased:
        return SubpixelAntialiased;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return AutoSmoothing;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(FontWeight weight)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (weight) {
    case FontWeight900:
        m_value.valueID = CSSValue900;
        return;
    case FontWeight800:
        m_value.valueID = CSSValue800;
        return;
    case FontWeight700:
        m_value.valueID = CSSValueBold;
        return;
    case FontWeight600:
        m_value.valueID = CSSValue600;
        return;
    case FontWeight500:
        m_value.valueID = CSSValue500;
        return;
    case FontWeight400:
        m_value.valueID = CSSValueNormal;
        return;
    case FontWeight300:
        m_value.valueID = CSSValue300;
        return;
    case FontWeight200:
        m_value.valueID = CSSValue200;
        return;
    case FontWeight100:
        m_value.valueID = CSSValue100;
        return;
    }

    ASSERT_NOT_REACHED();
    m_value.valueID = CSSValueNormal;
}

template<> inline CSSPrimitiveValue::operator FontWeight() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueBold:
        return FontWeightBold;
    case CSSValueNormal:
        return FontWeightNormal;
    case CSSValue900:
        return FontWeight900;
    case CSSValue800:
        return FontWeight800;
    case CSSValue700:
        return FontWeight700;
    case CSSValue600:
        return FontWeight600;
    case CSSValue500:
        return FontWeight500;
    case CSSValue400:
        return FontWeight400;
    case CSSValue300:
        return FontWeight300;
    case CSSValue200:
        return FontWeight200;
    case CSSValue100:
        return FontWeight100;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return FontWeightNormal;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(FontStyle italic)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (italic) {
    case FontStyleNormal:
        m_value.valueID = CSSValueNormal;
        return;
    case FontStyleItalic:
        m_value.valueID = CSSValueItalic;
        return;
    }

    ASSERT_NOT_REACHED();
    m_value.valueID = CSSValueNormal;
}

template<> inline CSSPrimitiveValue::operator FontStyle() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueOblique:
    // FIXME: oblique is the same as italic for the moment...
    case CSSValueItalic:
        return FontStyleItalic;
    case CSSValueNormal:
        return FontStyleNormal;
    default:
        break;
    }
    ASSERT_NOT_REACHED();
    return FontStyleNormal;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(FontStretch stretch)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (stretch) {
    case FontStretchUltraCondensed:
        m_value.valueID = CSSValueUltraCondensed;
        return;
    case FontStretchExtraCondensed:
        m_value.valueID = CSSValueExtraCondensed;
        return;
    case FontStretchCondensed:
        m_value.valueID = CSSValueCondensed;
        return;
    case FontStretchSemiCondensed:
        m_value.valueID = CSSValueSemiCondensed;
        return;
    case FontStretchNormal:
        m_value.valueID = CSSValueNormal;
        return;
    case FontStretchSemiExpanded:
        m_value.valueID = CSSValueSemiExpanded;
        return;
    case FontStretchExpanded:
        m_value.valueID = CSSValueExpanded;
        return;
    case FontStretchExtraExpanded:
        m_value.valueID = CSSValueExtraExpanded;
        return;
    case FontStretchUltraExpanded:
        m_value.valueID = CSSValueUltraExpanded;
        return;
    }

    ASSERT_NOT_REACHED();
    m_value.valueID = CSSValueNormal;
}

template<> inline CSSPrimitiveValue::operator FontStretch() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueUltraCondensed:
        return FontStretchUltraCondensed;
    case CSSValueExtraCondensed:
        return FontStretchExtraCondensed;
    case CSSValueCondensed:
        return FontStretchCondensed;
    case CSSValueSemiCondensed:
        return FontStretchSemiCondensed;
    case CSSValueNormal:
        return FontStretchNormal;
    case CSSValueSemiExpanded:
        return FontStretchSemiExpanded;
    case CSSValueExpanded:
        return FontStretchExpanded;
    case CSSValueExtraExpanded:
        return FontStretchExtraExpanded;
    case CSSValueUltraExpanded:
        return FontStretchUltraExpanded;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return FontStretchNormal;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(FontVariant smallCaps)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (smallCaps) {
    case FontVariantNormal:
        m_value.valueID = CSSValueNormal;
        return;
    case FontVariantSmallCaps:
        m_value.valueID = CSSValueSmallCaps;
        return;
    }

    ASSERT_NOT_REACHED();
    m_value.valueID = CSSValueNormal;
}

template<> inline CSSPrimitiveValue::operator FontVariant() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueSmallCaps:
        return FontVariantSmallCaps;
    case CSSValueNormal:
        return FontVariantNormal;
    default:
        break;
    }
    ASSERT_NOT_REACHED();
    return FontVariantNormal;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(TextRenderingMode e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case AutoTextRendering:
        m_value.valueID = CSSValueAuto;
        break;
    case OptimizeSpeed:
        m_value.valueID = CSSValueOptimizeSpeed;
        break;
    case OptimizeLegibility:
        m_value.valueID = CSSValueOptimizeLegibility;
        break;
    case GeometricPrecision:
        m_value.valueID = CSSValueGeometricPrecision;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator TextRenderingMode() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return AutoTextRendering;
    case CSSValueOptimizeSpeed:
        return OptimizeSpeed;
    case CSSValueOptimizeLegibility:
        return OptimizeLegibility;
    case CSSValueGeometricPrecision:
        return GeometricPrecision;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return AutoTextRendering;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ESpeak e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case SpeakNone:
        m_value.valueID = CSSValueNone;
        break;
    case SpeakNormal:
        m_value.valueID = CSSValueNormal;
        break;
    case SpeakSpellOut:
        m_value.valueID = CSSValueSpellOut;
        break;
    case SpeakDigits:
        m_value.valueID = CSSValueDigits;
        break;
    case SpeakLiteralPunctuation:
        m_value.valueID = CSSValueLiteralPunctuation;
        break;
    case SpeakNoPunctuation:
        m_value.valueID = CSSValueNoPunctuation;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator Order() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueLogical:
        return LogicalOrder;
    case CSSValueVisual:
        return VisualOrder;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return LogicalOrder;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(Order e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case LogicalOrder:
        m_value.valueID = CSSValueLogical;
        break;
    case VisualOrder:
        m_value.valueID = CSSValueVisual;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ESpeak() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueNone:
        return SpeakNone;
    case CSSValueNormal:
        return SpeakNormal;
    case CSSValueSpellOut:
        return SpeakSpellOut;
    case CSSValueDigits:
        return SpeakDigits;
    case CSSValueLiteralPunctuation:
        return SpeakLiteralPunctuation;
    case CSSValueNoPunctuation:
        return SpeakNoPunctuation;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return SpeakNormal;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(blink::WebBlendMode blendMode)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (blendMode) {
    case blink::WebBlendModeNormal:
        m_value.valueID = CSSValueNormal;
        break;
    case blink::WebBlendModeMultiply:
        m_value.valueID = CSSValueMultiply;
        break;
    case blink::WebBlendModeScreen:
        m_value.valueID = CSSValueScreen;
        break;
    case blink::WebBlendModeOverlay:
        m_value.valueID = CSSValueOverlay;
        break;
    case blink::WebBlendModeDarken:
        m_value.valueID = CSSValueDarken;
        break;
    case blink::WebBlendModeLighten:
        m_value.valueID = CSSValueLighten;
        break;
    case blink::WebBlendModeColorDodge:
        m_value.valueID = CSSValueColorDodge;
        break;
    case blink::WebBlendModeColorBurn:
        m_value.valueID = CSSValueColorBurn;
        break;
    case blink::WebBlendModeHardLight:
        m_value.valueID = CSSValueHardLight;
        break;
    case blink::WebBlendModeSoftLight:
        m_value.valueID = CSSValueSoftLight;
        break;
    case blink::WebBlendModeDifference:
        m_value.valueID = CSSValueDifference;
        break;
    case blink::WebBlendModeExclusion:
        m_value.valueID = CSSValueExclusion;
        break;
    case blink::WebBlendModeHue:
        m_value.valueID = CSSValueHue;
        break;
    case blink::WebBlendModeSaturation:
        m_value.valueID = CSSValueSaturation;
        break;
    case blink::WebBlendModeColor:
        m_value.valueID = CSSValueColor;
        break;
    case blink::WebBlendModeLuminosity:
        m_value.valueID = CSSValueLuminosity;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator blink::WebBlendMode() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueNormal:
        return blink::WebBlendModeNormal;
    case CSSValueMultiply:
        return blink::WebBlendModeMultiply;
    case CSSValueScreen:
        return blink::WebBlendModeScreen;
    case CSSValueOverlay:
        return blink::WebBlendModeOverlay;
    case CSSValueDarken:
        return blink::WebBlendModeDarken;
    case CSSValueLighten:
        return blink::WebBlendModeLighten;
    case CSSValueColorDodge:
        return blink::WebBlendModeColorDodge;
    case CSSValueColorBurn:
        return blink::WebBlendModeColorBurn;
    case CSSValueHardLight:
        return blink::WebBlendModeHardLight;
    case CSSValueSoftLight:
        return blink::WebBlendModeSoftLight;
    case CSSValueDifference:
        return blink::WebBlendModeDifference;
    case CSSValueExclusion:
        return blink::WebBlendModeExclusion;
    case CSSValueHue:
        return blink::WebBlendModeHue;
    case CSSValueSaturation:
        return blink::WebBlendModeSaturation;
    case CSSValueColor:
        return blink::WebBlendModeColor;
    case CSSValueLuminosity:
        return blink::WebBlendModeLuminosity;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return blink::WebBlendModeNormal;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(LineCap e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case ButtCap:
        m_value.valueID = CSSValueButt;
        break;
    case RoundCap:
        m_value.valueID = CSSValueRound;
        break;
    case SquareCap:
        m_value.valueID = CSSValueSquare;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator LineCap() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueButt:
        return ButtCap;
    case CSSValueRound:
        return RoundCap;
    case CSSValueSquare:
        return SquareCap;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return ButtCap;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(LineJoin e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case MiterJoin:
        m_value.valueID = CSSValueMiter;
        break;
    case RoundJoin:
        m_value.valueID = CSSValueRound;
        break;
    case BevelJoin:
        m_value.valueID = CSSValueBevel;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator LineJoin() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueMiter:
        return MiterJoin;
    case CSSValueRound:
        return RoundJoin;
    case CSSValueBevel:
        return BevelJoin;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return MiterJoin;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(WindRule e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case RULE_NONZERO:
        m_value.valueID = CSSValueNonzero;
        break;
    case RULE_EVENODD:
        m_value.valueID = CSSValueEvenodd;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator WindRule() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueNonzero:
        return RULE_NONZERO;
    case CSSValueEvenodd:
        return RULE_EVENODD;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return RULE_NONZERO;
}


template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EAlignmentBaseline e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case AB_AUTO:
        m_value.valueID = CSSValueAuto;
        break;
    case AB_BASELINE:
        m_value.valueID = CSSValueBaseline;
        break;
    case AB_BEFORE_EDGE:
        m_value.valueID = CSSValueBeforeEdge;
        break;
    case AB_TEXT_BEFORE_EDGE:
        m_value.valueID = CSSValueTextBeforeEdge;
        break;
    case AB_MIDDLE:
        m_value.valueID = CSSValueMiddle;
        break;
    case AB_CENTRAL:
        m_value.valueID = CSSValueCentral;
        break;
    case AB_AFTER_EDGE:
        m_value.valueID = CSSValueAfterEdge;
        break;
    case AB_TEXT_AFTER_EDGE:
        m_value.valueID = CSSValueTextAfterEdge;
        break;
    case AB_IDEOGRAPHIC:
        m_value.valueID = CSSValueIdeographic;
        break;
    case AB_ALPHABETIC:
        m_value.valueID = CSSValueAlphabetic;
        break;
    case AB_HANGING:
        m_value.valueID = CSSValueHanging;
        break;
    case AB_MATHEMATICAL:
        m_value.valueID = CSSValueMathematical;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EAlignmentBaseline() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return AB_AUTO;
    case CSSValueBaseline:
        return AB_BASELINE;
    case CSSValueBeforeEdge:
        return AB_BEFORE_EDGE;
    case CSSValueTextBeforeEdge:
        return AB_TEXT_BEFORE_EDGE;
    case CSSValueMiddle:
        return AB_MIDDLE;
    case CSSValueCentral:
        return AB_CENTRAL;
    case CSSValueAfterEdge:
        return AB_AFTER_EDGE;
    case CSSValueTextAfterEdge:
        return AB_TEXT_AFTER_EDGE;
    case CSSValueIdeographic:
        return AB_IDEOGRAPHIC;
    case CSSValueAlphabetic:
        return AB_ALPHABETIC;
    case CSSValueHanging:
        return AB_HANGING;
    case CSSValueMathematical:
        return AB_MATHEMATICAL;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return AB_AUTO;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EBorderCollapse e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case BSEPARATE:
        m_value.valueID = CSSValueSeparate;
        break;
    case BCOLLAPSE:
        m_value.valueID = CSSValueCollapse;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EBorderCollapse() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueSeparate:
        return BSEPARATE;
    case CSSValueCollapse:
        return BCOLLAPSE;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return BSEPARATE;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EImageRendering e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case ImageRenderingAuto:
        m_value.valueID = CSSValueAuto;
        break;
    case ImageRenderingOptimizeSpeed:
        m_value.valueID = CSSValueOptimizeSpeed;
        break;
    case ImageRenderingOptimizeQuality:
        m_value.valueID = CSSValueOptimizeQuality;
        break;
    case ImageRenderingPixelated:
        m_value.valueID = CSSValuePixelated;
        break;
    case ImageRenderingOptimizeContrast:
        m_value.valueID = CSSValueWebkitOptimizeContrast;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EImageRendering() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return ImageRenderingAuto;
    case CSSValueOptimizeSpeed:
        return ImageRenderingOptimizeSpeed;
    case CSSValueOptimizeQuality:
        return ImageRenderingOptimizeQuality;
    case CSSValuePixelated:
        return ImageRenderingPixelated;
    case CSSValueWebkitOptimizeContrast:
        return ImageRenderingOptimizeContrast;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return ImageRenderingAuto;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ETransformStyle3D e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case TransformStyle3DFlat:
        m_value.valueID = CSSValueFlat;
        break;
    case TransformStyle3DPreserve3D:
        m_value.valueID = CSSValuePreserve3d;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ETransformStyle3D() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueFlat:
        return TransformStyle3DFlat;
    case CSSValuePreserve3d:
        return TransformStyle3DPreserve3D;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TransformStyle3DFlat;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EBufferedRendering e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case BR_AUTO:
        m_value.valueID = CSSValueAuto;
        break;
    case BR_DYNAMIC:
        m_value.valueID = CSSValueDynamic;
        break;
    case BR_STATIC:
        m_value.valueID = CSSValueStatic;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EBufferedRendering() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return BR_AUTO;
    case CSSValueDynamic:
        return BR_DYNAMIC;
    case CSSValueStatic:
        return BR_STATIC;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return BR_AUTO;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EColorInterpolation e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case CI_AUTO:
        m_value.valueID = CSSValueAuto;
        break;
    case CI_SRGB:
        m_value.valueID = CSSValueSRGB;
        break;
    case CI_LINEARRGB:
        m_value.valueID = CSSValueLinearRGB;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EColorInterpolation() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueSRGB:
        return CI_SRGB;
    case CSSValueLinearRGB:
        return CI_LINEARRGB;
    case CSSValueAuto:
        return CI_AUTO;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return CI_AUTO;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EColorRendering e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case CR_AUTO:
        m_value.valueID = CSSValueAuto;
        break;
    case CR_OPTIMIZESPEED:
        m_value.valueID = CSSValueOptimizeSpeed;
        break;
    case CR_OPTIMIZEQUALITY:
        m_value.valueID = CSSValueOptimizeQuality;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EColorRendering() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueOptimizeSpeed:
        return CR_OPTIMIZESPEED;
    case CSSValueOptimizeQuality:
        return CR_OPTIMIZEQUALITY;
    case CSSValueAuto:
        return CR_AUTO;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return CR_AUTO;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EDominantBaseline e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case DB_AUTO:
        m_value.valueID = CSSValueAuto;
        break;
    case DB_USE_SCRIPT:
        m_value.valueID = CSSValueUseScript;
        break;
    case DB_NO_CHANGE:
        m_value.valueID = CSSValueNoChange;
        break;
    case DB_RESET_SIZE:
        m_value.valueID = CSSValueResetSize;
        break;
    case DB_CENTRAL:
        m_value.valueID = CSSValueCentral;
        break;
    case DB_MIDDLE:
        m_value.valueID = CSSValueMiddle;
        break;
    case DB_TEXT_BEFORE_EDGE:
        m_value.valueID = CSSValueTextBeforeEdge;
        break;
    case DB_TEXT_AFTER_EDGE:
        m_value.valueID = CSSValueTextAfterEdge;
        break;
    case DB_IDEOGRAPHIC:
        m_value.valueID = CSSValueIdeographic;
        break;
    case DB_ALPHABETIC:
        m_value.valueID = CSSValueAlphabetic;
        break;
    case DB_HANGING:
        m_value.valueID = CSSValueHanging;
        break;
    case DB_MATHEMATICAL:
        m_value.valueID = CSSValueMathematical;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EDominantBaseline() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return DB_AUTO;
    case CSSValueUseScript:
        return DB_USE_SCRIPT;
    case CSSValueNoChange:
        return DB_NO_CHANGE;
    case CSSValueResetSize:
        return DB_RESET_SIZE;
    case CSSValueIdeographic:
        return DB_IDEOGRAPHIC;
    case CSSValueAlphabetic:
        return DB_ALPHABETIC;
    case CSSValueHanging:
        return DB_HANGING;
    case CSSValueMathematical:
        return DB_MATHEMATICAL;
    case CSSValueCentral:
        return DB_CENTRAL;
    case CSSValueMiddle:
        return DB_MIDDLE;
    case CSSValueTextAfterEdge:
        return DB_TEXT_AFTER_EDGE;
    case CSSValueTextBeforeEdge:
        return DB_TEXT_BEFORE_EDGE;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return DB_AUTO;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EShapeRendering e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case SR_AUTO:
        m_value.valueID = CSSValueAuto;
        break;
    case SR_OPTIMIZESPEED:
        m_value.valueID = CSSValueOptimizeSpeed;
        break;
    case SR_CRISPEDGES:
        m_value.valueID = CSSValueCrispEdges;
        break;
    case SR_GEOMETRICPRECISION:
        m_value.valueID = CSSValueGeometricPrecision;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EShapeRendering() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return SR_AUTO;
    case CSSValueOptimizeSpeed:
        return SR_OPTIMIZESPEED;
    case CSSValueCrispEdges:
        return SR_CRISPEDGES;
    case CSSValueGeometricPrecision:
        return SR_GEOMETRICPRECISION;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return SR_AUTO;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ETextAnchor e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case TA_START:
        m_value.valueID = CSSValueStart;
        break;
    case TA_MIDDLE:
        m_value.valueID = CSSValueMiddle;
        break;
    case TA_END:
        m_value.valueID = CSSValueEnd;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ETextAnchor() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueStart:
        return TA_START;
    case CSSValueMiddle:
        return TA_MIDDLE;
    case CSSValueEnd:
        return TA_END;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TA_START;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(SVGWritingMode e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case WM_LRTB:
        m_value.valueID = CSSValueLrTb;
        break;
    case WM_LR:
        m_value.valueID = CSSValueLr;
        break;
    case WM_RLTB:
        m_value.valueID = CSSValueRlTb;
        break;
    case WM_RL:
        m_value.valueID = CSSValueRl;
        break;
    case WM_TBRL:
        m_value.valueID = CSSValueTbRl;
        break;
    case WM_TB:
        m_value.valueID = CSSValueTb;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator SVGWritingMode() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueLrTb:
        return WM_LRTB;
    case CSSValueLr:
        return WM_LR;
    case CSSValueRlTb:
        return WM_RLTB;
    case CSSValueRl:
        return WM_RL;
    case CSSValueTbRl:
        return WM_TBRL;
    case CSSValueTb:
        return WM_TB;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return WM_LRTB;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EVectorEffect e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case VE_NONE:
        m_value.valueID = CSSValueNone;
        break;
    case VE_NON_SCALING_STROKE:
        m_value.valueID = CSSValueNonScalingStroke;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EVectorEffect() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueNone:
        return VE_NONE;
    case CSSValueNonScalingStroke:
        return VE_NON_SCALING_STROKE;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return VE_NONE;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EPaintOrderType e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case PT_FILL:
        m_value.valueID = CSSValueFill;
        break;
    case PT_STROKE:
        m_value.valueID = CSSValueStroke;
        break;
    case PT_MARKERS:
        m_value.valueID = CSSValueMarkers;
        break;
    default:
        ASSERT_NOT_REACHED();
        m_value.valueID = CSSValueFill;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EPaintOrderType() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueFill:
        return PT_FILL;
    case CSSValueStroke:
        return PT_STROKE;
    case CSSValueMarkers:
        return PT_MARKERS;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return PT_NONE;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EMaskType e)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (e) {
    case MT_LUMINANCE:
        m_value.valueID = CSSValueLuminance;
        break;
    case MT_ALPHA:
        m_value.valueID = CSSValueAlpha;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EMaskType() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueLuminance:
        return MT_LUMINANCE;
    case CSSValueAlpha:
        return MT_ALPHA;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return MT_LUMINANCE;
}

template<> inline CSSPrimitiveValue::operator TouchAction() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueNone:
        return TouchActionNone;
    case CSSValueAuto:
        return TouchActionAuto;
    case CSSValuePanX:
        return TouchActionPanX;
    case CSSValuePanY:
        return TouchActionPanY;
    case CSSValueManipulation:
        return TouchActionPanX | TouchActionPanY | TouchActionPinchZoom;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TouchActionNone;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(EIsolation i)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (i) {
    case IsolationAuto:
        m_value.valueID = CSSValueAuto;
        break;
    case IsolationIsolate:
        m_value.valueID = CSSValueIsolate;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator EIsolation() const
{
    ASSERT(isValueID());
    switch (m_value.valueID) {
    case CSSValueAuto:
        return IsolationAuto;
    case CSSValueIsolate:
        return IsolationIsolate;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return IsolationAuto;
}

template<> inline CSSPrimitiveValue::operator WebScrollBlocksOn() const
{
    switch (m_value.valueID) {
    case CSSValueNone:
        return WebScrollBlocksOnNone;
    case CSSValueStartTouch:
        return WebScrollBlocksOnStartTouch;
    case CSSValueWheelEvent:
        return WebScrollBlocksOnWheelEvent;
    case CSSValueScrollEvent:
        return WebScrollBlocksOnScrollEvent;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return WebScrollBlocksOnNone;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(CSSBoxType cssBox)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (cssBox) {
    case MarginBox:
        m_value.valueID = CSSValueMarginBox;
        break;
    case BorderBox:
        m_value.valueID = CSSValueBorderBox;
        break;
    case PaddingBox:
        m_value.valueID = CSSValuePaddingBox;
        break;
    case ContentBox:
        m_value.valueID = CSSValueContentBox;
        break;
    case BoxMissing:
        // The missing box should convert to a null primitive value.
        ASSERT_NOT_REACHED();
    }
}

template<> inline CSSPrimitiveValue::operator CSSBoxType() const
{
    switch (getValueID()) {
    case CSSValueMarginBox:
        return MarginBox;
    case CSSValueBorderBox:
        return BorderBox;
    case CSSValuePaddingBox:
        return PaddingBox;
    case CSSValueContentBox:
        return ContentBox;
    default:
        break;
    }
    ASSERT_NOT_REACHED();
    return ContentBox;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ItemPosition itemPosition)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (itemPosition) {
    case ItemPositionAuto:
        m_value.valueID = CSSValueAuto;
        break;
    case ItemPositionStretch:
        m_value.valueID = CSSValueStretch;
        break;
    case ItemPositionBaseline:
        m_value.valueID = CSSValueBaseline;
        break;
    case ItemPositionLastBaseline:
        m_value.valueID = CSSValueLastBaseline;
        break;
    case ItemPositionCenter:
        m_value.valueID = CSSValueCenter;
        break;
    case ItemPositionStart:
        m_value.valueID = CSSValueStart;
        break;
    case ItemPositionEnd:
        m_value.valueID = CSSValueEnd;
        break;
    case ItemPositionSelfStart:
        m_value.valueID = CSSValueSelfStart;
        break;
    case ItemPositionSelfEnd:
        m_value.valueID = CSSValueSelfEnd;
        break;
    case ItemPositionFlexStart:
        m_value.valueID = CSSValueFlexStart;
        break;
    case ItemPositionFlexEnd:
        m_value.valueID = CSSValueFlexEnd;
        break;
    case ItemPositionLeft:
        m_value.valueID = CSSValueLeft;
        break;
    case ItemPositionRight:
        m_value.valueID = CSSValueRight;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ItemPosition() const
{
    switch (m_value.valueID) {
    case CSSValueAuto:
        return ItemPositionAuto;
    case CSSValueStretch:
        return ItemPositionStretch;
    case CSSValueBaseline:
        return ItemPositionBaseline;
    case CSSValueLastBaseline:
        return ItemPositionLastBaseline;
    case CSSValueCenter:
        return ItemPositionCenter;
    case CSSValueStart:
        return ItemPositionStart;
    case CSSValueEnd:
        return ItemPositionEnd;
    case CSSValueSelfStart:
        return ItemPositionSelfStart;
    case CSSValueSelfEnd:
        return ItemPositionSelfEnd;
    case CSSValueFlexStart:
        return ItemPositionFlexStart;
    case CSSValueFlexEnd:
        return ItemPositionFlexEnd;
    case CSSValueLeft:
        return ItemPositionLeft;
    case CSSValueRight:
        return ItemPositionRight;
    default:
        break;
    }
    ASSERT_NOT_REACHED();
    return ItemPositionAuto;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ContentPosition contentPosition)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (contentPosition) {
    case ContentPositionAuto:
        m_value.valueID = CSSValueAuto;
        break;
    case ContentPositionBaseline:
        m_value.valueID = CSSValueBaseline;
        break;
    case ContentPositionLastBaseline:
        m_value.valueID = CSSValueLastBaseline;
        break;
    case ContentPositionCenter:
        m_value.valueID = CSSValueCenter;
        break;
    case ContentPositionStart:
        m_value.valueID = CSSValueStart;
        break;
    case ContentPositionEnd:
        m_value.valueID = CSSValueEnd;
        break;
    case ContentPositionFlexStart:
        m_value.valueID = CSSValueFlexStart;
        break;
    case ContentPositionFlexEnd:
        m_value.valueID = CSSValueFlexEnd;
        break;
    case ContentPositionLeft:
        m_value.valueID = CSSValueLeft;
        break;
    case ContentPositionRight:
        m_value.valueID = CSSValueRight;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ContentPosition() const
{
    switch (m_value.valueID) {
    case CSSValueAuto:
        return ContentPositionAuto;
    case CSSValueBaseline:
        return ContentPositionBaseline;
    case CSSValueLastBaseline:
        return ContentPositionLastBaseline;
    case CSSValueCenter:
        return ContentPositionCenter;
    case CSSValueStart:
        return ContentPositionStart;
    case CSSValueEnd:
        return ContentPositionEnd;
    case CSSValueFlexStart:
        return ContentPositionFlexStart;
    case CSSValueFlexEnd:
        return ContentPositionFlexEnd;
    case CSSValueLeft:
        return ContentPositionLeft;
    case CSSValueRight:
        return ContentPositionRight;
    default:
        break;
    }
    ASSERT_NOT_REACHED();
    return ContentPositionAuto;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ContentDistributionType contentDistribution)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (contentDistribution) {
    case ContentDistributionDefault:
        m_value.valueID = CSSValueDefault;
        break;
    case ContentDistributionSpaceBetween:
        m_value.valueID = CSSValueSpaceBetween;
        break;
    case ContentDistributionSpaceAround:
        m_value.valueID = CSSValueSpaceAround;
        break;
    case ContentDistributionSpaceEvenly:
        m_value.valueID = CSSValueSpaceEvenly;
        break;
    case ContentDistributionStretch:
        m_value.valueID = CSSValueStretch;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator ContentDistributionType() const
{
    switch (m_value.valueID) {
    case CSSValueSpaceBetween:
        return ContentDistributionSpaceBetween;
    case CSSValueSpaceAround:
        return ContentDistributionSpaceAround;
    case CSSValueSpaceEvenly:
        return ContentDistributionSpaceEvenly;
    case CSSValueStretch:
        return ContentDistributionStretch;
    default:
        break;
    }
    ASSERT_NOT_REACHED();
    return ContentDistributionStretch;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(OverflowAlignment overflowAlignment)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (overflowAlignment) {
    case OverflowAlignmentDefault:
        m_value.valueID = CSSValueDefault;
        break;
    case OverflowAlignmentTrue:
        m_value.valueID = CSSValueTrue;
        break;
    case OverflowAlignmentSafe:
        m_value.valueID = CSSValueSafe;
        break;
    }
}

template<> inline CSSPrimitiveValue::operator OverflowAlignment() const
{
    switch (m_value.valueID) {
    case CSSValueTrue:
        return OverflowAlignmentTrue;
    case CSSValueSafe:
        return OverflowAlignmentSafe;
    default:
        break;
    }
    ASSERT_NOT_REACHED();
    return OverflowAlignmentTrue;
}

template<> inline CSSPrimitiveValue::CSSPrimitiveValue(ScrollBehavior behavior)
    : CSSValue(PrimitiveClass)
{
    m_primitiveUnitType = CSS_VALUE_ID;
    switch (behavior) {
    case ScrollBehaviorAuto:
        m_value.valueID = CSSValueAuto;
        break;
    case ScrollBehaviorSmooth:
        m_value.valueID = CSSValueSmooth;
        break;
    case ScrollBehaviorInstant:
        // Behavior 'instant' is only allowed in ScrollOptions arguments passed to
        // CSSOM scroll APIs.
        ASSERT_NOT_REACHED();
    }
}

template<> inline CSSPrimitiveValue::operator ScrollBehavior() const
{
    switch (getValueID()) {
    case CSSValueAuto:
        return ScrollBehaviorAuto;
    case CSSValueSmooth:
        return ScrollBehaviorSmooth;
    default:
        break;
    }
    ASSERT_NOT_REACHED();
    return ScrollBehaviorAuto;
}

}

#endif
