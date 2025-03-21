// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/LayerClipRecorder.h"

#include "core/layout/ClipRect.h"
#include "core/layout/LayoutView.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/geometry/IntRect.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/graphics/paint/ClipRecorder.h"
#include "platform/graphics/paint/DisplayItemList.h"

namespace blink {

LayerClipRecorder::LayerClipRecorder(GraphicsContext& graphicsContext, const LayoutBoxModelObject& layoutObject, DisplayItem::Type clipType, const ClipRect& clipRect,
    const DeprecatedPaintLayerPaintingInfo* localPaintingInfo, const LayoutPoint& fragmentOffset, PaintLayerFlags paintFlags, BorderRadiusClippingRule rule)
    : m_graphicsContext(graphicsContext)
    , m_layoutObject(layoutObject)
    , m_clipType(clipType)
{
    IntRect snappedClipRect = pixelSnappedIntRect(clipRect.rect());
    OwnPtr<ClipDisplayItem> clipDisplayItem = ClipDisplayItem::create(layoutObject, clipType, snappedClipRect);
    if (localPaintingInfo && clipRect.hasRadius())
        collectRoundedRectClips(*layoutObject.layer(), *localPaintingInfo, graphicsContext, fragmentOffset, paintFlags, rule, clipDisplayItem->roundedRectClips());
    if (!RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        clipDisplayItem->replay(graphicsContext);
    } else {
        ASSERT(m_graphicsContext.displayItemList());
        m_graphicsContext.displayItemList()->add(clipDisplayItem.release());
    }
}

static bool inContainingBlockChain(DeprecatedPaintLayer* startLayer, DeprecatedPaintLayer* endLayer)
{
    if (startLayer == endLayer)
        return true;

    LayoutView* view = startLayer->layoutObject()->view();
    for (LayoutBlock* currentBlock = startLayer->layoutObject()->containingBlock(); currentBlock && currentBlock != view; currentBlock = currentBlock->containingBlock()) {
        if (currentBlock->layer() == endLayer)
            return true;
    }

    return false;
}

void LayerClipRecorder::collectRoundedRectClips(DeprecatedPaintLayer& renderLayer, const DeprecatedPaintLayerPaintingInfo& localPaintingInfo, GraphicsContext& context, const LayoutPoint& fragmentOffset, PaintLayerFlags paintFlags,
    BorderRadiusClippingRule rule, Vector<FloatRoundedRect>& roundedRectClips)
{
    // If the clip rect has been tainted by a border radius, then we have to walk up our layer chain applying the clips from
    // any layers with overflow. The condition for being able to apply these clips is that the overflow object be in our
    // containing block chain so we check that also.
    for (DeprecatedPaintLayer* layer = rule == IncludeSelfForBorderRadius ? &renderLayer : renderLayer.parent(); layer; layer = layer->parent()) {
        // Composited scrolling layers handle border-radius clip in the compositor via a mask layer. We do not
        // want to apply a border-radius clip to the layer contents itself, because that would require re-rastering
        // every frame to update the clip. We only want to make sure that the mask layer is properly clipped so
        // that it can in turn clip the scrolled contents in the compositor.
        if (layer->needsCompositedScrolling() && !(paintFlags & PaintLayerPaintingChildClippingMaskPhase))
            break;

        if (layer->layoutObject()->hasOverflowClip() && layer->layoutObject()->style()->hasBorderRadius() && inContainingBlockChain(&renderLayer, layer)) {
            LayoutPoint delta(fragmentOffset);
            layer->convertToLayerCoords(localPaintingInfo.rootLayer, delta);
            roundedRectClips.append(layer->layoutObject()->style()->getRoundedInnerBorderFor(LayoutRect(delta, LayoutSize(layer->size()))));
        }

        if (layer == localPaintingInfo.rootLayer)
            break;
    }
}

LayerClipRecorder::~LayerClipRecorder()
{
    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        DisplayItem::Type endType = DisplayItem::clipTypeToEndClipType(m_clipType);
        OwnPtr<EndClipDisplayItem> endClip = EndClipDisplayItem::create(m_layoutObject, endType);
        ASSERT(m_graphicsContext.displayItemList());
        m_graphicsContext.displayItemList()->add(endClip.release());
    } else {
        m_graphicsContext.restore();
    }
}

} // namespace blink
