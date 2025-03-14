/*
 * Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 *                     1999 Lars Knoll <knoll@kde.org>
 *                     1999 Antti Koivisto <koivisto@kde.org>
 *                     2000 Simon Hausmann <hausmann@kde.org>
 *                     2000 Stefan Schimanski <1Stein@gmx.de>
 *                     2001 George Staikos <staikos@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2005 Alexey Proskuryakov <ap@nypop.com>
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2008 Google Inc.
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
#include "core/frame/LocalFrame.h"

#include "bindings/core/v8/ScriptController.h"
#include "core/dom/DocumentType.h"
#include "core/editing/Editor.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/InputMethodController.h"
#include "core/editing/SpellChecker.h"
#include "core/editing/htmlediting.h"
#include "core/editing/markup.h"
#include "core/events/Event.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/frame/EventHandlerRegistry.h"
#include "core/frame/FrameConsole.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLFrameElementBase.h"
#include "core/html/HTMLPlugInElement.h"
#include "core/inspector/ConsoleMessageStorage.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InstrumentingAgents.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutView.h"
#include "core/layout/compositing/DeprecatedPaintLayerCompositor.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/page/Chrome.h"
#include "core/page/EventHandler.h"
#include "core/page/FocusController.h"
#include "core/page/Page.h"
#include "core/page/scrolling/ScrollingCoordinator.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/TransformRecorder.h"
#include "core/svg/SVGDocumentExtensions.h"
#include "platform/DragImage.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/ScriptForbiddenScope.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/ImageBuffer.h"
#include "platform/graphics/paint/ClipRecorder.h"
#include "platform/graphics/paint/DisplayItemListContextRecorder.h"
#include "platform/text/TextStream.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/StdLibExtras.h"

namespace blink {

using namespace HTMLNames;

namespace {

struct ScopedFramePaintingState {
    ScopedFramePaintingState(LocalFrame* frame, Node* node)
        : frame(frame)
        , node(node)
        , paintBehavior(frame->view()->paintBehavior())
    {
        ASSERT(!node || node->layoutObject());
        if (node)
            node->layoutObject()->updateDragState(true);
    }

    ~ScopedFramePaintingState()
    {
        if (node && node->layoutObject())
            node->layoutObject()->updateDragState(false);
        frame->view()->setPaintBehavior(paintBehavior);
        frame->view()->setNodeToDraw(0);
    }

    LocalFrame* frame;
    Node* node;
    PaintBehavior paintBehavior;
};

inline float parentPageZoomFactor(LocalFrame* frame)
{
    Frame* parent = frame->tree().parent();
    if (!parent || !parent->isLocalFrame())
        return 1;
    return toLocalFrame(parent)->pageZoomFactor();
}

inline float parentTextZoomFactor(LocalFrame* frame)
{
    Frame* parent = frame->tree().parent();
    if (!parent || !parent->isLocalFrame())
        return 1;
    return toLocalFrame(parent)->textZoomFactor();
}

} // namespace

PassRefPtrWillBeRawPtr<LocalFrame> LocalFrame::create(FrameLoaderClient* client, FrameHost* host, FrameOwner* owner)
{
    RefPtrWillBeRawPtr<LocalFrame> frame = adoptRefWillBeNoop(new LocalFrame(client, host, owner));
    InspectorInstrumentation::frameAttachedToParent(frame.get());
    return frame.release();
}

void LocalFrame::setView(PassRefPtrWillBeRawPtr<FrameView> view)
{
    ASSERT(!m_view || m_view != view);
    detachView();

    // Prepare for destruction now, so any unload event handlers get run and the LocalDOMWindow is
    // notified. If we wait until the view is destroyed, then things won't be hooked up enough for
    // these calls to work.
    if (!view && document() && document()->isActive()) {
        // FIXME: We don't call willRemove here. Why is that OK?
        document()->prepareForDestruction();
    }

    eventHandler().clear();

    m_view = view;
}

void LocalFrame::createView(const IntSize& viewportSize, const Color& backgroundColor, bool transparent,
    ScrollbarMode horizontalScrollbarMode, bool horizontalLock,
    ScrollbarMode verticalScrollbarMode, bool verticalLock)
{
    ASSERT(this);
    ASSERT(page());

    bool isLocalRoot = this->isLocalRoot();

    if (isLocalRoot && view())
        view()->setParentVisible(false);

    setView(nullptr);

    RefPtrWillBeRawPtr<FrameView> frameView = nullptr;
    if (isLocalRoot) {
        frameView = FrameView::create(this, viewportSize);

        // The layout size is set by WebViewImpl to support @viewport
        frameView->setLayoutSizeFixedToFrameSize(false);
    } else {
        frameView = FrameView::create(this);
    }

    frameView->setScrollbarModes(horizontalScrollbarMode, verticalScrollbarMode, horizontalLock, verticalLock);

    setView(frameView);

    frameView->updateBackgroundRecursively(backgroundColor, transparent);

    if (isLocalRoot)
        frameView->setParentVisible(true);

    // FIXME: Not clear what the right thing for OOPI is here.
    if (ownerLayoutObject()) {
        HTMLFrameOwnerElement* owner = deprecatedLocalOwner();
        ASSERT(owner);
        // FIXME: OOPI might lead to us temporarily lying to a frame and telling it
        // that it's owned by a FrameOwner that knows nothing about it. If we're
        // lying to this frame, don't let it clobber the existing widget.
        if (owner->contentFrame() == this)
            owner->setWidget(frameView);
    }

    if (HTMLFrameOwnerElement* owner = deprecatedLocalOwner())
        view()->setCanHaveScrollbars(owner->scrollingMode() != ScrollbarAlwaysOff);
}

LocalFrame::~LocalFrame()
{
    // Verify that the FrameView has been cleared as part of detaching
    // the frame owner.
    ASSERT(!m_view);
#if !ENABLE(OILPAN)
    // Oilpan: see setDOMWindow() comment why it is acceptable not to
    // explicitly call setDOMWindow() here.
    setDOMWindow(nullptr);
#endif
}

DEFINE_TRACE(LocalFrame)
{
    visitor->trace(m_instrumentingAgents);
#if ENABLE(OILPAN)
    visitor->trace(m_loader);
    visitor->trace(m_navigationScheduler);
    visitor->trace(m_view);
    visitor->trace(m_domWindow);
    visitor->trace(m_pagePopupOwner);
    visitor->trace(m_script);
    visitor->trace(m_editor);
    visitor->trace(m_spellChecker);
    visitor->trace(m_selection);
    visitor->trace(m_eventHandler);
    visitor->trace(m_console);
    visitor->trace(m_inputMethodController);
    visitor->template registerWeakMembers<LocalFrame, &LocalFrame::clearWeakMembers>(this);
    HeapSupplementable<LocalFrame>::trace(visitor);
#endif
    LocalFrameLifecycleNotifier::trace(visitor);
    Frame::trace(visitor);
}

DOMWindow* LocalFrame::domWindow() const
{
    return m_domWindow.get();
}

WindowProxy* LocalFrame::windowProxy(DOMWrapperWorld& world)
{
    return m_script->windowProxy(world);
}

void LocalFrame::navigate(Document& originDocument, const KURL& url, bool lockBackForwardList)
{
    m_navigationScheduler.scheduleLocationChange(&originDocument, url.string(), lockBackForwardList);
}

void LocalFrame::reload(ReloadPolicy reloadPolicy, ClientRedirectPolicy clientRedirectPolicy)
{
    ASSERT(clientRedirectPolicy == NotClientRedirect || reloadPolicy == NormalReload);
    if (clientRedirectPolicy == NotClientRedirect)
        m_loader.reload(reloadPolicy);
    else
        m_navigationScheduler.scheduleReload();
}

void LocalFrame::detach()
{
    // A lot of the following steps can result in the current frame being
    // detached, so protect a reference to it.
    RefPtrWillBeRawPtr<LocalFrame> protect(this);
    m_loader.stopAllLoaders();
    m_loader.dispatchUnloadEvent();
    detachChildren();
    // stopAllLoaders() needs to be called after detachChildren(), because detachChildren()
    // will trigger the unload event handlers of any child frames, and those event
    // handlers might start a new subresource load in this frame.
    m_loader.stopAllLoaders();
    m_loader.detach();
    if (!client())
        return;

    client()->willBeDetached();
    // Notify ScriptController that the frame is closing, since its cleanup ends up calling
    // back to FrameLoaderClient via WindowProxy.
    script().clearForClose();
    ScriptForbiddenScope forbidScript;
    setView(nullptr);
    willDetachFrameHost();
    InspectorInstrumentation::frameDetachedFromParent(this);
    Frame::detach();
    // Clear the FrameLoader right here rather than during
    // finalization. Too late to access various heap objects at that
    // stage.
    m_loader.clear();

    // Signal frame destruction here rather than in the destructor.
    // Main motivation is to avoid being dependent on its exact timing (Oilpan.)
    LocalFrameLifecycleNotifier::notifyContextDestroyed();
    m_supplements.clear();
}

SecurityContext* LocalFrame::securityContext() const
{
    return document();
}

void LocalFrame::printNavigationErrorMessage(const Frame& targetFrame, const char* reason)
{
    if (!targetFrame.isLocalFrame())
        return;

    const LocalFrame& targetLocalFrame = toLocalFrameTemporary(targetFrame);
    String message = "Unsafe JavaScript attempt to initiate navigation for frame with URL '" + targetLocalFrame.document()->url().string() + "' from frame with URL '" + document()->url().string() + "'. " + reason + "\n";

    // FIXME: should we print to the console of the document performing the navigation instead?
    targetLocalFrame.localDOMWindow()->printErrorMessage(message);
}

bool LocalFrame::isLoadingAsChild() const
{
    return isLoading() || !document()->isLoadCompleted();
}

WindowProxyManager* LocalFrame::windowProxyManager() const
{
    return m_script->windowProxyManager();
}

void LocalFrame::disconnectOwnerElement()
{
    if (owner()) {
        if (Document* document = this->document())
            document->topDocument().clearAXObjectCache();
#if ENABLE(OILPAN)
        // First give the plugin elements holding persisted,
        // renderer-less plugins the opportunity to dispose of them.
        for (const auto& pluginElement : m_pluginElements)
            pluginElement->disconnectContentFrame();
        m_pluginElements.clear();
#endif
    }
    Frame::disconnectOwnerElement();
}

void LocalFrame::willDetachFrameHost()
{
    LocalFrameLifecycleNotifier::notifyWillDetachFrameHost();

    // FIXME: Page should take care of updating focus/scrolling instead of Frame.
    // FIXME: It's unclear as to why this is called more than once, but it is,
    // so page() could be null.
    if (page() && page()->focusController().focusedFrame() == this)
        page()->focusController().setFocusedFrame(nullptr);
    script().clearScriptObjects();

    if (page() && page()->scrollingCoordinator() && m_view)
        page()->scrollingCoordinator()->willDestroyScrollableArea(m_view.get());
}

void LocalFrame::setDOMWindow(PassRefPtrWillBeRawPtr<LocalDOMWindow> domWindow)
{
    // Oilpan: setDOMWindow() cannot be used when finalizing. Which
    // is acceptable as its actions are either not needed or handled
    // by other means --
    //
    //  - LocalFrameLifecycleObserver::willDetachFrameHost() will have
    //    signalled the Inspector frameWindowDiscarded() notifications.
    //    We assume that all LocalFrames are detached, where that notification
    //    will have been done.
    //
    //  - Calling LocalDOMWindow::reset() is not needed (called from
    //    Frame::setDOMWindow().) The Member references it clears will now
    //    die with the window. And the registered DOMWindowProperty instances that don't,
    //    only keep a weak reference to this frame, so there's no need to be
    //    explicitly notified that this frame is going away.
    if (m_domWindow && host())
        host()->consoleMessageStorage().frameWindowDiscarded(m_domWindow.get());
    if (domWindow)
        script().clearWindowProxy();

    if (m_domWindow)
        m_domWindow->reset();
    m_domWindow = domWindow;
}

Document* LocalFrame::document() const
{
    return m_domWindow ? m_domWindow->document() : nullptr;
}

void LocalFrame::setPagePopupOwner(Element& owner)
{
    m_pagePopupOwner = &owner;
}

LayoutView* LocalFrame::contentRenderer() const
{
    return document() ? document()->layoutView() : nullptr;
}

void LocalFrame::didChangeVisibilityState()
{
    if (document())
        document()->didChangeVisibilityState();

    WillBeHeapVector<RefPtrWillBeMember<LocalFrame>> childFrames;
    for (Frame* child = tree().firstChild(); child; child = child->tree().nextSibling()) {
        if (child->isLocalFrame())
            childFrames.append(toLocalFrame(child));
    }

    for (size_t i = 0; i < childFrames.size(); ++i)
        childFrames[i]->didChangeVisibilityState();
}

LocalFrame* LocalFrame::localFrameRoot()
{
    LocalFrame* curFrame = this;
    while (curFrame && curFrame->tree().parent() && curFrame->tree().parent()->isLocalFrame())
        curFrame = toLocalFrame(curFrame->tree().parent());

    return curFrame;
}

bool LocalFrame::inScope(TreeScope* scope) const
{
    ASSERT(scope);
    Document* doc = document();
    if (!doc)
        return false;
    // FIXME: This check is broken in for OOPI.
    HTMLFrameOwnerElement* owner = doc->ownerElement();
    if (!owner)
        return false;
    return owner->treeScope() == scope;
}

String LocalFrame::layerTreeAsText(LayerTreeFlags flags) const
{
    TextStream textStream;
    textStream << localLayerTreeAsText(flags);

    for (Frame* child = tree().firstChild(); child; child = child->tree().traverseNext(this)) {
        if (!child->isLocalFrame())
            continue;
        String childLayerTree = toLocalFrame(child)->localLayerTreeAsText(flags);
        if (!childLayerTree.length())
            continue;

        textStream << "\n\n--------\nFrame: '";
        textStream << child->tree().uniqueName();
        textStream << "'\n--------\n";
        textStream << childLayerTree;
    }

    return textStream.release();
}

void LocalFrame::setPrinting(bool printing, const FloatSize& pageSize, const FloatSize& originalPageSize, float maximumShrinkRatio)
{
    // In setting printing, we should not validate resources already cached for the document.
    // See https://bugs.webkit.org/show_bug.cgi?id=43704
    ResourceCacheValidationSuppressor validationSuppressor(document()->fetcher());

    document()->setPrinting(printing);
    view()->adjustMediaTypeForPrinting(printing);

    document()->styleResolverChanged();
    if (shouldUsePrintingLayout()) {
        view()->forceLayoutForPagination(pageSize, originalPageSize, maximumShrinkRatio);
    } else {
        if (LayoutView* layoutView = view()->layoutView())
            layoutView->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::PrintingChanged);
        view()->layout();
        view()->adjustViewSize();
    }

    // Subframes of the one we're printing don't lay out to the page size.
    for (RefPtrWillBeRawPtr<Frame> child = tree().firstChild(); child; child = child->tree().nextSibling()) {
        if (child->isLocalFrame())
            toLocalFrame(child.get())->setPrinting(printing, FloatSize(), FloatSize(), 0);
    }
}

bool LocalFrame::shouldUsePrintingLayout() const
{
    // Only top frame being printed should be fit to page size.
    // Subframes should be constrained by parents only.
    return document()->printing() && (!tree().parent() || !tree().parent()->isLocalFrame() || !toLocalFrame(tree().parent())->document()->printing());
}

FloatSize LocalFrame::resizePageRectsKeepingRatio(const FloatSize& originalSize, const FloatSize& expectedSize)
{
    FloatSize resultSize;
    if (!contentRenderer())
        return FloatSize();

    if (contentRenderer()->style()->isHorizontalWritingMode()) {
        ASSERT(fabs(originalSize.width()) > std::numeric_limits<float>::epsilon());
        float ratio = originalSize.height() / originalSize.width();
        resultSize.setWidth(floorf(expectedSize.width()));
        resultSize.setHeight(floorf(resultSize.width() * ratio));
    } else {
        ASSERT(fabs(originalSize.height()) > std::numeric_limits<float>::epsilon());
        float ratio = originalSize.width() / originalSize.height();
        resultSize.setHeight(floorf(expectedSize.height()));
        resultSize.setWidth(floorf(resultSize.height() * ratio));
    }
    return resultSize;
}

void LocalFrame::setPageZoomFactor(float factor)
{
    setPageAndTextZoomFactors(factor, m_textZoomFactor);
}

void LocalFrame::setTextZoomFactor(float factor)
{
    setPageAndTextZoomFactors(m_pageZoomFactor, factor);
}

void LocalFrame::setPageAndTextZoomFactors(float pageZoomFactor, float textZoomFactor)
{
    if (m_pageZoomFactor == pageZoomFactor && m_textZoomFactor == textZoomFactor)
        return;

    Page* page = this->page();
    if (!page)
        return;

    Document* document = this->document();
    if (!document)
        return;

    // Respect SVGs zoomAndPan="disabled" property in standalone SVG documents.
    // FIXME: How to handle compound documents + zoomAndPan="disabled"? Needs SVG WG clarification.
    if (document->isSVGDocument()) {
        if (!document->accessSVGExtensions().zoomAndPanEnabled())
            return;
    }

    if (m_pageZoomFactor != pageZoomFactor) {
        if (FrameView* view = this->view()) {
            // Update the scroll position when doing a full page zoom, so the content stays in relatively the same position.
            LayoutPoint scrollPosition = view->scrollPosition();
            float percentDifference = (pageZoomFactor / m_pageZoomFactor);
            view->setScrollPosition(DoublePoint(scrollPosition.x() * percentDifference, scrollPosition.y() * percentDifference));
        }
    }

    m_pageZoomFactor = pageZoomFactor;
    m_textZoomFactor = textZoomFactor;

    for (RefPtrWillBeRawPtr<Frame> child = tree().firstChild(); child; child = child->tree().nextSibling()) {
        if (child->isLocalFrame())
            toLocalFrame(child.get())->setPageAndTextZoomFactors(m_pageZoomFactor, m_textZoomFactor);
    }

    document->setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::Zoom));
    document->updateLayoutIgnorePendingStylesheets();
}

void LocalFrame::deviceScaleFactorChanged()
{
    document()->mediaQueryAffectingValueChanged();
    for (RefPtrWillBeRawPtr<Frame> child = tree().firstChild(); child; child = child->tree().nextSibling()) {
        if (child->isLocalFrame())
            toLocalFrame(child.get())->deviceScaleFactorChanged();
    }
}

double LocalFrame::devicePixelRatio() const
{
    if (!m_host)
        return 0;

    double ratio = m_host->deviceScaleFactor();
    ratio *= pageZoomFactor();
    return ratio;
}

PassOwnPtr<DragImage> LocalFrame::paintIntoDragImage(
    const DisplayItemClientWrapper& displayItemClient, DisplayItem::Type clipType, RespectImageOrientationEnum shouldRespectImageOrientation, IntRect paintingRect)
{
    ASSERT(document()->isActive());
    float deviceScaleFactor = m_host->deviceScaleFactor();
    paintingRect.setWidth(paintingRect.width() * deviceScaleFactor);
    paintingRect.setHeight(paintingRect.height() * deviceScaleFactor);

    OwnPtr<ImageBuffer> buffer = ImageBuffer::create(paintingRect.size());
    if (!buffer)
        return nullptr;

    {
        DisplayItemListContextRecorder contextRecorder(*buffer->context());
        GraphicsContext& paintContext = contextRecorder.context();

        AffineTransform transform;
        transform.scale(deviceScaleFactor, deviceScaleFactor);
        transform.translate(-paintingRect.x(), -paintingRect.y());
        TransformRecorder transformRecorder(paintContext, displayItemClient, transform);

        ClipRecorder clipRecorder(paintContext, displayItemClient, clipType,
            LayoutRect(0, 0, paintingRect.maxX(), paintingRect.maxY()));

        m_view->paintContents(&paintContext, paintingRect);
    }

    RefPtr<Image> image = buffer->copyImage();
    return DragImage::create(image.get(), shouldRespectImageOrientation, deviceScaleFactor);
}

PassOwnPtr<DragImage> LocalFrame::nodeImage(Node& node)
{
    if (!node.layoutObject())
        return nullptr;

    const ScopedFramePaintingState state(this, &node);

    m_view->updateLayoutAndStyleForPainting();

    m_view->setPaintBehavior(state.paintBehavior | PaintBehaviorFlattenCompositingLayers);

    m_view->setNodeToDraw(&node); // Enable special sub-tree drawing mode.

    // Document::updateLayout may have blown away the original LayoutObject.
    LayoutObject* layoutObject = node.layoutObject();
    if (!layoutObject)
        return nullptr;

    IntRect rect;

    return paintIntoDragImage(*layoutObject, DisplayItem::ClipNodeImage, layoutObject->shouldRespectImageOrientation(),
        layoutObject->paintingRootRect(rect));
}

PassOwnPtr<DragImage> LocalFrame::dragImageForSelection()
{
    if (!selection().isRange())
        return nullptr;

    const ScopedFramePaintingState state(this, 0);
    m_view->setPaintBehavior(PaintBehaviorSelectionOnly | PaintBehaviorFlattenCompositingLayers);
    m_view->updateLayoutAndStyleForPainting();

    return paintIntoDragImage(*this, DisplayItem::ClipSelectionImage, DoNotRespectImageOrientation, enclosingIntRect(selection().bounds()));
}

String LocalFrame::selectedText() const
{
    return selection().selectedText();
}

String LocalFrame::selectedTextForClipboard() const
{
    return selection().selectedTextForClipboard();
}

VisiblePosition LocalFrame::visiblePositionForPoint(const IntPoint& framePoint)
{
    HitTestResult result = eventHandler().hitTestResultAtPoint(framePoint);
    Node* node = result.innerNonSharedNode();
    if (!node)
        return VisiblePosition();
    LayoutObject* renderer = node->layoutObject();
    if (!renderer)
        return VisiblePosition();
    VisiblePosition visiblePos = VisiblePosition(renderer->positionForPoint(result.localPoint()));
    if (visiblePos.isNull())
        visiblePos = VisiblePosition(firstPositionInOrBeforeNode(node));
    return visiblePos;
}

Document* LocalFrame::documentAtPoint(const IntPoint& pointInRootFrame)
{
    if (!view())
        return nullptr;

    IntPoint pt = view()->rootFrameToContents(pointInRootFrame);

    if (!contentRenderer())
        return nullptr;
    HitTestResult result = eventHandler().hitTestResultAtPoint(pt, HitTestRequest::ReadOnly | HitTestRequest::Active);
    return result.innerNode() ? &result.innerNode()->document() : nullptr;
}

PassRefPtrWillBeRawPtr<Range> LocalFrame::rangeForPoint(const IntPoint& framePoint)
{
    VisiblePosition position = visiblePositionForPoint(framePoint);
    if (position.isNull())
        return nullptr;

    VisiblePosition previous = position.previous();
    if (previous.isNotNull()) {
        RefPtrWillBeRawPtr<Range> previousCharacterRange = makeRange(previous, position);
        IntRect rect = editor().firstRectForRange(previousCharacterRange.get());
        if (rect.contains(framePoint))
            return previousCharacterRange.release();
    }

    VisiblePosition next = position.next();
    if (RefPtrWillBeRawPtr<Range> nextCharacterRange = makeRange(position, next)) {
        IntRect rect = editor().firstRectForRange(nextCharacterRange.get());
        if (rect.contains(framePoint))
            return nextCharacterRange.release();
    }

    return nullptr;
}

bool LocalFrame::isURLAllowed(const KURL& url) const
{
    // We allow one level of self-reference because some sites depend on that,
    // but we don't allow more than one.
    if (host()->subframeCount() >= FrameHost::maxNumberOfFrames)
        return false;
    bool foundSelfReference = false;
    for (const Frame* frame = this; frame; frame = frame->tree().parent()) {
        if (!frame->isLocalFrame())
            continue;
        if (equalIgnoringFragmentIdentifier(toLocalFrame(frame)->document()->url(), url)) {
            if (foundSelfReference)
                return false;
            foundSelfReference = true;
        }
    }
    return true;
}

bool LocalFrame::shouldReuseDefaultView(const KURL& url) const
{
    return loader().stateMachine()->isDisplayingInitialEmptyDocument() && document()->isSecureTransitionTo(url);
}

void LocalFrame::removeSpellingMarkersUnderWords(const Vector<String>& words)
{
    spellChecker().removeSpellingMarkersUnderWords(words);
}

static bool scrollAreaOnBothAxes(const FloatSize& delta, ScrollableArea& view)
{
    bool scrolledHorizontal = view.scroll(ScrollLeft, ScrollByPrecisePixel, delta.width());
    bool scrolledVertical = view.scroll(ScrollUp, ScrollByPrecisePixel, delta.height());
    return scrolledHorizontal || scrolledVertical;
}

// Returns true if a scroll occurred.
bool LocalFrame::applyScrollDelta(const FloatSize& delta, bool isScrollBegin)
{
    if (isScrollBegin)
        host()->topControls().scrollBegin();

    if (!view() || delta.isZero())
        return false;

    // If this is main frame, allow top controls to scroll first.
    bool giveToTopControls = false;
    if (isMainFrame()) {
        // Always give the delta to the top controls if the scroll is in
        // the direction to show the top controls. If it's in the
        // direction to hide the top controls, only give the delta to the
        // top controls when the frame can scroll.
        giveToTopControls = delta.height() > 0
            || view()->scrollPosition().y() < view()->maximumScrollPosition().y();
    }

    FloatSize remainingDelta = delta;
    if (giveToTopControls)
        remainingDelta = host()->topControls().scrollBy(remainingDelta);

    if (remainingDelta.isZero())
        return true;

    bool consumed = remainingDelta != delta;

    if (scrollAreaOnBothAxes(remainingDelta, *view()))
        return true;

    // If this is the main frame and it didn't scroll, propagate up to the pinch viewport.
    if (!isMainFrame())
        return consumed;

    if (scrollAreaOnBothAxes(remainingDelta, page()->frameHost().pinchViewport()))
        return true;

    return consumed;
}

#if ENABLE(OILPAN)
void LocalFrame::registerPluginElement(HTMLPlugInElement* plugin)
{
    m_pluginElements.add(plugin);
}

void LocalFrame::unregisterPluginElement(HTMLPlugInElement* plugin)
{
    ASSERT(m_pluginElements.contains(plugin));
    m_pluginElements.remove(plugin);
}

void LocalFrame::clearWeakMembers(Visitor* visitor)
{
    Vector<HTMLPlugInElement*> deadPlugins;
    for (const auto& pluginElement : m_pluginElements) {
        if (!visitor->isAlive(pluginElement)) {
            pluginElement->shouldDisposePlugin();
            deadPlugins.append(pluginElement);
        }
    }
    for (unsigned i = 0; i < deadPlugins.size(); ++i)
        m_pluginElements.remove(deadPlugins[i]);
}
#endif

String LocalFrame::localLayerTreeAsText(unsigned flags) const
{
    if (!contentRenderer())
        return String();

    return contentRenderer()->compositor()->layerTreeAsText(static_cast<LayerTreeFlags>(flags));
}

inline LocalFrame::LocalFrame(FrameLoaderClient* client, FrameHost* host, FrameOwner* owner)
    : Frame(client, host, owner)
    , m_loader(this)
    , m_navigationScheduler(this)
    , m_script(ScriptController::create(this))
    , m_editor(Editor::create(*this))
    , m_spellChecker(SpellChecker::create(*this))
    , m_selection(FrameSelection::create(this))
    , m_eventHandler(adoptPtrWillBeNoop(new EventHandler(this)))
    , m_console(FrameConsole::create(*this))
    , m_inputMethodController(InputMethodController::create(*this))
    , m_pageZoomFactor(parentPageZoomFactor(this))
    , m_textZoomFactor(parentTextZoomFactor(this))
    , m_inViewSourceMode(false)
    , m_shouldSendDPRHint(false)
    , m_shouldSendRWHint(false)
{
    if (isLocalRoot())
        m_instrumentingAgents = InstrumentingAgents::create();
    else
        m_instrumentingAgents = localFrameRoot()->m_instrumentingAgents;
}

void LocalFrame::detachView()
{
    // We detach the FrameView's custom scroll bars as early as
    // possible to prevent m_doc->detach() from messing with the view
    // such that its scroll bars won't be torn down.
    //
    // FIXME: We should revisit this.
    if (m_view)
        m_view->prepareForDetach();
}

} // namespace blink
