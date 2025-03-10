// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/TransformRecorder.h"

#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include "platform/graphics/paint/TransformDisplayItem.h"

namespace blink {

TransformRecorder::TransformRecorder(GraphicsContext& context, const DisplayItemClientWrapper& client, const AffineTransform& transform)
    : m_context(context)
    , m_client(client)
{
    m_skipRecordingForIdentityTransform = transform.isIdentity();

    if (m_skipRecordingForIdentityTransform)
        return;

    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        ASSERT(m_context.displayItemList());
        m_context.displayItemList()->add(BeginTransformDisplayItem::create(m_client, transform));
    } else {
        BeginTransformDisplayItem beginTransform(m_client, transform);
        beginTransform.replay(m_context);
    }
}

TransformRecorder::~TransformRecorder()
{
    if (m_skipRecordingForIdentityTransform)
        return;

    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        ASSERT(m_context.displayItemList());
        m_context.displayItemList()->add(EndTransformDisplayItem::create(m_client));
    } else {
        EndTransformDisplayItem endTransform(m_client);
        endTransform.replay(m_context);
    }
}

} // namespace blink
