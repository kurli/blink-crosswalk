/*
 * Copyright (C) 1999 Lars Knoll <knoll@kde.org>
 * Copyright (C) 1999 Antti Koivisto <koivisto@kde.org>
 * Copyright (C) 2000 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 Allan Sandfeld Jensen <kde@carewolf.com>
 * Copyright (C) 2006 Samuel Weinig <sam.weinig@gmail.com>
 * Copyright (C) 2003, 2004, 2005, 2006, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) 2010 Patrick Gansterer <paroga@paroga.com>
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

#include "config.h"
#include "core/layout/LayoutImageResource.h"

#include "core/layout/LayoutImage.h"

namespace blink {

LayoutImageResource::LayoutImageResource()
    : m_renderer(0)
    , m_cachedImage(0)
{
}

LayoutImageResource::~LayoutImageResource()
{
}

void LayoutImageResource::initialize(LayoutObject* renderer)
{
    ASSERT(!m_renderer);
    ASSERT(renderer);
    m_renderer = renderer;
}

void LayoutImageResource::shutdown()
{
    ASSERT(m_renderer);

    if (m_cachedImage)
        m_cachedImage->removeClient(m_renderer);
}

void LayoutImageResource::setImageResource(ImageResource* newImage)
{
    ASSERT(m_renderer);

    if (m_cachedImage == newImage)
        return;

    if (m_cachedImage)
        m_cachedImage->removeClient(m_renderer);
    m_cachedImage = newImage;
    if (m_cachedImage) {
        m_cachedImage->addClient(m_renderer);
        if (m_cachedImage->errorOccurred())
            m_renderer->imageChanged(m_cachedImage.get());
    } else {
        m_renderer->imageChanged(m_cachedImage.get());
    }
}

void LayoutImageResource::resetAnimation()
{
    ASSERT(m_renderer);

    if (!m_cachedImage)
        return;

    image()->resetAnimation();

    m_renderer->setShouldDoFullPaintInvalidation();
}

void LayoutImageResource::setContainerSizeForLayoutObject(const IntSize& imageContainerSize)
{
    ASSERT(m_renderer);
    if (m_cachedImage)
        m_cachedImage->setContainerSizeForLayoutObject(m_renderer, imageContainerSize, m_renderer->style()->effectiveZoom());
}

LayoutSize LayoutImageResource::getImageSize(float multiplier, ImageResource::SizeType type) const
{
    if (!m_cachedImage)
        return LayoutSize();
    LayoutSize size = m_cachedImage->imageSizeForLayoutObject(m_renderer, multiplier, type);
    if (m_renderer && m_renderer->isLayoutImage())
        size.scale(toLayoutImage(m_renderer)->imageDevicePixelRatio());
    return size;
}

} // namespace blink
