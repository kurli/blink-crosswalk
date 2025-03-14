// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/TextPainter.h"

#include "core/CSSPropertyNames.h"
#include "core/frame/Settings.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutTextCombine.h"
#include "core/layout/line/InlineTextBox.h"
#include "core/style/ComputedStyle.h"
#include "core/style/ShadowList.h"
#include "platform/fonts/Font.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/GraphicsContextStateSaver.h"
#include "platform/text/TextRun.h"
#include "wtf/Assertions.h"
#include "wtf/unicode/CharacterNames.h"

namespace blink {

TextPainter::TextPainter(GraphicsContext* context, const Font& font, const TextRun& run, const FloatPoint& textOrigin, const FloatRect& textBounds, bool horizontal)
    : m_graphicsContext(context)
    , m_font(font)
    , m_run(run)
    , m_textOrigin(textOrigin)
    , m_textBounds(textBounds)
    , m_horizontal(horizontal)
    , m_emphasisMarkOffset(0)
    , m_combinedText(0)
{
}

TextPainter::~TextPainter()
{
}

void TextPainter::setEmphasisMark(const AtomicString& emphasisMark, TextEmphasisPosition position)
{
    m_emphasisMark = emphasisMark;

    if (emphasisMark.isNull()) {
        m_emphasisMarkOffset = 0;
    } else if (position == TextEmphasisPositionOver) {
        m_emphasisMarkOffset = -m_font.fontMetrics().ascent() - m_font.emphasisMarkDescent(emphasisMark);
    } else {
        ASSERT(position == TextEmphasisPositionUnder);
        m_emphasisMarkOffset = m_font.fontMetrics().descent() + m_font.emphasisMarkAscent(emphasisMark);
    }
}

void TextPainter::paint(int startOffset, int endOffset, int length, const Style& textStyle, TextBlobPtr* cachedTextBlob)
{
    GraphicsContextStateSaver stateSaver(*m_graphicsContext, false);
    updateGraphicsContext(textStyle, stateSaver);
    if (m_combinedText) {
        m_graphicsContext->save();
        m_combinedText->transformToInlineCoordinates(*m_graphicsContext, m_textBounds);
        paintInternal<PaintText>(startOffset, endOffset, length, cachedTextBlob);
        m_graphicsContext->restore();
    } else {
        paintInternal<PaintText>(startOffset, endOffset, length, cachedTextBlob);
    }

    if (!m_emphasisMark.isEmpty()) {
        if (textStyle.emphasisMarkColor != textStyle.fillColor)
            m_graphicsContext->setFillColor(textStyle.emphasisMarkColor);

        if (m_combinedText)
            paintEmphasisMarkForCombinedText();
        else
            paintInternal<PaintEmphasisMark>(startOffset, endOffset, length);
    }
}

// static
void TextPainter::updateGraphicsContext(GraphicsContext* context, const Style& textStyle, bool horizontal, GraphicsContextStateSaver& stateSaver)
{
    TextDrawingModeFlags mode = context->textDrawingMode();
    if (textStyle.strokeWidth > 0) {
        TextDrawingModeFlags newMode = mode | TextModeStroke;
        if (mode != newMode) {
            if (!stateSaver.saved())
                stateSaver.save();
            context->setTextDrawingMode(newMode);
            mode = newMode;
        }
    }

    if (mode & TextModeFill && textStyle.fillColor != context->fillColor())
        context->setFillColor(textStyle.fillColor);

    if (mode & TextModeStroke) {
        if (textStyle.strokeColor != context->strokeColor())
            context->setStrokeColor(textStyle.strokeColor);
        if (textStyle.strokeWidth != context->strokeThickness())
            context->setStrokeThickness(textStyle.strokeWidth);
    }

    // Text shadows are disabled when printing. http://crbug.com/258321
    if (textStyle.shadow && !context->printing()) {
        if (!stateSaver.saved())
            stateSaver.save();
        context->setDrawLooper(textStyle.shadow->createDrawLooper(DrawLooperBuilder::ShadowIgnoresAlpha, horizontal));
    }
}

static Color textColorForWhiteBackground(Color textColor)
{
    int distanceFromWhite = differenceSquared(textColor, Color::white);
    // semi-arbitrarily chose 65025 (255^2) value here after a few tests;
    return distanceFromWhite > 65025 ? textColor : textColor.dark();
}

// static
TextPainter::Style TextPainter::textPaintingStyle(LayoutObject& renderer, const ComputedStyle& style, bool forceBlackText, bool isPrinting)
{
    TextPainter::Style textStyle;

    if (forceBlackText) {
        textStyle.fillColor = Color::black;
        textStyle.strokeColor = Color::black;
        textStyle.emphasisMarkColor = Color::black;
        textStyle.strokeWidth = style.textStrokeWidth();
        textStyle.shadow = 0;
    } else {
        textStyle.fillColor = renderer.resolveColor(style, CSSPropertyWebkitTextFillColor);
        textStyle.strokeColor = renderer.resolveColor(style, CSSPropertyWebkitTextStrokeColor);
        textStyle.emphasisMarkColor = renderer.resolveColor(style, CSSPropertyWebkitTextEmphasisColor);
        textStyle.strokeWidth = style.textStrokeWidth();
        textStyle.shadow = style.textShadow();

        // Adjust text color when printing with a white background.
        bool forceBackgroundToWhite = false;
        if (isPrinting) {
            if (style.printColorAdjust() == PrintColorAdjustEconomy)
                forceBackgroundToWhite = true;
            if (renderer.document().settings() && renderer.document().settings()->shouldPrintBackgrounds())
                forceBackgroundToWhite = false;
        }
        if (forceBackgroundToWhite) {
            textStyle.fillColor = textColorForWhiteBackground(textStyle.fillColor);
            textStyle.strokeColor = textColorForWhiteBackground(textStyle.strokeColor);
            textStyle.emphasisMarkColor = textColorForWhiteBackground(textStyle.emphasisMarkColor);
        }

        // Text shadows are disabled when printing. http://crbug.com/258321
        if (isPrinting)
            textStyle.shadow = 0;
    }

    return textStyle;
}

TextPainter::Style TextPainter::selectionPaintingStyle(LayoutObject& renderer, bool haveSelection, bool forceBlackText, bool isPrinting, const TextPainter::Style& textStyle)
{
    TextPainter::Style selectionStyle = textStyle;

    if (haveSelection) {
        if (!forceBlackText) {
            selectionStyle.fillColor = renderer.selectionForegroundColor();
            selectionStyle.emphasisMarkColor = renderer.selectionEmphasisMarkColor();
        }

        if (const ComputedStyle* pseudoStyle = renderer.getCachedPseudoStyle(SELECTION)) {
            selectionStyle.strokeColor = forceBlackText ? Color::black : renderer.resolveColor(*pseudoStyle, CSSPropertyWebkitTextStrokeColor);
            selectionStyle.strokeWidth = pseudoStyle->textStrokeWidth();
            selectionStyle.shadow = forceBlackText ? 0 : pseudoStyle->textShadow();
        }

        // Text shadows are disabled when printing. http://crbug.com/258321
        if (isPrinting)
            selectionStyle.shadow = 0;
    }

    return selectionStyle;
}

template <TextPainter::PaintInternalStep step>
void TextPainter::paintInternalRun(TextRunPaintInfo& textRunPaintInfo, int from, int to)
{
    ASSERT(from <= textRunPaintInfo.run.length());
    ASSERT(to <= textRunPaintInfo.run.length());

    textRunPaintInfo.from = from;
    textRunPaintInfo.to = to;

    if (step == PaintEmphasisMark) {
        m_graphicsContext->drawEmphasisMarks(m_font, textRunPaintInfo, m_emphasisMark,
            m_textOrigin + IntSize(0, m_emphasisMarkOffset));
    } else {
        ASSERT(step == PaintText);
        m_graphicsContext->drawText(m_font, textRunPaintInfo, m_textOrigin);
    }
}

template <TextPainter::PaintInternalStep Step>
void TextPainter::paintInternal(int startOffset, int endOffset, int truncationPoint, TextBlobPtr* cachedTextBlob)
{
    TextRunPaintInfo textRunPaintInfo(m_run);
    textRunPaintInfo.bounds = m_textBounds;
    if (startOffset <= endOffset) {
        // FIXME: We should be able to use cachedTextBlob in more cases.
        textRunPaintInfo.cachedTextBlob = cachedTextBlob;
        paintInternalRun<Step>(textRunPaintInfo, startOffset, endOffset);
    } else {
        if (endOffset > 0)
            paintInternalRun<Step>(textRunPaintInfo, 0, endOffset);
        if (startOffset < truncationPoint)
            paintInternalRun<Step>(textRunPaintInfo, startOffset, truncationPoint);
    }
}

void TextPainter::paintEmphasisMarkForCombinedText()
{
    ASSERT(m_combinedText);
    DEFINE_STATIC_LOCAL(TextRun, placeholderTextRun, (&ideographicFullStop, 1));
    FloatPoint emphasisMarkTextOrigin(m_textBounds.x(), m_textBounds.y() + m_font.fontMetrics().ascent() + m_emphasisMarkOffset);
    TextRunPaintInfo textRunPaintInfo(placeholderTextRun);
    textRunPaintInfo.bounds = m_textBounds;
    m_graphicsContext->concatCTM(rotation(m_textBounds, Clockwise));
    m_graphicsContext->drawEmphasisMarks(m_combinedText->originalFont(), textRunPaintInfo, m_emphasisMark, emphasisMarkTextOrigin);
    m_graphicsContext->concatCTM(rotation(m_textBounds, Counterclockwise));
}

} // namespace blink
