/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "web/ChromeClientImpl.h"

#include "bindings/core/v8/ScriptController.h"
#include "core/HTMLNames.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/Document.h"
#include "core/dom/Fullscreen.h"
#include "core/dom/Node.h"
#include "core/frame/Console.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/forms/ColorChooser.h"
#include "core/html/forms/ColorChooserClient.h"
#include "core/html/forms/DateTimeChooser.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutPart.h"
#include "core/layout/compositing/CompositedSelectionBound.h"
#include "core/loader/DocumentLoader.h"
#include "core/loader/FrameLoadRequest.h"
#include "core/page/Page.h"
#include "modules/accessibility/AXObject.h"
#include "platform/Cursor.h"
#include "platform/FileChooser.h"
#include "platform/PlatformScreen.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/exported/WrappedResourceRequest.h"
#include "platform/geometry/IntRect.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCursorInfo.h"
#include "public/platform/WebRect.h"
#include "public/platform/WebSelectionBound.h"
#include "public/platform/WebURLRequest.h"
#include "public/web/WebAXObject.h"
#include "public/web/WebAutofillClient.h"
#include "public/web/WebColorChooser.h"
#include "public/web/WebColorSuggestion.h"
#include "public/web/WebConsoleMessage.h"
#include "public/web/WebFrameClient.h"
#include "public/web/WebInputElement.h"
#include "public/web/WebInputEvent.h"
#include "public/web/WebKit.h"
#include "public/web/WebNode.h"
#include "public/web/WebPlugin.h"
#include "public/web/WebPopupMenuInfo.h"
#include "public/web/WebSettings.h"
#include "public/web/WebTextDirection.h"
#include "public/web/WebTouchAction.h"
#include "public/web/WebUserGestureIndicator.h"
#include "public/web/WebUserGestureToken.h"
#include "public/web/WebViewClient.h"
#include "public/web/WebWindowFeatures.h"
#include "web/ColorChooserPopupUIController.h"
#include "web/ColorChooserUIController.h"
#include "web/DateTimeChooserImpl.h"
#include "web/ExternalDateTimeChooser.h"
#include "web/ExternalPopupMenu.h"
#include "web/PopupMenuChromium.h"
#include "web/PopupMenuImpl.h"
#include "web/WebFileChooserCompletionImpl.h"
#include "web/WebFrameWidgetImpl.h"
#include "web/WebInputEventConversion.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebPluginContainerImpl.h"
#include "web/WebPopupMenuImpl.h"
#include "web/WebSettingsImpl.h"
#include "web/WebViewImpl.h"
#include "wtf/text/CString.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/StringConcatenate.h"
#include "wtf/unicode/CharacterNames.h"

namespace blink {

class WebCompositorAnimationTimeline;

// Converts a AXObjectCache::AXNotification to a WebAXEvent
static WebAXEvent toWebAXEvent(AXObjectCache::AXNotification notification)
{
    // These enums have the same values; enforced in AssertMatchingEnums.cpp.
    return static_cast<WebAXEvent>(notification);
}

static WebSelectionBound toWebSelectionBound(const CompositedSelectionBound& bound)
{
    ASSERT(bound.layer);

    // These enums have the same values; enforced in AssertMatchingEnums.cpp.
    WebSelectionBound result(static_cast<WebSelectionBound::Type>(bound.type));
    result.layerId = bound.layer->platformLayer()->id();
    result.edgeTopInLayer = roundedIntPoint(bound.edgeTopInLayer);
    result.edgeBottomInLayer = roundedIntPoint(bound.edgeBottomInLayer);
    return result;
}

ChromeClientImpl::ChromeClientImpl(WebViewImpl* webView)
    : m_webView(webView)
{
}

ChromeClientImpl::~ChromeClientImpl()
{
}

void* ChromeClientImpl::webView() const
{
    return static_cast<void*>(m_webView);
}

void ChromeClientImpl::chromeDestroyed()
{
    // Our lifetime is bound to the WebViewImpl.
}

void ChromeClientImpl::setWindowRect(const IntRect& r)
{
    if (m_webView->client())
        m_webView->client()->setWindowRect(r);
}

IntRect ChromeClientImpl::windowRect()
{
    WebRect rect;
    if (m_webView->client())
        rect = m_webView->client()->rootWindowRect();
    else {
        // These numbers will be fairly wrong. The window's x/y coordinates will
        // be the top left corner of the screen and the size will be the content
        // size instead of the window size.
        rect.width = m_webView->size().width;
        rect.height = m_webView->size().height;
    }
    return IntRect(rect);
}

IntRect ChromeClientImpl::pageRect()
{
    // We hide the details of the window's border thickness from the web page by
    // simple re-using the window position here.  So, from the point-of-view of
    // the web page, the window has no border.
    return windowRect();
}

void ChromeClientImpl::focus()
{
    if (m_webView->client())
        m_webView->client()->didFocus();
}

bool ChromeClientImpl::canTakeFocus(WebFocusType)
{
    // For now the browser can always take focus if we're not running layout
    // tests.
    return !layoutTestMode();
}

void ChromeClientImpl::takeFocus(WebFocusType type)
{
    if (!m_webView->client())
        return;
    if (type == WebFocusTypeBackward)
        m_webView->client()->focusPrevious();
    else
        m_webView->client()->focusNext();
}

void ChromeClientImpl::focusedNodeChanged(Node* fromNode, Node* toNode)
{
    m_webView->client()->focusedNodeChanged(WebNode(fromNode), WebNode(toNode));

    WebURL focusURL;
    if (toNode && toNode->isElementNode() && toElement(toNode)->isLiveLink() && toNode->shouldHaveFocusAppearance())
        focusURL = toElement(toNode)->hrefURL();
    m_webView->client()->setKeyboardFocusURL(focusURL);
}

void ChromeClientImpl::focusedFrameChanged(LocalFrame* frame)
{
    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(frame);
    if (webframe && webframe->client())
        webframe->client()->frameFocused();
}

Page* ChromeClientImpl::createWindow(LocalFrame* frame, const FrameLoadRequest& r, const WindowFeatures& features,
    NavigationPolicy navigationPolicy, ShouldSendReferrer shouldSendReferrer)
{
    if (!m_webView->client())
        return nullptr;

    WebNavigationPolicy policy = static_cast<WebNavigationPolicy>(navigationPolicy);
    if (policy == WebNavigationPolicyIgnore)
        policy = getNavigationPolicy(features);
    else if (policy == WebNavigationPolicyNewBackgroundTab && getNavigationPolicy(m_windowFeatures) != WebNavigationPolicyNewBackgroundTab)
        policy = WebNavigationPolicyNewForegroundTab;

    ASSERT(frame->document());
    Fullscreen::fullyExitFullscreen(*frame->document());

    WebViewImpl* newView = toWebViewImpl(
        m_webView->client()->createView(WebLocalFrameImpl::fromFrame(frame), WrappedResourceRequest(r.resourceRequest()), features, r.frameName(), policy, shouldSendReferrer == NeverSendReferrer));
    if (!newView)
        return nullptr;
    return newView->page();
}

static inline void updatePolicyForEvent(const WebInputEvent* inputEvent, NavigationPolicy* policy)
{
    if (!inputEvent || inputEvent->type != WebInputEvent::MouseUp)
        return;

    const WebMouseEvent* mouseEvent = static_cast<const WebMouseEvent*>(inputEvent);

    unsigned short buttonNumber;
    switch (mouseEvent->button) {
    case WebMouseEvent::ButtonLeft:
        buttonNumber = 0;
        break;
    case WebMouseEvent::ButtonMiddle:
        buttonNumber = 1;
        break;
    case WebMouseEvent::ButtonRight:
        buttonNumber = 2;
        break;
    default:
        return;
    }
    bool ctrl = mouseEvent->modifiers & WebMouseEvent::ControlKey;
    bool shift = mouseEvent->modifiers & WebMouseEvent::ShiftKey;
    bool alt = mouseEvent->modifiers & WebMouseEvent::AltKey;
    bool meta = mouseEvent->modifiers & WebMouseEvent::MetaKey;

    NavigationPolicy userPolicy = *policy;
    navigationPolicyFromMouseEvent(buttonNumber, ctrl, shift, alt, meta, &userPolicy);
    // User and app agree that we want a new window; let the app override the decorations.
    if (userPolicy == NavigationPolicyNewWindow && *policy == NavigationPolicyNewPopup)
        return;
    *policy = userPolicy;
}

WebNavigationPolicy ChromeClientImpl::getNavigationPolicy(const WindowFeatures& features)
{
    // If our default configuration was modified by a script or wasn't
    // created by a user gesture, then show as a popup. Else, let this
    // new window be opened as a toplevel window.
    bool asPopup = !features.toolBarVisible
        || !features.statusBarVisible
        || !features.scrollbarsVisible
        || !features.menuBarVisible
        || !features.resizable;

    NavigationPolicy policy = NavigationPolicyNewForegroundTab;
    if (asPopup)
        policy = NavigationPolicyNewPopup;
    updatePolicyForEvent(WebViewImpl::currentInputEvent(), &policy);

    return static_cast<WebNavigationPolicy>(policy);
}

void ChromeClientImpl::show(NavigationPolicy navigationPolicy)
{
    if (!m_webView->client())
        return;

    WebNavigationPolicy policy = static_cast<WebNavigationPolicy>(navigationPolicy);
    if (policy == WebNavigationPolicyIgnore)
        policy = getNavigationPolicy(m_windowFeatures);
    else if (policy == WebNavigationPolicyNewBackgroundTab && getNavigationPolicy(m_windowFeatures) != WebNavigationPolicyNewBackgroundTab)
        policy = WebNavigationPolicyNewForegroundTab;
    m_webView->client()->show(policy);
}

void ChromeClientImpl::setToolbarsVisible(bool value)
{
    m_windowFeatures.toolBarVisible = value;
}

bool ChromeClientImpl::toolbarsVisible()
{
    return m_windowFeatures.toolBarVisible;
}

void ChromeClientImpl::setStatusbarVisible(bool value)
{
    m_windowFeatures.statusBarVisible = value;
}

bool ChromeClientImpl::statusbarVisible()
{
    return m_windowFeatures.statusBarVisible;
}

void ChromeClientImpl::setScrollbarsVisible(bool value)
{
    m_windowFeatures.scrollbarsVisible = value;
    WebLocalFrameImpl* webFrame = toWebLocalFrameImpl(m_webView->mainFrame());
    if (webFrame)
        webFrame->setCanHaveScrollbars(value);
}

bool ChromeClientImpl::scrollbarsVisible()
{
    return m_windowFeatures.scrollbarsVisible;
}

void ChromeClientImpl::setMenubarVisible(bool value)
{
    m_windowFeatures.menuBarVisible = value;
}

bool ChromeClientImpl::menubarVisible()
{
    return m_windowFeatures.menuBarVisible;
}

void ChromeClientImpl::setResizable(bool value)
{
    m_windowFeatures.resizable = value;
}

bool ChromeClientImpl::shouldReportDetailedMessageForSource(LocalFrame& localFrame, const String& url)
{
    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(localFrame.localFrameRoot());
    return webframe && webframe->client() && webframe->client()->shouldReportDetailedMessageForSource(url);
}

void ChromeClientImpl::addMessageToConsole(LocalFrame* localFrame, MessageSource source, MessageLevel level, const String& message, unsigned lineNumber, const String& sourceID, const String& stackTrace)
{
    WebLocalFrameImpl* frame = WebLocalFrameImpl::fromFrame(localFrame);
    if (frame && frame->client()) {
        frame->client()->didAddMessageToConsole(
            WebConsoleMessage(static_cast<WebConsoleMessage::Level>(level), message),
            sourceID,
            lineNumber,
            stackTrace);
    }
}

bool ChromeClientImpl::canRunBeforeUnloadConfirmPanel()
{
    return !!m_webView->client();
}

bool ChromeClientImpl::runBeforeUnloadConfirmPanel(const String& message, LocalFrame* frame)
{
    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(frame);

    bool isReload = false;
    WebDataSource* ds = webframe->provisionalDataSource();
    if (ds)
        isReload = (ds->navigationType() == WebNavigationTypeReload);

    if (webframe->client())
        return webframe->client()->runModalBeforeUnloadDialog(isReload, message);
    return false;
}

void ChromeClientImpl::closeWindowSoon()
{
    // Make sure this Page can no longer be found by JS.
    Page::ordinaryPages().remove(m_webView->page());

    // Make sure that all loading is stopped.  Ensures that JS stops executing!
    m_webView->mainFrame()->stopLoading();

    if (m_webView->client())
        m_webView->client()->closeWidgetSoon();
}

// Although a LocalFrame is passed in, we don't actually use it, since we
// already know our own m_webView.
void ChromeClientImpl::runJavaScriptAlert(LocalFrame* frame, const String& message)
{
    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(frame);
    if (webframe->client()) {
        if (WebUserGestureIndicator::isProcessingUserGesture())
            WebUserGestureIndicator::currentUserGestureToken().setJavascriptPrompt();
        webframe->client()->runModalAlertDialog(message);
    }
}

// See comments for runJavaScriptAlert().
bool ChromeClientImpl::runJavaScriptConfirm(LocalFrame* frame, const String& message)
{
    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(frame);
    if (webframe->client()) {
        if (WebUserGestureIndicator::isProcessingUserGesture())
            WebUserGestureIndicator::currentUserGestureToken().setJavascriptPrompt();
        return webframe->client()->runModalConfirmDialog(message);
    }
    return false;
}

// See comments for runJavaScriptAlert().
bool ChromeClientImpl::runJavaScriptPrompt(LocalFrame* frame,
                                           const String& message,
                                           const String& defaultValue,
                                           String& result)
{
    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(frame);
    if (webframe->client()) {
        if (WebUserGestureIndicator::isProcessingUserGesture())
            WebUserGestureIndicator::currentUserGestureToken().setJavascriptPrompt();
        WebString actualValue;
        bool ok = webframe->client()->runModalPromptDialog(
            message,
            defaultValue,
            &actualValue);
        if (ok)
            result = actualValue;
        return ok;
    }
    return false;
}

void ChromeClientImpl::setStatusbarText(const String& message)
{
    if (m_webView->client())
        m_webView->client()->setStatusText(message);
}

bool ChromeClientImpl::tabsToLinks()
{
    return m_webView->tabsToLinks();
}

IntRect ChromeClientImpl::windowResizerRect() const
{
    IntRect result;
    if (m_webView->client())
        result = m_webView->client()->windowResizerRect();
    return result;
}

void ChromeClientImpl::invalidateRect(const IntRect& updateRect)
{
    if (updateRect.isEmpty())
        return;
    m_webView->invalidateRect(updateRect);
}

void ChromeClientImpl::scheduleAnimation()
{
    m_webView->scheduleAnimation();
}

void ChromeClientImpl::scheduleAnimationForFrame(LocalFrame* localRoot)
{
    ASSERT(WebLocalFrameImpl::fromFrame(localRoot));
    // If the frame is still being created, it might not yet have a WebWidget.
    // FIXME: Is this the right thing to do? Is there a way to avoid having
    // a local frame root that doesn't have a WebWidget? During initialization
    // there is no content to draw so this call serves no purpose.
    if (WebLocalFrameImpl::fromFrame(localRoot)->frameWidget())
        WebLocalFrameImpl::fromFrame(localRoot)->frameWidget()->scheduleAnimation();
}

IntRect ChromeClientImpl::viewportToScreen(const IntRect& rectInViewport) const
{
    IntRect screenRect(rectInViewport);

    if (m_webView->client()) {
        WebRect windowRect = m_webView->client()->windowRect();
        screenRect.move(windowRect.x, windowRect.y);
    }

    return screenRect;
}

WebScreenInfo ChromeClientImpl::screenInfo() const
{
    return m_webView->client() ? m_webView->client()->screenInfo() : WebScreenInfo();
}

void ChromeClientImpl::contentsSizeChanged(LocalFrame* frame, const IntSize& size) const
{
    m_webView->didChangeContentsSize();

    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(frame);
    webframe->didChangeContentsSize(size);

    frame->loader().restoreScrollPositionAndViewState();
}

void ChromeClientImpl::pageScaleFactorChanged() const
{
    m_webView->pageScaleFactorChanged();
}

float ChromeClientImpl::clampPageScaleFactorToLimits(float scale) const
{
    return m_webView->clampPageScaleFactorToLimits(scale);
}

void ChromeClientImpl::layoutUpdated(LocalFrame* frame) const
{
    m_webView->layoutUpdated(WebLocalFrameImpl::fromFrame(frame));
}

void ChromeClientImpl::mouseDidMoveOverElement(const HitTestResult& result)
{
    if (!m_webView->client())
        return;

    WebURL url;
    // Find out if the mouse is over a link, and if so, let our UI know...
    if (result.isLiveLink() && !result.absoluteLinkURL().string().isEmpty()) {
        url = result.absoluteLinkURL();
    } else if (result.innerNonSharedNode()
        && (isHTMLObjectElement(*result.innerNonSharedNode())
            || isHTMLEmbedElement(*result.innerNonSharedNode()))) {
        LayoutObject* object = result.innerNonSharedNode()->layoutObject();
        if (object && object->isLayoutPart()) {
            Widget* widget = toLayoutPart(object)->widget();
            if (widget && widget->isPluginContainer()) {
                WebPluginContainerImpl* plugin = toWebPluginContainerImpl(widget);
                url = plugin->plugin()->linkAtPosition(result.roundedPointInInnerNodeFrame());
            }
        }
    }

    m_webView->client()->setMouseOverURL(url);
}

void ChromeClientImpl::setToolTip(const String& tooltipText, TextDirection dir)
{
    if (m_webView->client())
        m_webView->client()->setToolTipText(tooltipText, toWebTextDirection(dir));
}

void ChromeClientImpl::dispatchViewportPropertiesDidChange(const ViewportDescription& description) const
{
    m_webView->updatePageDefinedViewportConstraints(description);
}

void ChromeClientImpl::print(LocalFrame* frame)
{
    if (m_webView->client())
        m_webView->client()->printPage(WebLocalFrameImpl::fromFrame(frame));
}

PassOwnPtrWillBeRawPtr<ColorChooser> ChromeClientImpl::createColorChooser(LocalFrame* frame, ColorChooserClient* chooserClient, const Color&)
{
    OwnPtrWillBeRawPtr<ColorChooserUIController> controller = nullptr;
    if (RuntimeEnabledFeatures::pagePopupEnabled())
        controller = ColorChooserPopupUIController::create(frame, this, chooserClient);
    else
        controller = ColorChooserUIController::create(frame, chooserClient);
    controller->openUI();
    return controller.release();
}

PassRefPtr<DateTimeChooser> ChromeClientImpl::openDateTimeChooser(DateTimeChooserClient* pickerClient, const DateTimeChooserParameters& parameters)
{
#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
    return DateTimeChooserImpl::create(this, pickerClient, parameters);
#else
    return ExternalDateTimeChooser::create(this, m_webView->client(), pickerClient, parameters);
#endif
}

void ChromeClientImpl::runOpenPanel(LocalFrame* frame, PassRefPtr<FileChooser> fileChooser)
{
    WebViewClient* client = m_webView->client();
    if (!client)
        return;

    WebFileChooserParams params;
    params.multiSelect = fileChooser->settings().allowsMultipleFiles;
    params.directory = fileChooser->settings().allowsDirectoryUpload;
    params.acceptTypes = fileChooser->settings().acceptTypes();
    params.selectedFiles = fileChooser->settings().selectedFiles;
    if (params.selectedFiles.size() > 0)
        params.initialValue = params.selectedFiles[0];
    params.useMediaCapture = fileChooser->settings().useMediaCapture;
    params.needLocalPath = fileChooser->settings().allowsDirectoryUpload;

    WebFileChooserCompletionImpl* chooserCompletion =
        new WebFileChooserCompletionImpl(fileChooser);

    if (client->runFileChooser(params, chooserCompletion))
        return;

    // Choosing failed, so do callback with an empty list.
    chooserCompletion->didChooseFile(WebVector<WebString>());
}

void ChromeClientImpl::enumerateChosenDirectory(FileChooser* fileChooser)
{
    WebViewClient* client = m_webView->client();
    if (!client)
        return;

    WebFileChooserCompletionImpl* chooserCompletion =
        new WebFileChooserCompletionImpl(fileChooser);

    ASSERT(fileChooser && fileChooser->settings().selectedFiles.size());

    // If the enumeration can't happen, call the callback with an empty list.
    if (!client->enumerateChosenDirectory(fileChooser->settings().selectedFiles[0], chooserCompletion))
        chooserCompletion->didChooseFile(WebVector<WebString>());
}

void ChromeClientImpl::setCursor(const Cursor& cursor)
{
    setCursor(WebCursorInfo(cursor));
}

void ChromeClientImpl::setCursor(const WebCursorInfo& cursor)
{
#if OS(MACOSX)
    // On Mac the mousemove event propagates to both the popup and main window.
    // If a popup is open we don't want the main window to change the cursor.
    if (m_webView->hasOpenedPopup())
        return;
#endif
    if (m_webView->client())
        m_webView->client()->didChangeCursor(cursor);
}

void ChromeClientImpl::setCursorForPlugin(const WebCursorInfo& cursor)
{
    setCursor(cursor);
}

void ChromeClientImpl::postAccessibilityNotification(AXObject* obj, AXObjectCache::AXNotification notification)
{
    // Alert assistive technology about the accessibility object notification.
    if (!obj || !obj->document())
        return;

    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(obj->document()->axObjectCacheOwner().frame());
    if (webframe && webframe->client())
        webframe->client()->postAccessibilityEvent(WebAXObject(obj), toWebAXEvent(notification));

    // FIXME: delete these lines once Chrome only uses the frame client interface, above.
    if (m_webView->client())
        m_webView->client()->postAccessibilityEvent(WebAXObject(obj), toWebAXEvent(notification));
}

String ChromeClientImpl::acceptLanguages()
{
    return m_webView->client()->acceptLanguages();
}

bool ChromeClientImpl::paintCustomOverhangArea(GraphicsContext* context, const IntRect& horizontalOverhangArea, const IntRect& verticalOverhangArea, const IntRect& dirtyRect)
{
    LocalFrame* frame = m_webView->mainFrameImpl()->frame();
    WebPluginContainerImpl* pluginContainer = WebLocalFrameImpl::pluginContainerFromFrame(frame);
    if (pluginContainer)
        return pluginContainer->paintCustomOverhangArea(context, horizontalOverhangArea, verticalOverhangArea, dirtyRect);
    return false;
}

GraphicsLayerFactory* ChromeClientImpl::graphicsLayerFactory() const
{
    return m_webView->graphicsLayerFactory();
}

void ChromeClientImpl::attachRootGraphicsLayer(GraphicsLayer* rootLayer, LocalFrame* localRoot)
{
    // FIXME: For top-level frames we still use the WebView as a WebWidget. This special
    // case will be removed when top-level frames get WebFrameWidgets.
    if (localRoot->isMainFrame()) {
        m_webView->setRootGraphicsLayer(rootLayer);
    } else {
        WebLocalFrameImpl* webFrame = WebLocalFrameImpl::fromFrame(localRoot);
        // FIXME: The following conditional is only needed for staging until the Chromium patch
        // lands that instantiates a WebFrameWidget.
        if (!webFrame->frameWidget()) {
            m_webView->setRootGraphicsLayer(rootLayer);
            return;
        }
        ASSERT(webFrame && webFrame->frameWidget());
        webFrame->frameWidget()->setRootGraphicsLayer(rootLayer);
    }
}

void ChromeClientImpl::attachCompositorAnimationTimeline(WebCompositorAnimationTimeline* compositorTimeline, LocalFrame* localRoot)
{
    // FIXME: For top-level frames we still use the WebView as a WebWidget. This special
    // case will be removed when top-level frames get WebFrameWidgets.
    if (localRoot->isMainFrame()) {
        m_webView->attachCompositorAnimationTimeline(compositorTimeline);
    } else {
        WebLocalFrameImpl* webFrame = WebLocalFrameImpl::fromFrame(localRoot);
        // FIXME: The following conditional is only needed for staging until the Chromium patch
        // lands that instantiates a WebFrameWidget.
        if (!webFrame->frameWidget()) {
            m_webView->attachCompositorAnimationTimeline(compositorTimeline);
            return;
        }
        ASSERT(webFrame && webFrame->frameWidget());
        webFrame->frameWidget()->attachCompositorAnimationTimeline(compositorTimeline);
    }
}

void ChromeClientImpl::detachCompositorAnimationTimeline(WebCompositorAnimationTimeline* compositorTimeline, LocalFrame* localRoot)
{
    // FIXME: For top-level frames we still use the WebView as a WebWidget. This special
    // case will be removed when top-level frames get WebFrameWidgets.
    if (localRoot->isMainFrame()) {
        m_webView->detachCompositorAnimationTimeline(compositorTimeline);
    } else {
        WebLocalFrameImpl* webFrame = WebLocalFrameImpl::fromFrame(localRoot);
        // FIXME: The following conditional is only needed for staging until the Chromium patch
        // lands that instantiates a WebFrameWidget.
        if (!webFrame->frameWidget()) {
            m_webView->detachCompositorAnimationTimeline(compositorTimeline);
            return;
        }
        ASSERT(webFrame && webFrame->frameWidget());
        webFrame->frameWidget()->detachCompositorAnimationTimeline(compositorTimeline);
    }
}

void ChromeClientImpl::enterFullScreenForElement(Element* element)
{
    m_webView->enterFullScreenForElement(element);
}

void ChromeClientImpl::exitFullScreenForElement(Element* element)
{
    m_webView->exitFullScreenForElement(element);
}

void ChromeClientImpl::clearCompositedSelectionBounds()
{
    m_webView->clearCompositedSelectionBounds();
}

void ChromeClientImpl::updateCompositedSelectionBounds(const CompositedSelectionBound& anchor, const CompositedSelectionBound& focus)
{
    m_webView->updateCompositedSelectionBounds(toWebSelectionBound(anchor), toWebSelectionBound(focus));
}

bool ChromeClientImpl::hasOpenedPopup() const
{
    return m_webView->hasOpenedPopup();
}

PassRefPtrWillBeRawPtr<PopupMenu> ChromeClientImpl::createPopupMenu(LocalFrame& frame, PopupMenuClient* client)
{
    if (WebViewImpl::useExternalPopupMenus())
        return adoptRefWillBeNoop(new ExternalPopupMenu(frame, client, *m_webView));

    if (RuntimeEnabledFeatures::htmlPopupMenuEnabled() && RuntimeEnabledFeatures::pagePopupEnabled())
        return PopupMenuImpl::create(this, client);

    return adoptRefWillBeNoop(new PopupMenuChromium(frame, client));
}

PagePopup* ChromeClientImpl::openPagePopup(PagePopupClient* client)
{
    return m_webView->openPagePopup(client);
}

void ChromeClientImpl::closePagePopup(PagePopup* popup)
{
    m_webView->closePagePopup(popup);
}

DOMWindow* ChromeClientImpl::pagePopupWindowForTesting() const
{
    return m_webView->pagePopupWindow();
}

bool ChromeClientImpl::shouldRunModalDialogDuringPageDismissal(const DialogType& dialogType, const String& dialogMessage, Document::PageDismissalType dismissalType) const
{
    const char* kDialogs[] = {"alert", "confirm", "prompt"};
    int dialog = static_cast<int>(dialogType);
    ASSERT_WITH_SECURITY_IMPLICATION(0 <= dialog && dialog < static_cast<int>(arraysize(kDialogs)));

    const char* kDismissals[] = {"beforeunload", "pagehide", "unload"};
    int dismissal = static_cast<int>(dismissalType) - 1; // Exclude NoDismissal.
    ASSERT_WITH_SECURITY_IMPLICATION(0 <= dismissal && dismissal < static_cast<int>(arraysize(kDismissals)));

    Platform::current()->histogramEnumeration("Renderer.ModalDialogsDuringPageDismissal", dismissal * arraysize(kDialogs) + dialog, arraysize(kDialogs) * arraysize(kDismissals));

    String message = String("Blocked ") + kDialogs[dialog] + "('" + dialogMessage + "') during " + kDismissals[dismissal] + ".";
    m_webView->mainFrame()->addMessageToConsole(WebConsoleMessage(WebConsoleMessage::LevelError, message));

    return false;
}

void ChromeClientImpl::needTouchEvents(bool needsTouchEvents)
{
    m_webView->hasTouchEventHandlers(needsTouchEvents);
}

void ChromeClientImpl::setTouchAction(TouchAction touchAction)
{
    if (WebViewClient* client = m_webView->client()) {
        WebTouchAction webTouchAction = static_cast<WebTouchAction>(touchAction);
        client->setTouchAction(webTouchAction);
    }
}

bool ChromeClientImpl::requestPointerLock()
{
    return m_webView->requestPointerLock();
}

void ChromeClientImpl::requestPointerUnlock()
{
    return m_webView->requestPointerUnlock();
}

void ChromeClientImpl::annotatedRegionsChanged()
{
    WebViewClient* client = m_webView->client();
    if (client)
        client->draggableRegionsChanged();
}

void ChromeClientImpl::didAssociateFormControls(const WillBeHeapVector<RefPtrWillBeMember<Element>>& elements, LocalFrame* frame)
{
    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(frame);
    if (webframe->autofillClient())
        webframe->autofillClient()->didAssociateFormControls(elements);
}

void ChromeClientImpl::didCancelCompositionOnSelectionChange()
{
    if (m_webView->client())
        m_webView->client()->didCancelCompositionOnSelectionChange();
}

void ChromeClientImpl::willSetInputMethodState()
{
    if (m_webView->client())
        m_webView->client()->resetInputMethod();
}

void ChromeClientImpl::didUpdateTextOfFocusedElementByNonUserInput()
{
    if (m_webView->client())
        m_webView->client()->didUpdateTextOfFocusedElementByNonUserInput();
}

void ChromeClientImpl::showImeIfNeeded()
{
    if (m_webView->client())
        m_webView->client()->showImeIfNeeded();
}

void ChromeClientImpl::showUnhandledTapUIIfNeeded(IntPoint tappedPositionInViewport, Node* tappedNode, bool pageChanged)
{
    if (m_webView->client())
        m_webView->client()->showUnhandledTapUIIfNeeded(WebPoint(tappedPositionInViewport), WebNode(tappedNode), pageChanged);
}

void ChromeClientImpl::handleKeyboardEventOnTextField(HTMLInputElement& inputElement, KeyboardEvent& event)
{
    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(inputElement.document().frame());
    if (webframe->autofillClient())
        webframe->autofillClient()->textFieldDidReceiveKeyDown(WebInputElement(&inputElement), WebKeyboardEventBuilder(event));
}

void ChromeClientImpl::didChangeValueInTextField(HTMLFormControlElement& element)
{
    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(element.document().frame());
    if (webframe->autofillClient())
        webframe->autofillClient()->textFieldDidChange(WebFormControlElement(&element));
}

void ChromeClientImpl::didEndEditingOnTextField(HTMLInputElement& inputElement)
{
    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(inputElement.document().frame());
    if (webframe->autofillClient())
        webframe->autofillClient()->textFieldDidEndEditing(WebInputElement(&inputElement));
}

void ChromeClientImpl::openTextDataListChooser(HTMLInputElement& input)
{
    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(input.document().frame());
    if (webframe->autofillClient())
        webframe->autofillClient()->openTextDataListChooser(WebInputElement(&input));
}

void ChromeClientImpl::textFieldDataListChanged(HTMLInputElement& input)
{
    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(input.document().frame());
    if (webframe->autofillClient())
        webframe->autofillClient()->dataListOptionsChanged(WebInputElement(&input));
}

void ChromeClientImpl::xhrSucceeded(LocalFrame* frame)
{
    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(frame);
    if (webframe->autofillClient())
        webframe->autofillClient()->xhrSucceeded();
}

void ChromeClientImpl::registerViewportLayers() const
{
    if (m_webView->rootGraphicsLayer() && m_webView->layerTreeView())
        m_webView->page()->frameHost().pinchViewport().registerLayersWithTreeView(m_webView->layerTreeView());
}

void ChromeClientImpl::didUpdateTopControls() const
{
    m_webView->didUpdateTopControls();
}

} // namespace blink
