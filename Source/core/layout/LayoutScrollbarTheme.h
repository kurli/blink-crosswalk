/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LayoutScrollbarTheme_h
#define LayoutScrollbarTheme_h

#include "platform/scroll/ScrollbarTheme.h"

namespace blink {

class PlatformMouseEvent;

class LayoutScrollbarTheme final : public ScrollbarTheme {
public:
    virtual ~LayoutScrollbarTheme() { }

    virtual int scrollbarThickness(ScrollbarControlSize controlSize) override { return ScrollbarTheme::theme()->scrollbarThickness(controlSize); }

    virtual ScrollbarButtonsPlacement buttonsPlacement() const override { return ScrollbarTheme::theme()->buttonsPlacement(); }

    virtual bool paint(ScrollbarThemeClient*, GraphicsContext*, const IntRect& damageRect);
    virtual void paintScrollCorner(GraphicsContext*, const DisplayItemClientWrapper&, const IntRect& cornerRect) override;

    virtual bool shouldCenterOnThumb(ScrollbarThemeClient* scrollbar, const PlatformMouseEvent& event) override { return ScrollbarTheme::theme()->shouldCenterOnThumb(scrollbar, event); }
    virtual bool shouldSnapBackToDragOrigin(ScrollbarThemeClient* scrollbar, const PlatformMouseEvent& event) override { return ScrollbarTheme::theme()->shouldSnapBackToDragOrigin(scrollbar, event); }

    virtual double initialAutoscrollTimerDelay() override { return ScrollbarTheme::theme()->initialAutoscrollTimerDelay(); }
    virtual double autoscrollTimerDelay() override { return ScrollbarTheme::theme()->autoscrollTimerDelay(); }

    virtual void registerScrollbar(ScrollbarThemeClient* scrollbar) override { return ScrollbarTheme::theme()->registerScrollbar(scrollbar); }
    virtual void unregisterScrollbar(ScrollbarThemeClient* scrollbar) override { return ScrollbarTheme::theme()->unregisterScrollbar(scrollbar); }

    virtual int minimumThumbLength(ScrollbarThemeClient*) override;

    void buttonSizesAlongTrackAxis(ScrollbarThemeClient*, int& beforeSize, int& afterSize);

    static LayoutScrollbarTheme* renderScrollbarTheme();

protected:
    virtual bool hasButtons(ScrollbarThemeClient*) override;
    virtual bool hasThumb(ScrollbarThemeClient*) override;

    virtual IntRect backButtonRect(ScrollbarThemeClient*, ScrollbarPart, bool painting = false) override;
    virtual IntRect forwardButtonRect(ScrollbarThemeClient*, ScrollbarPart, bool painting = false) override;
    virtual IntRect trackRect(ScrollbarThemeClient*, bool painting = false) override;

    virtual void paintScrollbarBackground(GraphicsContext*, ScrollbarThemeClient*) override;
    virtual void paintTrackBackground(GraphicsContext*, ScrollbarThemeClient*, const IntRect&) override;
    virtual void paintTrackPiece(GraphicsContext*, ScrollbarThemeClient*, const IntRect&, ScrollbarPart) override;
    virtual void paintButton(GraphicsContext*, ScrollbarThemeClient*, const IntRect&, ScrollbarPart) override;
    virtual void paintThumb(GraphicsContext*, ScrollbarThemeClient*, const IntRect&) override;
    virtual void paintTickmarks(GraphicsContext*, ScrollbarThemeClient*, const IntRect&) override;

    virtual IntRect constrainTrackRectToTrackPieces(ScrollbarThemeClient*, const IntRect&) override;
};

} // namespace blink

#endif // LayoutScrollbarTheme_h
