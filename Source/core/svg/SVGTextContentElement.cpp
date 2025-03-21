/*
 * Copyright (C) 2004, 2005, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Rob Buis <buis@kde.org>
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
#include "core/svg/SVGTextContentElement.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/CSSPropertyNames.h"
#include "core/CSSValueKeywords.h"
#include "core/SVGNames.h"
#include "core/XMLNames.h"
#include "core/editing/FrameSelection.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/UseCounter.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/svg/SVGTextQuery.h"

namespace blink {

template<> const SVGEnumerationStringEntries& getStaticStringEntries<SVGLengthAdjustType>()
{
    DEFINE_STATIC_LOCAL(SVGEnumerationStringEntries, entries, ());
    if (entries.isEmpty()) {
        entries.append(std::make_pair(SVGLengthAdjustSpacing, "spacing"));
        entries.append(std::make_pair(SVGLengthAdjustSpacingAndGlyphs, "spacingAndGlyphs"));
    }
    return entries;
}

// SVGTextContentElement's 'textLength' attribute needs special handling.
// It should return getComputedTextLength() when textLength is not specified manually.
class SVGAnimatedTextLength final : public SVGAnimatedLength {
public:
    static PassRefPtrWillBeRawPtr<SVGAnimatedTextLength> create(SVGTextContentElement* contextElement)
    {
        return adoptRefWillBeNoop(new SVGAnimatedTextLength(contextElement));
    }

    virtual SVGLengthTearOff* baseVal() override
    {
        SVGTextContentElement* textContentElement = toSVGTextContentElement(contextElement());
        if (!textContentElement->textLengthIsSpecifiedByUser())
            baseValue()->newValueSpecifiedUnits(LengthTypeNumber, textContentElement->getComputedTextLength());

        return SVGAnimatedLength::baseVal();
    }

private:
    SVGAnimatedTextLength(SVGTextContentElement* contextElement)
        : SVGAnimatedLength(contextElement, SVGNames::textLengthAttr, SVGLength::create(SVGLengthMode::Width), ForbidNegativeLengths)
    {
    }
};


SVGTextContentElement::SVGTextContentElement(const QualifiedName& tagName, Document& document)
    : SVGGraphicsElement(tagName, document)
    , m_textLength(SVGAnimatedTextLength::create(this))
    , m_textLengthIsSpecifiedByUser(false)
    , m_lengthAdjust(SVGAnimatedEnumeration<SVGLengthAdjustType>::create(this, SVGNames::lengthAdjustAttr, SVGLengthAdjustSpacing))
{
    addToPropertyMap(m_textLength);
    addToPropertyMap(m_lengthAdjust);
}

DEFINE_TRACE(SVGTextContentElement)
{
    visitor->trace(m_textLength);
    visitor->trace(m_lengthAdjust);
    SVGGraphicsElement::trace(visitor);
}

unsigned SVGTextContentElement::getNumberOfChars()
{
    document().updateLayoutIgnorePendingStylesheets();
    return SVGTextQuery(layoutObject()).numberOfCharacters();
}

float SVGTextContentElement::getComputedTextLength()
{
    document().updateLayoutIgnorePendingStylesheets();
    return SVGTextQuery(layoutObject()).textLength();
}

float SVGTextContentElement::getSubStringLength(unsigned charnum, unsigned nchars, ExceptionState& exceptionState)
{
    document().updateLayoutIgnorePendingStylesheets();

    unsigned numberOfChars = getNumberOfChars();
    if (charnum >= numberOfChars) {
        exceptionState.throwDOMException(IndexSizeError, ExceptionMessages::indexExceedsMaximumBound("charnum", charnum, getNumberOfChars()));
        return 0.0f;
    }

    if (nchars > numberOfChars - charnum)
        nchars = numberOfChars - charnum;

    return SVGTextQuery(layoutObject()).subStringLength(charnum, nchars);
}

PassRefPtrWillBeRawPtr<SVGPointTearOff> SVGTextContentElement::getStartPositionOfChar(unsigned charnum, ExceptionState& exceptionState)
{
    document().updateLayoutIgnorePendingStylesheets();

    if (charnum > getNumberOfChars()) {
        exceptionState.throwDOMException(IndexSizeError, ExceptionMessages::indexExceedsMaximumBound("charnum", charnum, getNumberOfChars()));
        return nullptr;
    }

    FloatPoint point = SVGTextQuery(layoutObject()).startPositionOfCharacter(charnum);
    return SVGPointTearOff::create(SVGPoint::create(point), 0, PropertyIsNotAnimVal);
}

PassRefPtrWillBeRawPtr<SVGPointTearOff> SVGTextContentElement::getEndPositionOfChar(unsigned charnum, ExceptionState& exceptionState)
{
    document().updateLayoutIgnorePendingStylesheets();

    if (charnum > getNumberOfChars()) {
        exceptionState.throwDOMException(IndexSizeError, ExceptionMessages::indexExceedsMaximumBound("charnum", charnum, getNumberOfChars()));
        return nullptr;
    }

    FloatPoint point = SVGTextQuery(layoutObject()).endPositionOfCharacter(charnum);
    return SVGPointTearOff::create(SVGPoint::create(point), 0, PropertyIsNotAnimVal);
}

PassRefPtrWillBeRawPtr<SVGRectTearOff> SVGTextContentElement::getExtentOfChar(unsigned charnum, ExceptionState& exceptionState)
{
    document().updateLayoutIgnorePendingStylesheets();

    if (charnum > getNumberOfChars()) {
        exceptionState.throwDOMException(IndexSizeError, ExceptionMessages::indexExceedsMaximumBound("charnum", charnum, getNumberOfChars()));
        return nullptr;
    }

    FloatRect rect = SVGTextQuery(layoutObject()).extentOfCharacter(charnum);
    return SVGRectTearOff::create(SVGRect::create(rect), 0, PropertyIsNotAnimVal);
}

float SVGTextContentElement::getRotationOfChar(unsigned charnum, ExceptionState& exceptionState)
{
    document().updateLayoutIgnorePendingStylesheets();

    if (charnum > getNumberOfChars()) {
        exceptionState.throwDOMException(IndexSizeError, ExceptionMessages::indexExceedsMaximumBound("charnum", charnum, getNumberOfChars()));
        return 0.0f;
    }

    return SVGTextQuery(layoutObject()).rotationOfCharacter(charnum);
}

int SVGTextContentElement::getCharNumAtPosition(PassRefPtrWillBeRawPtr<SVGPointTearOff> point, ExceptionState& exceptionState)
{
    document().updateLayoutIgnorePendingStylesheets();
    return SVGTextQuery(layoutObject()).characterNumberAtPosition(point->target()->value());
}

void SVGTextContentElement::selectSubString(unsigned charnum, unsigned nchars, ExceptionState& exceptionState)
{
    unsigned numberOfChars = getNumberOfChars();
    if (charnum >= numberOfChars) {
        exceptionState.throwDOMException(IndexSizeError, ExceptionMessages::indexExceedsMaximumBound("charnum", charnum, getNumberOfChars()));
        return;
    }

    if (nchars > numberOfChars - charnum)
        nchars = numberOfChars - charnum;

    ASSERT(document().frame());

    // Find selection start
    VisiblePosition start(firstPositionInNode(const_cast<SVGTextContentElement*>(this)));
    for (unsigned i = 0; i < charnum; ++i)
        start = start.next();

    // Find selection end
    VisiblePosition end(start);
    for (unsigned i = 0; i < nchars; ++i)
        end = end.next();

    document().frame()->selection().setSelection(VisibleSelection(start, end));
}

bool SVGTextContentElement::isSupportedAttribute(const QualifiedName& attrName)
{
    DEFINE_STATIC_LOCAL(HashSet<QualifiedName>, supportedAttributes, ());
    if (supportedAttributes.isEmpty()) {
        supportedAttributes.add(SVGNames::lengthAdjustAttr);
        supportedAttributes.add(SVGNames::textLengthAttr);
        supportedAttributes.add(XMLNames::spaceAttr);
    }
    return supportedAttributes.contains<SVGAttributeHashTranslator>(attrName);
}

bool SVGTextContentElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name.matches(XMLNames::spaceAttr))
        return true;
    return SVGGraphicsElement::isPresentationAttribute(name);
}

void SVGTextContentElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (!isSupportedAttribute(name))
        SVGGraphicsElement::collectStyleForPresentationAttribute(name, value, style);
    else if (name.matches(XMLNames::spaceAttr)) {
        DEFINE_STATIC_LOCAL(const AtomicString, preserveString, ("preserve", AtomicString::ConstructFromLiteral));

        if (value == preserveString) {
            UseCounter::count(document(), UseCounter::WhiteSpacePreFromXMLSpace);
            addPropertyToPresentationAttributeStyle(style, CSSPropertyWhiteSpace, CSSValuePre);
        } else {
            UseCounter::count(document(), UseCounter::WhiteSpaceNowrapFromXMLSpace);
            addPropertyToPresentationAttributeStyle(style, CSSPropertyWhiteSpace, CSSValueNowrap);
        }
    }
}

void SVGTextContentElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (!isSupportedAttribute(attrName)) {
        SVGGraphicsElement::svgAttributeChanged(attrName);
        return;
    }

    if (attrName == SVGNames::textLengthAttr)
        m_textLengthIsSpecifiedByUser = true;

    SVGElement::InvalidationGuard invalidationGuard(this);

    if (LayoutObject* renderer = this->layoutObject())
        markForLayoutAndParentResourceInvalidation(renderer);
}

bool SVGTextContentElement::selfHasRelativeLengths() const
{
    // Any element of the <text> subtree is advertized as using relative lengths.
    // On any window size change, we have to relayout the text subtree, as the
    // effective 'on-screen' font size may change.
    return true;
}

SVGTextContentElement* SVGTextContentElement::elementFromRenderer(LayoutObject* renderer)
{
    if (!renderer)
        return 0;

    if (!renderer->isSVGText() && !renderer->isSVGInline())
        return 0;

    SVGElement* element = toSVGElement(renderer->node());
    ASSERT(element);
    return isSVGTextContentElement(*element) ? toSVGTextContentElement(element) : 0;
}

}
