// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/Transform3DRecorder.h"

#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include "platform/graphics/paint/Transform3DDisplayItem.h"

namespace blink {

Transform3DRecorder::Transform3DRecorder(GraphicsContext& context, const DisplayItemClientWrapper& client, DisplayItem::Type type, const TransformationMatrix& transform)
    : m_context(context)
    , m_client(client)
    , m_type(type)
{
    ASSERT(DisplayItem::isTransform3DType(type));
    m_skipRecordingForIdentityTransform = transform.isIdentity();

    if (m_skipRecordingForIdentityTransform)
        return;

    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        ASSERT(m_context.displayItemList());
        m_context.displayItemList()->add(BeginTransform3DDisplayItem::create(m_client, m_type, transform));
    } else {
        BeginTransform3DDisplayItem beginTransform(m_client, m_type, transform);
        beginTransform.replay(m_context);
    }
}

Transform3DRecorder::~Transform3DRecorder()
{
    if (m_skipRecordingForIdentityTransform)
        return;

    DisplayItem::Type endType = DisplayItem::transform3DTypeToEndTransform3DType(m_type);
    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        ASSERT(m_context.displayItemList());
        m_context.displayItemList()->add(EndTransform3DDisplayItem::create(m_client, endType));
    } else {
        EndTransform3DDisplayItem endTransform(m_client, endType);
        endTransform.replay(m_context);
    }
}

} // namespace blink
