/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Graham Dennis (graham.dennis@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef StyleRareNonInheritedData_h
#define StyleRareNonInheritedData_h

#include "core/css/StyleColor.h"
#include "core/layout/ClipPathOperation.h"
#include "core/style/BasicShapes.h"
#include "core/style/CounterDirectives.h"
#include "core/style/CursorData.h"
#include "core/style/DataRef.h"
#include "core/style/FillLayer.h"
#include "core/style/ComputedStyleConstants.h"
#include "core/style/LineClampValue.h"
#include "core/style/NinePieceImage.h"
#include "core/style/ShapeValue.h"
#include "platform/LengthPoint.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/Vector.h"

namespace blink {

class ContentData;
class CSSAnimationData;
class CSSTransitionData;
class LengthSize;
class ShadowList;
class StyleDeprecatedFlexibleBoxData;
class StyleFilterData;
class StyleFlexibleBoxData;
class StyleGridData;
class StyleGridItemData;
class StyleMultiColData;
class StyleReflection;
class StyleTransformData;
class StyleWillChangeData;

// Page size type.
// StyleRareNonInheritedData::m_pageSize is meaningful only when
// StyleRareNonInheritedData::m_pageSizeType is PAGE_SIZE_RESOLVED.
enum PageSizeType {
    PAGE_SIZE_AUTO, // size: auto
    PAGE_SIZE_AUTO_LANDSCAPE, // size: landscape
    PAGE_SIZE_AUTO_PORTRAIT, // size: portrait
    PAGE_SIZE_RESOLVED // Size is fully resolved.
};

// This struct is for rarely used non-inherited CSS3, CSS2, and WebKit-specific properties.
// By grouping them together, we save space, and only allocate this object when someone
// actually uses one of these properties.
class StyleRareNonInheritedData : public RefCounted<StyleRareNonInheritedData> {
public:
    static PassRefPtr<StyleRareNonInheritedData> create() { return adoptRef(new StyleRareNonInheritedData); }
    PassRefPtr<StyleRareNonInheritedData> copy() const { return adoptRef(new StyleRareNonInheritedData(*this)); }
    ~StyleRareNonInheritedData();

    bool operator==(const StyleRareNonInheritedData&) const;
    bool operator!=(const StyleRareNonInheritedData& o) const { return !(*this == o); }

    bool contentDataEquivalent(const StyleRareNonInheritedData&) const;
    bool counterDataEquivalent(const StyleRareNonInheritedData&) const;
    bool shadowDataEquivalent(const StyleRareNonInheritedData&) const;
    bool reflectionDataEquivalent(const StyleRareNonInheritedData&) const;
    bool animationDataEquivalent(const StyleRareNonInheritedData&) const;
    bool transitionDataEquivalent(const StyleRareNonInheritedData&) const;
    bool shapeOutsideDataEquivalent(const StyleRareNonInheritedData&) const;
    bool clipPathDataEquivalent(const StyleRareNonInheritedData&) const;
    bool hasFilters() const;
    bool hasOpacity() const { return opacity < 1; }

    float opacity; // Whether or not we're transparent.

    float m_perspective;
    LengthPoint m_perspectiveOrigin;

    LineClampValue lineClamp; // An Apple extension.
    DraggableRegionMode m_draggableRegionMode;

    DataRef<StyleDeprecatedFlexibleBoxData> m_deprecatedFlexibleBox; // Flexible box properties
    DataRef<StyleFlexibleBoxData> m_flexibleBox;
    DataRef<StyleMultiColData> m_multiCol; //  CSS3 multicol properties
    DataRef<StyleTransformData> m_transform; // Transform properties (rotate, scale, skew, etc.)
    DataRef<StyleWillChangeData> m_willChange; // CSS Will Change

    DataRef<StyleFilterData> m_filter; // Filter operations (url, sepia, blur, etc.)

    DataRef<StyleGridData> m_grid;
    DataRef<StyleGridItemData> m_gridItem;

    OwnPtr<ContentData> m_content;
    OwnPtr<CounterDirectiveMap> m_counterDirectives;

    RefPtr<ShadowList> m_boxShadow;

    RefPtr<StyleReflection> m_boxReflect;

    OwnPtr<CSSAnimationData> m_animations;
    OwnPtr<CSSTransitionData> m_transitions;

    FillLayer m_mask;
    NinePieceImage m_maskBoxImage;

    LengthSize m_pageSize;

    RefPtr<ShapeValue> m_shapeOutside;
    Length m_shapeMargin;
    float m_shapeImageThreshold;

    RefPtr<ClipPathOperation> m_clipPath;

    StyleColor m_textDecorationColor;
    StyleColor m_visitedLinkTextDecorationColor;
    StyleColor m_visitedLinkBackgroundColor;
    StyleColor m_visitedLinkOutlineColor;
    StyleColor m_visitedLinkBorderLeftColor;
    StyleColor m_visitedLinkBorderRightColor;
    StyleColor m_visitedLinkBorderTopColor;
    StyleColor m_visitedLinkBorderBottomColor;

    int m_order;

    LengthPoint m_objectPosition;

    Vector<String> m_callbackSelectors;

    unsigned m_pageSizeType : 2; // PageSizeType
    unsigned m_transformStyle3D : 1; // ETransformStyle3D
    unsigned m_backfaceVisibility : 1; // EBackfaceVisibility

    unsigned m_alignContent : 4; // ContentPosition
    unsigned m_alignContentDistribution : 3; // ContentDistributionType
    unsigned m_alignContentOverflowAlignment : 2; // OverflowAlignment
    unsigned m_alignItems : 4; // ItemPosition
    unsigned m_alignItemsOverflowAlignment : 2; // OverflowAlignment
    unsigned m_alignSelf : 4; // ItemPosition
    unsigned m_alignSelfOverflowAlignment : 2; // OverflowAlignment
    unsigned m_justifyContent : 4; // ContentPosition
    unsigned m_justifyContentDistribution : 3; // ContentDistributionType
    unsigned m_justifyContentOverflowAlignment : 2; // OverflowAlignment

    unsigned userDrag : 2; // EUserDrag
    unsigned textOverflow : 1; // Whether or not lines that spill out should be truncated with "..."
    unsigned marginBeforeCollapse : 2; // EMarginCollapse
    unsigned marginAfterCollapse : 2; // EMarginCollapse
    unsigned m_appearance : 6; // EAppearance
    unsigned m_textCombine : 1; // CSS3 text-combine properties

    unsigned m_textDecorationStyle : 3; // TextDecorationStyle
    unsigned m_wrapFlow: 3; // WrapFlow
    unsigned m_wrapThrough: 1; // WrapThrough

    unsigned m_hasCurrentOpacityAnimation : 1;
    unsigned m_hasCurrentTransformAnimation : 1;
    unsigned m_hasCurrentFilterAnimation : 1;
    unsigned m_runningOpacityAnimationOnCompositor : 1;
    unsigned m_runningTransformAnimationOnCompositor : 1;
    unsigned m_runningFilterAnimationOnCompositor : 1;

    unsigned m_effectiveBlendMode: 5; // EBlendMode

    unsigned m_touchAction : TouchActionBits; // TouchAction

    unsigned m_objectFit : 3; // ObjectFit

    unsigned m_isolation : 1; // Isolation

    unsigned m_justifyItems : 4; // ItemPosition
    unsigned m_justifyItemsOverflowAlignment : 2; // OverflowAlignment
    unsigned m_justifyItemsPositionType: 1; // Whether or not alignment uses the 'legacy' keyword.

    unsigned m_justifySelf : 4; // ItemPosition
    unsigned m_justifySelfOverflowAlignment : 2; // OverflowAlignment

    // ScrollBehavior. 'scroll-behavior' has 2 accepted values, but ScrollBehavior has a third
    // value (that can only be specified using CSSOM scroll APIs) so 2 bits are needed.
    unsigned m_scrollBehavior: 2;

    unsigned m_scrollBlocksOn: 3; // WebScrollBlocksOn

    // Plugins require accelerated compositing for reasons external to blink.
    // In which case, we need to update the ComputedStyle on the LayoutEmbeddedObject,
    // so store this bit so that the style actually changes when the plugin
    // becomes composited.
    unsigned m_requiresAcceleratedCompositingForExternalReasons: 1;

    // Whether the transform (if it exists) is stored in the element's inline style.
    unsigned m_hasInlineTransform : 1;
    unsigned m_resize : 2; // EResize

private:
    StyleRareNonInheritedData();
    StyleRareNonInheritedData(const StyleRareNonInheritedData&);
};

} // namespace blink

#endif // StyleRareNonInheritedData_h
