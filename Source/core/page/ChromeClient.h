/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011, 2012 Apple, Inc. All rights reserved.
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2012 Samsung Electronics. All rights reserved.
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

#ifndef ChromeClient_h
#define ChromeClient_h

#include "core/dom/AXObjectCache.h"
#include "core/inspector/ConsoleAPITypes.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/NavigationPolicy.h"
#include "core/frame/ConsoleTypes.h"
#include "core/html/forms/PopupMenuClient.h"
#include "core/style/ComputedStyleConstants.h"
#include "platform/Cursor.h"
#include "platform/HostWindow.h"
#include "platform/PopupMenu.h"
#include "platform/heap/Handle.h"
#include "platform/scroll/ScrollTypes.h"
#include "public/platform/WebFocusType.h"
#include "wtf/Forward.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Vector.h"

namespace blink {

class AXObject;
class ColorChooser;
class ColorChooserClient;
class DateTimeChooser;
class DateTimeChooserClient;
class Element;
class FileChooser;
class Frame;
class GraphicsContext;
class GraphicsLayer;
class GraphicsLayerFactory;
class HitTestResult;
class HTMLFormControlElement;
class HTMLInputElement;
class IntRect;
class LocalFrame;
class Node;
class Page;
class PagePopupDriver;
class PopupMenuClient;
class WebCompositorAnimationTimeline;

struct CompositedSelectionBound;
struct DateTimeChooserParameters;
struct FrameLoadRequest;
struct GraphicsDeviceAdapter;
struct ViewportDescription;
struct WindowFeatures;

class ChromeClient {
public:
    virtual void chromeDestroyed() = 0;

    virtual void setWindowRect(const IntRect&) = 0;
    virtual IntRect windowRect() = 0;

    virtual IntRect pageRect() = 0;

    virtual void focus() = 0;

    virtual bool canTakeFocus(WebFocusType) = 0;
    virtual void takeFocus(WebFocusType) = 0;

    virtual void focusedNodeChanged(Node*, Node*) = 0;

    virtual void focusedFrameChanged(LocalFrame*) = 0;

    // The LocalFrame pointer provides the ChromeClient with context about which
    // LocalFrame wants to create the new Page. Also, the newly created window
    // should not be shown to the user until the ChromeClient of the newly
    // created Page has its show method called.
    // The FrameLoadRequest parameter is only for ChromeClient to check if the
    // request could be fulfilled. The ChromeClient should not load the request.
    virtual Page* createWindow(LocalFrame*, const FrameLoadRequest&, const WindowFeatures&, NavigationPolicy, ShouldSendReferrer) = 0;
    virtual void show(NavigationPolicy) = 0;

    virtual void setToolbarsVisible(bool) = 0;
    virtual bool toolbarsVisible() = 0;

    virtual void setStatusbarVisible(bool) = 0;
    virtual bool statusbarVisible() = 0;

    virtual void setScrollbarsVisible(bool) = 0;
    virtual bool scrollbarsVisible() = 0;

    virtual void setMenubarVisible(bool) = 0;
    virtual bool menubarVisible() = 0;

    virtual void setResizable(bool) = 0;

    virtual bool shouldReportDetailedMessageForSource(LocalFrame&, const String& source) = 0;
    virtual void addMessageToConsole(LocalFrame*, MessageSource, MessageLevel, const String& message, unsigned lineNumber, const String& sourceID, const String& stackTrace) = 0;

    virtual bool canRunBeforeUnloadConfirmPanel() = 0;
    virtual bool runBeforeUnloadConfirmPanel(const String& message, LocalFrame*) = 0;

    virtual void closeWindowSoon() = 0;

    virtual void runJavaScriptAlert(LocalFrame*, const String&) = 0;
    virtual bool runJavaScriptConfirm(LocalFrame*, const String&) = 0;
    virtual bool runJavaScriptPrompt(LocalFrame*, const String& message, const String& defaultValue, String& result) = 0;
    virtual void setStatusbarText(const String&) = 0;
    virtual bool tabsToLinks() = 0;

    virtual void* webView() const = 0;

    virtual IntRect windowResizerRect() const = 0;

    // Methods used by HostWindow.
    virtual void invalidateRect(const IntRect&) = 0;
    virtual IntRect viewportToScreen(const IntRect&) const = 0;
    virtual blink::WebScreenInfo screenInfo() const = 0;
    virtual void setCursor(const Cursor&) = 0;
    virtual void scheduleAnimation() = 0;
    // End methods used by HostWindow.

    virtual void scheduleAnimationForFrame(LocalFrame*) { }

    virtual void dispatchViewportPropertiesDidChange(const ViewportDescription&) const { }

    virtual void contentsSizeChanged(LocalFrame*, const IntSize&) const = 0;
    virtual void pageScaleFactorChanged() const { }
    virtual float clampPageScaleFactorToLimits(float scale) const { return scale; }
    virtual void layoutUpdated(LocalFrame*) const { }

    virtual void mouseDidMoveOverElement(const HitTestResult&) = 0;

    virtual void setToolTip(const String&, TextDirection) = 0;

    virtual void print(LocalFrame*) = 0;

    virtual void annotatedRegionsChanged() = 0;

    virtual bool paintCustomOverhangArea(GraphicsContext*, const IntRect&, const IntRect&, const IntRect&) = 0;

    virtual PassOwnPtrWillBeRawPtr<ColorChooser> createColorChooser(LocalFrame*, ColorChooserClient*, const Color&) = 0;

    // This function is used for:
    //  - Mandatory date/time choosers if !ENABLE(INPUT_MULTIPLE_FIELDS_UI)
    //  - Date/time choosers for types for which LayoutTheme::supportsCalendarPicker
    //    returns true, if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
    //  - <datalist> UI for date/time input types regardless of
    //    ENABLE(INPUT_MULTIPLE_FIELDS_UI)
    virtual PassRefPtr<DateTimeChooser> openDateTimeChooser(DateTimeChooserClient*, const DateTimeChooserParameters&) = 0;

    virtual void openTextDataListChooser(HTMLInputElement&) = 0;

    virtual void runOpenPanel(LocalFrame*, PassRefPtr<FileChooser>) = 0;

    // Asychronous request to enumerate all files in a directory chosen by the user.
    virtual void enumerateChosenDirectory(FileChooser*) = 0;

    // Allows ports to customize the type of graphics layers created by this page.
    virtual GraphicsLayerFactory* graphicsLayerFactory() const { return nullptr; }

    // Pass 0 as the GraphicsLayer to detach the root layer.
    // This sets the graphics layer for the LocalFrame's WebWidget, if it has one. Otherwise
    // it sets it for the WebViewImpl.
    virtual void attachRootGraphicsLayer(GraphicsLayer*, LocalFrame* localRoot) = 0;

    virtual void attachCompositorAnimationTimeline(WebCompositorAnimationTimeline*, LocalFrame* localRoot) { }
    virtual void detachCompositorAnimationTimeline(WebCompositorAnimationTimeline*, LocalFrame* localRoot) { }

    virtual void enterFullScreenForElement(Element*) { }
    virtual void exitFullScreenForElement(Element*) { }

    virtual void clearCompositedSelectionBounds() { }
    virtual void updateCompositedSelectionBounds(const CompositedSelectionBound& anchor, const CompositedSelectionBound& focus) { }

    virtual void needTouchEvents(bool) = 0;

    virtual void setTouchAction(TouchAction) = 0;

    // Checks if there is an opened popup, called by LayoutMenuList::showPopup().
    virtual bool hasOpenedPopup() const = 0;
    virtual PassRefPtrWillBeRawPtr<PopupMenu> createPopupMenu(LocalFrame&, PopupMenuClient*) = 0;
    virtual DOMWindow* pagePopupWindowForTesting() const = 0;

    virtual void postAccessibilityNotification(AXObject*, AXObjectCache::AXNotification) { }
    virtual String acceptLanguages() = 0;

    enum DialogType {
        AlertDialog = 0,
        ConfirmDialog = 1,
        PromptDialog = 2,
        HTMLDialog = 3
    };
    virtual bool shouldRunModalDialogDuringPageDismissal(const DialogType&, const String&, Document::PageDismissalType) const { return true; }

    virtual bool isSVGImageChromeClient() const { return false; }

    virtual bool requestPointerLock() { return false; }
    virtual void requestPointerUnlock() { }

    virtual IntSize minimumWindowSize() const { return IntSize(100, 100); }

    virtual bool isChromeClientImpl() const { return false; }

    virtual void didAssociateFormControls(const WillBeHeapVector<RefPtrWillBeMember<Element>>&, LocalFrame*) { }
    virtual void didChangeValueInTextField(HTMLFormControlElement&) { }
    virtual void didEndEditingOnTextField(HTMLInputElement&) { }
    virtual void handleKeyboardEventOnTextField(HTMLInputElement&, KeyboardEvent&) { }
    virtual void textFieldDataListChanged(HTMLInputElement&) { }
    virtual void xhrSucceeded(LocalFrame*) { }

    // Input mehtod editor related functions.
    virtual void didCancelCompositionOnSelectionChange() { }
    virtual void willSetInputMethodState() { }
    virtual void didUpdateTextOfFocusedElementByNonUserInput() { }
    virtual void showImeIfNeeded() { }

    virtual void registerViewportLayers() const { }

    virtual void showUnhandledTapUIIfNeeded(IntPoint, Node*, bool) { }

    virtual void didUpdateTopControls() const { }

protected:
    virtual ~ChromeClient() { }
};

} // namespace blink

#endif // ChromeClient_h
