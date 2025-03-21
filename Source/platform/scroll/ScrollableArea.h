/*
 * Copyright (C) 2008, 2011 Apple Inc. All Rights Reserved.
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

#ifndef ScrollableArea_h
#define ScrollableArea_h

#include "platform/PlatformExport.h"
#include "platform/geometry/DoublePoint.h"
#include "platform/scroll/ScrollAnimator.h"
#include "platform/scroll/ScrollTypes.h"
#include "platform/scroll/Scrollbar.h"
#include "wtf/Noncopyable.h"
#include "wtf/Vector.h"

namespace blink {

class FloatPoint;
class GraphicsLayer;
class HostWindow;
class PlatformWheelEvent;
class ProgrammaticScrollAnimator;
class ScrollAnimator;

enum ScrollBehavior {
    ScrollBehaviorAuto,
    ScrollBehaviorInstant,
    ScrollBehaviorSmooth,
};

enum IncludeScrollbarsInRect {
    ExcludeScrollbars,
    IncludeScrollbars,
};

class PLATFORM_EXPORT ScrollableArea {
    WTF_MAKE_NONCOPYABLE(ScrollableArea);
public:
    static int pixelsPerLineStep();
    static float minFractionToStepWhenPaging();
    static int maxOverlapBetweenPages();

    // The window that hosts the ScrollableArea. The ScrollableArea will communicate scrolls and repaints to the
    // host window in the window's coordinate space.
    virtual HostWindow* hostWindow() const { return 0; };

    bool scroll(ScrollDirection, ScrollGranularity, float delta = 1);
    virtual void setScrollPosition(const DoublePoint&, ScrollBehavior = ScrollBehaviorInstant);
    void scrollToOffsetWithoutAnimation(const FloatPoint&, bool cancelProgrammaticAnimations = true);
    void scrollToOffsetWithoutAnimation(ScrollbarOrientation, float offset);

    void programmaticallyScrollSmoothlyToOffset(const FloatPoint&);

    // Should be called when the scroll position changes externally, for example if the scroll layer position
    // is updated on the scrolling thread and we need to notify the main thread.
    void notifyScrollPositionChanged(const DoublePoint&);

    static bool scrollBehaviorFromString(const String&, ScrollBehavior&);

    ScrollResult handleWheelEvent(const PlatformWheelEvent&);

    // Functions for controlling if you can scroll past the end of the document.
    bool constrainsScrollingToContentEdge() const { return m_constrainsScrollingToContentEdge; }
    void setConstrainsScrollingToContentEdge(bool constrainsScrollingToContentEdge) { m_constrainsScrollingToContentEdge = constrainsScrollingToContentEdge; }

    bool inLiveResize() const { return m_inLiveResize; }
    void willStartLiveResize();
    void willEndLiveResize();

    void contentAreaWillPaint() const;
    void mouseEnteredContentArea() const;
    void mouseExitedContentArea() const;
    void mouseMovedInContentArea() const;
    void mouseEnteredScrollbar(Scrollbar*) const;
    void mouseExitedScrollbar(Scrollbar*) const;
    void contentAreaDidShow() const;
    void contentAreaDidHide() const;

    void finishCurrentScrollAnimations() const;

    virtual void didAddScrollbar(Scrollbar*, ScrollbarOrientation);
    virtual void willRemoveScrollbar(Scrollbar*, ScrollbarOrientation);

    virtual void contentsResized();

    bool hasOverlayScrollbars() const;
    void setScrollbarOverlayStyle(ScrollbarOverlayStyle);
    ScrollbarOverlayStyle scrollbarOverlayStyle() const { return static_cast<ScrollbarOverlayStyle>(m_scrollbarOverlayStyle); }

    // This getter will create a ScrollAnimator if it doesn't already exist.
    ScrollAnimator* scrollAnimator() const;

    // This getter will return null if the ScrollAnimator hasn't been created yet.
    ScrollAnimator* existingScrollAnimator() const { return m_animators ? m_animators->scrollAnimator.get() : 0; }

    ProgrammaticScrollAnimator* programmaticScrollAnimator() const;
    ProgrammaticScrollAnimator* existingProgrammaticScrollAnimator() const
    {
        return m_animators ? m_animators->programmaticScrollAnimator.get() : 0;
    }

    const IntPoint& scrollOrigin() const { return m_scrollOrigin; }
    bool scrollOriginChanged() const { return m_scrollOriginChanged; }


    // This is used to determine whether the incoming fractional scroll offset should
    // be truncated to integer. Current rule is that if preferCompositingToLCDTextEnabled()
    // is disabled (which is true on low-dpi device by default) we should do the truncation.
    // The justification is that non-composited elements using fractional scroll offsets
    // is causing too much nasty bugs but does not add too benefit on low-dpi devices.
    virtual bool shouldUseIntegerScrollOffset() const { return false; }

    // FIXME(bokan): Meaningless name, rename to isActiveFocus
    virtual bool isActive() const = 0;
    virtual int scrollSize(ScrollbarOrientation) const = 0;
    virtual void invalidateScrollbar(Scrollbar*, const IntRect&);
    virtual bool isScrollCornerVisible() const = 0;
    virtual IntRect scrollCornerRect() const = 0;
    virtual void invalidateScrollCorner(const IntRect&);
    virtual void getTickmarks(Vector<IntRect>&) const { }

    // Convert points and rects between the scrollbar and its containing view.
    // The client needs to implement these in order to be aware of layout effects
    // like CSS transforms.
    virtual IntRect convertFromScrollbarToContainingView(const Scrollbar* scrollbar, const IntRect& scrollbarRect) const
    {
        return scrollbar->Widget::convertToContainingView(scrollbarRect);
    }
    virtual IntRect convertFromContainingViewToScrollbar(const Scrollbar* scrollbar, const IntRect& parentRect) const
    {
        return scrollbar->Widget::convertFromContainingView(parentRect);
    }
    virtual IntPoint convertFromScrollbarToContainingView(const Scrollbar* scrollbar, const IntPoint& scrollbarPoint) const
    {
        return scrollbar->Widget::convertToContainingView(scrollbarPoint);
    }
    virtual IntPoint convertFromContainingViewToScrollbar(const Scrollbar* scrollbar, const IntPoint& parentPoint) const
    {
        return scrollbar->Widget::convertFromContainingView(parentPoint);
    }

    virtual Scrollbar* horizontalScrollbar() const { return 0; }
    virtual Scrollbar* verticalScrollbar() const { return 0; }

    // scrollPosition is relative to the scrollOrigin. i.e. If the page is RTL
    // then scrollPosition will be negative. By default, scrollPositionDouble()
    // just call into scrollPosition(). Subclass can override scrollPositionDouble()
    // to return floating point precision scrolloffset.
    // FIXME: Remove scrollPosition(). crbug.com/414283.
    virtual IntPoint scrollPosition() const = 0;
    virtual DoublePoint scrollPositionDouble() const { return DoublePoint(scrollPosition()); }
    virtual IntPoint minimumScrollPosition() const = 0;
    virtual DoublePoint minimumScrollPositionDouble() const { return DoublePoint(minimumScrollPosition()); }
    virtual IntPoint maximumScrollPosition() const = 0;
    virtual DoublePoint maximumScrollPositionDouble() const { return DoublePoint(maximumScrollPosition()); }

    virtual IntRect visibleContentRect(IncludeScrollbarsInRect = ExcludeScrollbars) const;
    virtual int visibleHeight() const { return visibleContentRect().height(); }
    virtual int visibleWidth() const { return visibleContentRect().width(); }
    virtual IntSize contentsSize() const = 0;
    virtual IntSize overhangAmount() const { return IntSize(); }
    virtual IntPoint lastKnownMousePosition() const { return IntPoint(); }

    virtual bool shouldSuspendScrollAnimations() const { return true; }
    virtual void scrollbarStyleChanged() { }

    virtual bool scrollbarsCanBeActive() const = 0;

    // Returns the bounding box of this scrollable area, in the coordinate system of the enclosing scroll view.
    virtual IntRect scrollableAreaBoundingBox() const = 0;

    virtual bool scrollAnimatorEnabled() const { return false; }

    // NOTE: Only called from Internals for testing.
    void setScrollOffsetFromInternals(const IntPoint&);

    IntPoint clampScrollPosition(const IntPoint&) const;

    // Let subclasses provide a way of asking for and servicing scroll
    // animations.
    bool scheduleAnimation();
    void serviceScrollAnimations(double monotonicTime);
    void updateCompositorScrollAnimations();
    virtual void registerForAnimation() { }
    virtual void deregisterForAnimation() { }

    void notifyCompositorAnimationFinished(int groupId);

    virtual bool usesCompositedScrolling() const { return false; }

    // Returns true if the GraphicsLayer tree needs to be rebuilt.
    virtual bool updateAfterCompositingChange() { return false; }

    virtual bool userInputScrollable(ScrollbarOrientation) const = 0;
    virtual bool shouldPlaceVerticalScrollbarOnLeft() const = 0;

    // Convenience functions
    int scrollPosition(ScrollbarOrientation orientation) { return orientation == HorizontalScrollbar ? scrollPosition().x() : scrollPosition().y(); }
    int minimumScrollPosition(ScrollbarOrientation orientation) { return orientation == HorizontalScrollbar ? minimumScrollPosition().x() : minimumScrollPosition().y(); }
    int maximumScrollPosition(ScrollbarOrientation orientation) { return orientation == HorizontalScrollbar ? maximumScrollPosition().x() : maximumScrollPosition().y(); }
    int clampScrollPosition(ScrollbarOrientation orientation, int pos)  { return std::max(std::min(pos, maximumScrollPosition(orientation)), minimumScrollPosition(orientation)); }

    bool hasVerticalBarDamage() const { return !m_verticalBarDamage.isEmpty(); }
    bool hasHorizontalBarDamage() const { return !m_horizontalBarDamage.isEmpty(); }
    const IntRect& verticalBarDamage() const { return m_verticalBarDamage; }
    const IntRect& horizontalBarDamage() const { return m_horizontalBarDamage; }

    void addScrollbarDamage(Scrollbar* scrollbar, const IntRect& rect)
    {
        if (scrollbar == horizontalScrollbar())
            m_horizontalBarDamage.unite(rect);
        else
            m_verticalBarDamage.unite(rect);
    }

    void resetScrollbarDamage()
    {
        m_verticalBarDamage = IntRect();
        m_horizontalBarDamage = IntRect();
    }

    virtual GraphicsLayer* layerForContainer() const;
    virtual GraphicsLayer* layerForScrolling() const { return 0; }
    virtual GraphicsLayer* layerForHorizontalScrollbar() const { return 0; }
    virtual GraphicsLayer* layerForVerticalScrollbar() const { return 0; }
    virtual GraphicsLayer* layerForScrollCorner() const { return 0; }
    bool hasLayerForHorizontalScrollbar() const;
    bool hasLayerForVerticalScrollbar() const;
    bool hasLayerForScrollCorner() const;

    void layerForScrollingDidChange();

    void cancelProgrammaticScrollAnimation();

    DisplayItemClient displayItemClient() const { return toDisplayItemClient(this); }
    virtual String debugName() const { return "ScrollableArea"; }

protected:
    ScrollableArea();
    virtual ~ScrollableArea();

    void setScrollOrigin(const IntPoint&);
    void resetScrollOriginChanged() { m_scrollOriginChanged = false; }

    virtual void invalidateScrollbarRect(Scrollbar*, const IntRect&) = 0;
    virtual void invalidateScrollCornerRect(const IntRect&) = 0;

private:
    void scrollPositionChanged(const DoublePoint&);

    // NOTE: Only called from the ScrollAnimator.
    friend class ScrollAnimator;
    void setScrollOffsetFromAnimation(const DoublePoint&);

    // This function should be overriden by subclasses to perform the actual
    // scroll of the content. By default the DoublePoint version will just
    // call into the IntPoint version. If fractional scroll is needed, one
    // can override the DoublePoint version to take advantage of the double
    // precision scroll offset.
    // FIXME: Remove the IntPoint version. And change the function to
    // take DoubleSize. crbug.com/414283.
    virtual void setScrollOffset(const IntPoint&) = 0;
    virtual void setScrollOffset(const DoublePoint& offset)
    {
        setScrollOffset(flooredIntPoint(offset));
    }

    virtual int lineStep(ScrollbarOrientation) const;
    virtual int pageStep(ScrollbarOrientation) const;
    virtual int documentStep(ScrollbarOrientation) const;
    virtual float pixelStep(ScrollbarOrientation) const;

    // Stores the paint invalidations for the scrollbars during layout.
    IntRect m_horizontalBarDamage;
    IntRect m_verticalBarDamage;

    struct ScrollableAreaAnimators {
        RefPtr<ScrollAnimator> scrollAnimator;
        OwnPtr<ProgrammaticScrollAnimator> programmaticScrollAnimator;
    };

    mutable OwnPtr<ScrollableAreaAnimators> m_animators;
    unsigned m_constrainsScrollingToContentEdge : 1;

    unsigned m_inLiveResize : 1;

    unsigned m_scrollbarOverlayStyle : 2; // ScrollbarOverlayStyle

    unsigned m_scrollOriginChanged : 1;

    // There are 8 possible combinations of writing mode and direction. Scroll origin will be non-zero in the x or y axis
    // if there is any reversed direction or writing-mode. The combinations are:
    // writing-mode / direction     scrollOrigin.x() set    scrollOrigin.y() set
    // horizontal-tb / ltr          NO                      NO
    // horizontal-tb / rtl          YES                     NO
    // horizontal-bt / ltr          NO                      YES
    // horizontal-bt / rtl          YES                     YES
    // vertical-lr / ltr            NO                      NO
    // vertical-lr / rtl            NO                      YES
    // vertical-rl / ltr            YES                     NO
    // vertical-rl / rtl            YES                     YES
    IntPoint m_scrollOrigin;
};

} // namespace blink

#endif // ScrollableArea_h
