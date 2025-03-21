/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/accessibility/AXImageMapLink.h"

#include "modules/accessibility/AXLayoutObject.h"
#include "modules/accessibility/AXObjectCacheImpl.h"

namespace blink {

using namespace HTMLNames;

AXImageMapLink::AXImageMapLink(AXObjectCacheImpl* axObjectCache)
    : AXMockObject(axObjectCache)
    , m_areaElement(nullptr)
    , m_mapElement(nullptr)
{
}

AXImageMapLink::~AXImageMapLink()
{
}

void AXImageMapLink::detachFromParent()
{
    AXMockObject::detachFromParent();
    m_areaElement = nullptr;
    m_mapElement = nullptr;
}

PassRefPtr<AXImageMapLink> AXImageMapLink::create(AXObjectCacheImpl* axObjectCache)
{
    return adoptRef(new AXImageMapLink(axObjectCache));
}

AXObject* AXImageMapLink::computeParent() const
{
    if (m_parent)
        return m_parent;

    if (!m_mapElement.get() || !m_mapElement->layoutObject())
        return 0;

    return axObjectCache()->getOrCreate(m_mapElement->layoutObject());
}

AccessibilityRole AXImageMapLink::roleValue() const
{
    if (!m_areaElement)
        return LinkRole;

    const AtomicString& ariaRole = getAttribute(roleAttr);
    if (!ariaRole.isEmpty())
        return AXObject::ariaRoleToWebCoreRole(ariaRole);

    return LinkRole;
}

Element* AXImageMapLink::actionElement() const
{
    return anchorElement();
}

Element* AXImageMapLink::anchorElement() const
{
    return m_areaElement.get();
}

KURL AXImageMapLink::url() const
{
    if (!m_areaElement.get())
        return KURL();

    return m_areaElement->href();
}

String AXImageMapLink::accessibilityDescription() const
{
    const AtomicString& ariaLabel = getAttribute(aria_labelAttr);
    if (!ariaLabel.isEmpty())
        return ariaLabel;
    const AtomicString& alt = getAttribute(altAttr);
    if (!alt.isEmpty())
        return alt;

    return String();
}

String AXImageMapLink::title(TextUnderElementMode mode) const
{
    const AtomicString& title = getAttribute(titleAttr);
    if (!title.isEmpty())
        return title;
    const AtomicString& summary = getAttribute(summaryAttr);
    if (!summary.isEmpty())
        return summary;

    return String();
}

LayoutRect AXImageMapLink::elementRect() const
{
    if (!m_mapElement.get() || !m_areaElement.get())
        return LayoutRect();

    LayoutObject* layoutObject;
    if (m_parent && m_parent->isAXLayoutObject())
        layoutObject = toAXLayoutObject(m_parent)->layoutObject();
    else
        layoutObject = m_mapElement->layoutObject();

    if (!layoutObject)
        return LayoutRect();

    return m_areaElement->computeRect(layoutObject);
}

} // namespace blink
