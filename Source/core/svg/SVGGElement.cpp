/*
 * Copyright (C) 2004, 2005, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
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
 */

#include "config.h"
#include "core/svg/SVGGElement.h"

#include "core/SVGNames.h"
#include "core/layout/svg/LayoutSVGHiddenContainer.h"
#include "core/layout/svg/LayoutSVGTransformableContainer.h"

namespace blink {

inline SVGGElement::SVGGElement(Document& document, ConstructionType constructionType)
    : SVGGraphicsElement(SVGNames::gTag, document, constructionType)
{
}

DEFINE_NODE_FACTORY(SVGGElement)

LayoutObject* SVGGElement::createLayoutObject(const ComputedStyle& style)
{
    // SVG 1.1 testsuite explicitely uses constructs like <g display="none"><linearGradient>
    // We still have to create renderers for the <g> & <linearGradient> element, though the
    // subtree may be hidden - we only want the resource renderers to exist so they can be
    // referenced from somewhere else.
    if (style.display() == NONE)
        return new LayoutSVGHiddenContainer(this);

    return new LayoutSVGTransformableContainer(this);
}

bool SVGGElement::layoutObjectIsNeeded(const ComputedStyle&)
{
    // Unlike SVGElement::layoutObjectIsNeeded(), we still create renderers, even if
    // display is set to 'none' - which is special to SVG <g> container elements.
    return parentOrShadowHostElement() && parentOrShadowHostElement()->isSVGElement();
}

}
