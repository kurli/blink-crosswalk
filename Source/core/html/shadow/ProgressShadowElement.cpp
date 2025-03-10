/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "core/html/shadow/ProgressShadowElement.h"

#include "core/HTMLNames.h"
#include "core/html/HTMLProgressElement.h"
#include "core/layout/LayoutProgress.h"

namespace blink {

using namespace HTMLNames;

ProgressShadowElement::ProgressShadowElement(Document& document)
    : HTMLDivElement(document)
{
}

HTMLProgressElement* ProgressShadowElement::progressElement() const
{
    return toHTMLProgressElement(shadowHost());
}

bool ProgressShadowElement::layoutObjectIsNeeded(const ComputedStyle& style)
{
    LayoutObject* progressRenderer = progressElement()->layoutObject();
    return progressRenderer && !progressRenderer->style()->hasAppearance() && HTMLDivElement::layoutObjectIsNeeded(style);
}

inline ProgressInnerElement::ProgressInnerElement(Document& document)
    : ProgressShadowElement(document)
{
}

DEFINE_NODE_FACTORY(ProgressInnerElement)

LayoutObject* ProgressInnerElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutProgress(this);
}

bool ProgressInnerElement::layoutObjectIsNeeded(const ComputedStyle& style)
{
    if (progressElement()->hasOpenShadowRoot())
        return HTMLDivElement::layoutObjectIsNeeded(style);

    LayoutObject* progressRenderer = progressElement()->layoutObject();
    return progressRenderer && !progressRenderer->style()->hasAppearance() && HTMLDivElement::layoutObjectIsNeeded(style);
}

inline ProgressBarElement::ProgressBarElement(Document& document)
    : ProgressShadowElement(document)
{
}

DEFINE_NODE_FACTORY(ProgressBarElement)

ProgressValueElement::ProgressValueElement(Document& document)
    : ProgressShadowElement(document)
{
}

void ProgressValueElement::setWidthPercentage(double width)
{
    setInlineStyleProperty(CSSPropertyWidth, width, CSSPrimitiveValue::CSS_PERCENTAGE);
}

DEFINE_NODE_FACTORY(ProgressValueElement)

}
