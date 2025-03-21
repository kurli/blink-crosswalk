/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
#include "public/web/WebAXObject.h"

#include "core/HTMLNames.h"
#include "core/css/CSSPrimitiveValueMappings.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/Document.h"
#include "core/dom/Node.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/PinchViewport.h"
#include "core/layout/LayoutView.h"
#include "core/style/ComputedStyle.h"
#include "core/page/EventHandler.h"
#include "core/page/Page.h"
#include "modules/accessibility/AXObject.h"
#include "modules/accessibility/AXTable.h"
#include "modules/accessibility/AXTableCell.h"
#include "modules/accessibility/AXTableColumn.h"
#include "modules/accessibility/AXTableRow.h"
#include "platform/PlatformKeyboardEvent.h"
#include "public/platform/WebPoint.h"
#include "public/platform/WebRect.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"
#include "public/web/WebDocument.h"
#include "public/web/WebNode.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

#if ENABLE(ASSERT)
// It's not safe to call some WebAXObject APIs if a layout is pending.
// Clients should call updateLayoutAndCheckValidity first.
static bool isLayoutClean(Document* document)
{
    if (!document || !document->view())
        return false;
    return document->lifecycle().state() >= DocumentLifecycle::LayoutClean
        || (document->lifecycle().state() == DocumentLifecycle::StyleClean && !document->view()->needsLayout());
}
#endif

void WebAXObject::reset()
{
    m_private.reset();
}

void WebAXObject::assign(const WebAXObject& other)
{
    m_private = other.m_private;
}

bool WebAXObject::equals(const WebAXObject& n) const
{
    return m_private.get() == n.m_private.get();
}

bool WebAXObject::isDetached() const
{
    if (m_private.isNull())
        return true;

    return m_private->isDetached();
}

int WebAXObject::axID() const
{
    if (isDetached())
        return -1;

    return m_private->axObjectID();
}

bool WebAXObject::updateLayoutAndCheckValidity()
{
    if (!isDetached()) {
        Document* document = m_private->document();
        if (!document || !document->topDocument().view())
            return false;
        document->topDocument().view()->updateLayoutAndStyleIfNeededRecursive();
    }

    // Doing a layout can cause this object to be invalid, so check again.
    return !isDetached();
}

WebString WebAXObject::accessibilityDescription() const
{
    if (isDetached())
        return WebString();

    ASSERT(isLayoutClean(m_private->document()));

    return m_private->accessibilityDescription();
}

WebString WebAXObject::actionVerb() const
{
    if (isDetached())
        return WebString();

    return m_private->actionVerb();
}

bool WebAXObject::canDecrement() const
{
    if (isDetached())
        return false;

    return m_private->isSlider();
}

bool WebAXObject::canIncrement() const
{
    if (isDetached())
        return false;

    return m_private->isSlider();
}

bool WebAXObject::canPress() const
{
    if (isDetached())
        return false;

    return m_private->actionElement() || m_private->isButton() || m_private->isMenuRelated();
}

bool WebAXObject::canSetFocusAttribute() const
{
    if (isDetached())
        return false;

    return m_private->canSetFocusAttribute();
}

bool WebAXObject::canSetValueAttribute() const
{
    if (isDetached())
        return false;

    return m_private->canSetValueAttribute();
}

unsigned WebAXObject::childCount() const
{
    if (isDetached())
        return 0;

    return m_private->children().size();
}

WebAXObject WebAXObject::childAt(unsigned index) const
{
    if (isDetached())
        return WebAXObject();

    if (m_private->children().size() <= index)
        return WebAXObject();

    return WebAXObject(m_private->children()[index]);
}

WebAXObject WebAXObject::parentObject() const
{
    if (isDetached())
        return WebAXObject();

    return WebAXObject(m_private->parentObject());
}

bool WebAXObject::canSetSelectedAttribute() const
{
    if (isDetached())
        return false;

    return m_private->canSetSelectedAttribute();
}

bool WebAXObject::isAnchor() const
{
    if (isDetached())
        return false;

    return m_private->isAnchor();
}

WebAXOptionalBool WebAXObject::isAriaGrabbed() const
{
    if (isDetached())
        return WebAXOptionalBoolUndefined;

    return static_cast<WebAXOptionalBool>(m_private->isAriaGrabbed());
}

bool WebAXObject::isAriaReadOnly() const
{
    if (isDetached())
        return false;

    return equalIgnoringCase(m_private->getAttribute(HTMLNames::aria_readonlyAttr), "true");
}

WebString WebAXObject::ariaAutoComplete() const
{
    if (isDetached())
        return WebString();

    return m_private->ariaAutoComplete();
}

WebString WebAXObject::placeholder() const
{
    if (isDetached())
        return WebString();

    return WebString(m_private->placeholder());
}

bool WebAXObject::isButtonStateMixed() const
{
    if (isDetached())
        return false;

    return m_private->checkboxOrRadioValue() == ButtonStateMixed;
}

bool WebAXObject::isChecked() const
{
    if (isDetached())
        return false;

    return m_private->isChecked();
}

bool WebAXObject::isClickable() const
{
    if (isDetached())
        return false;

    return m_private->isClickable();
}

bool WebAXObject::isCollapsed() const
{
    if (isDetached())
        return false;

    return m_private->isCollapsed();
}

bool WebAXObject::isControl() const
{
    if (isDetached())
        return false;

    return m_private->isControl();
}

bool WebAXObject::isEnabled() const
{
    if (isDetached())
        return false;

    return m_private->isEnabled();
}

WebAXExpanded WebAXObject::isExpanded() const
{
    if (isDetached())
        return WebAXExpandedUndefined;

    return static_cast<WebAXExpanded>(m_private->isExpanded());
}

bool WebAXObject::isFocused() const
{
    if (isDetached())
        return false;

    return m_private->isFocused();
}

bool WebAXObject::isHovered() const
{
    if (isDetached())
        return false;

    return m_private->isHovered();
}

bool WebAXObject::isIndeterminate() const
{
    if (isDetached())
        return false;

    return m_private->isIndeterminate();
}

bool WebAXObject::isLinked() const
{
    if (isDetached())
        return false;

    return m_private->isLinked();
}

bool WebAXObject::isLoaded() const
{
    if (isDetached())
        return false;

    return m_private->isLoaded();
}

bool WebAXObject::isMultiSelectable() const
{
    if (isDetached())
        return false;

    return m_private->isMultiSelectable();
}

bool WebAXObject::isOffScreen() const
{
    if (isDetached())
        return false;

    return m_private->isOffScreen();
}

bool WebAXObject::isPasswordField() const
{
    if (isDetached())
        return false;

    return m_private->isPasswordField();
}

bool WebAXObject::isPressed() const
{
    if (isDetached())
        return false;

    return m_private->isPressed();
}

bool WebAXObject::isReadOnly() const
{
    if (isDetached())
        return false;

    return m_private->isReadOnly();
}

bool WebAXObject::isRequired() const
{
    if (isDetached())
        return false;

    return m_private->isRequired();
}

bool WebAXObject::isSelected() const
{
    if (isDetached())
        return false;

    return m_private->isSelected();
}

bool WebAXObject::isSelectedOptionActive() const
{
    if (isDetached())
        return false;

    return m_private->isSelectedOptionActive();
}

WebAXOrientation WebAXObject::orientation() const
{
    if (isDetached())
        return WebAXOrientationUndefined;

    return static_cast<WebAXOrientation>(m_private->orientation());
}

bool WebAXObject::isVisible() const
{
    if (isDetached())
        return false;

    return m_private->isVisible();
}

bool WebAXObject::isVisited() const
{
    if (isDetached())
        return false;

    return m_private->isVisited();
}

WebString WebAXObject::accessKey() const
{
    if (isDetached())
        return WebString();

    return WebString(m_private->accessKey());
}

WebAXObject WebAXObject::ariaActiveDescendant() const
{
    if (isDetached())
        return WebAXObject();

    return WebAXObject(m_private->activeDescendant());
}

bool WebAXObject::ariaControls(WebVector<WebAXObject>& controlsElements) const
{
    if (isDetached())
        return false;

    AXObject::AccessibilityChildrenVector controls;
    m_private->ariaControlsElements(controls);

    WebVector<WebAXObject> result(controls.size());
    for (size_t i = 0; i < controls.size(); i++)
        result[i] = WebAXObject(controls[i]);
    controlsElements.swap(result);

    return true;
}

bool WebAXObject::ariaDescribedby(WebVector<WebAXObject>& describedbyElements) const
{
    if (isDetached())
        return false;

    AXObject::AccessibilityChildrenVector describedby;
    m_private->ariaDescribedbyElements(describedby);

    WebVector<WebAXObject> result(describedby.size());
    for (size_t i = 0; i < describedby.size(); i++)
        result[i] = WebAXObject(describedby[i]);
    describedbyElements.swap(result);

    return true;
}

WebString WebAXObject::ariaDropEffect() const
{
    if (isDetached())
        return WebString();

    return WebString(m_private->ariaDropEffect());
}

bool WebAXObject::ariaHasPopup() const
{
    if (isDetached())
        return false;

    return m_private->ariaHasPopup();
}

bool WebAXObject::ariaFlowTo(WebVector<WebAXObject>& flowToElements) const
{
    if (isDetached())
        return false;

    AXObject::AccessibilityChildrenVector flowTo;
    m_private->ariaFlowToElements(flowTo);

    WebVector<WebAXObject> result(flowTo.size());
    for (size_t i = 0; i < flowTo.size(); i++)
        result[i] = WebAXObject(flowTo[i]);
    flowToElements.swap(result);

    return true;
}

bool WebAXObject::ariaLabelledby(WebVector<WebAXObject>& labelledbyElements) const
{
    if (isDetached())
        return false;

    AXObject::AccessibilityChildrenVector labelledby;
    m_private->ariaLabelledbyElements(labelledby);

    WebVector<WebAXObject> result(labelledby.size());
    for (size_t i = 0; i < labelledby.size(); i++)
        result[i] = WebAXObject(labelledby[i]);
    labelledbyElements.swap(result);

    return true;
}

bool WebAXObject::isInLiveRegion() const
{
    if (isDetached())
        return false;

    return 0 != m_private->liveRegionRoot();
}

bool WebAXObject::liveRegionAtomic() const
{
    if (isDetached())
        return false;

    return m_private->liveRegionAtomic();
}

bool WebAXObject::liveRegionBusy() const
{
    if (isDetached())
        return false;

    return m_private->liveRegionBusy();
}

WebString WebAXObject::liveRegionRelevant() const
{
    if (isDetached())
        return WebString();

    return m_private->liveRegionRelevant();
}

WebString WebAXObject::liveRegionStatus() const
{
    if (isDetached())
        return WebString();

    return m_private->liveRegionStatus();
}

bool WebAXObject::containerLiveRegionAtomic() const
{
    if (isDetached())
        return false;

    return m_private->containerLiveRegionAtomic();
}

bool WebAXObject::containerLiveRegionBusy() const
{
    if (isDetached())
        return false;

    return m_private->containerLiveRegionBusy();
}

WebString WebAXObject::containerLiveRegionRelevant() const
{
    if (isDetached())
        return WebString();

    return m_private->containerLiveRegionRelevant();
}

WebString WebAXObject::containerLiveRegionStatus() const
{
    if (isDetached())
        return WebString();

    return m_private->containerLiveRegionStatus();
}

bool WebAXObject::ariaOwns(WebVector<WebAXObject>& ownsElements) const
{
    if (isDetached())
        return false;

    AXObject::AccessibilityChildrenVector owns;
    m_private->ariaOwnsElements(owns);

    WebVector<WebAXObject> result(owns.size());
    for (size_t i = 0; i < owns.size(); i++)
        result[i] = WebAXObject(owns[i]);
    ownsElements.swap(result);

    return true;
}

WebRect WebAXObject::boundingBoxRect() const
{
    if (isDetached())
        return WebRect();

    ASSERT(isLayoutClean(m_private->document()));

    return pixelSnappedIntRect(m_private->elementRect());
}

bool WebAXObject::canvasHasFallbackContent() const
{
    if (isDetached())
        return false;

    return m_private->canvasHasFallbackContent();
}

WebPoint WebAXObject::clickPoint() const
{
    if (isDetached())
        return WebPoint();

    return WebPoint(m_private->clickPoint());
}

void WebAXObject::colorValue(int& r, int& g, int& b) const
{
    if (isDetached())
        return;

    m_private->colorValue(r, g, b);
}

WebAXInvalidState WebAXObject::invalidState() const
{
    if (isDetached())
        return WebAXInvalidStateUndefined;

    return static_cast<WebAXInvalidState>(m_private->invalidState());
}

// Only used when invalidState() returns WebAXInvalidStateOther.
WebString WebAXObject::ariaInvalidValue() const
{
    if (isDetached())
        return WebString();

    return m_private->ariaInvalidValue();
}

double WebAXObject::estimatedLoadingProgress() const
{
    if (isDetached())
        return 0.0;

    return m_private->estimatedLoadingProgress();
}

WebString WebAXObject::helpText() const
{
    if (isDetached())
        return WebString();

    return m_private->helpText();
}

int WebAXObject::headingLevel() const
{
    if (isDetached())
        return 0;

    return m_private->headingLevel();
}

int WebAXObject::hierarchicalLevel() const
{
    if (isDetached())
        return 0;

    return m_private->hierarchicalLevel();
}

// FIXME: This method passes in a point that has page scale applied but assumes that (0, 0)
// is the top left of the visual viewport. In other words, the point has the PinchViewport
// scale applied, but not the PinchViewport offset. crbug.com/459591.
WebAXObject WebAXObject::hitTest(const WebPoint& point) const
{
    if (isDetached())
        return WebAXObject();

    IntPoint contentsPoint = m_private->documentFrameView()->soonToBeRemovedUnscaledViewportToContents(point);
    RefPtr<AXObject> hit = m_private->accessibilityHitTest(contentsPoint);

    if (hit)
        return WebAXObject(hit);

    if (m_private->elementRect().contains(contentsPoint))
        return *this;

    return WebAXObject();
}

WebString WebAXObject::keyboardShortcut() const
{
    if (isDetached())
        return WebString();

    String accessKey = m_private->accessKey();
    if (accessKey.isNull())
        return WebString();

    DEFINE_STATIC_LOCAL(String, modifierString, ());
    if (modifierString.isNull()) {
        unsigned modifiers = EventHandler::accessKeyModifiers();
        // Follow the same order as Mozilla MSAA implementation:
        // Ctrl+Alt+Shift+Meta+key. MSDN states that keyboard shortcut strings
        // should not be localized and defines the separator as "+".
        StringBuilder modifierStringBuilder;
        if (modifiers & PlatformEvent::CtrlKey)
            modifierStringBuilder.appendLiteral("Ctrl+");
        if (modifiers & PlatformEvent::AltKey)
            modifierStringBuilder.appendLiteral("Alt+");
        if (modifiers & PlatformEvent::ShiftKey)
            modifierStringBuilder.appendLiteral("Shift+");
        if (modifiers & PlatformEvent::MetaKey)
            modifierStringBuilder.appendLiteral("Win+");
        modifierString = modifierStringBuilder.toString();
    }

    return String(modifierString + accessKey);
}

bool WebAXObject::performDefaultAction() const
{
    if (isDetached())
        return false;

    return m_private->performDefaultAction();
}

bool WebAXObject::increment() const
{
    if (isDetached())
        return false;

    if (canIncrement()) {
        m_private->increment();
        return true;
    }
    return false;
}

bool WebAXObject::decrement() const
{
    if (isDetached())
        return false;

    if (canDecrement()) {
        m_private->decrement();
        return true;
    }
    return false;
}

bool WebAXObject::press() const
{
    if (isDetached())
        return false;

    return m_private->press();
}

WebAXRole WebAXObject::role() const
{
    if (isDetached())
        return WebAXRoleUnknown;

    return static_cast<WebAXRole>(m_private->roleValue());
}

unsigned WebAXObject::selectionEnd() const
{
    if (isDetached())
        return 0;

    return m_private->selectedTextRange().start + m_private->selectedTextRange().length;
}

unsigned WebAXObject::selectionStart() const
{
    if (isDetached())
        return 0;

    return m_private->selectedTextRange().start;
}

unsigned WebAXObject::selectionEndLineNumber() const
{
    if (isDetached())
        return 0;

    VisiblePosition position = m_private->visiblePositionForIndex(selectionEnd());
    int lineNumber = m_private->lineForPosition(position);
    if (lineNumber < 0)
        return 0;
    return lineNumber;
}

unsigned WebAXObject::selectionStartLineNumber() const
{
    if (isDetached())
        return 0;

    VisiblePosition position = m_private->visiblePositionForIndex(selectionStart());
    int lineNumber = m_private->lineForPosition(position);
    if (lineNumber < 0)
        return 0;
    return lineNumber;
}

void WebAXObject::setFocused(bool on) const
{
    if (!isDetached())
        m_private->setFocused(on);
}

void WebAXObject::setSelectedTextRange(int selectionStart, int selectionEnd) const
{
    if (isDetached())
        return;

    m_private->setSelectedTextRange(AXObject::PlainTextRange(selectionStart, selectionEnd - selectionStart));
}

void WebAXObject::setValue(WebString value) const
{
    if (isDetached())
        return;

    m_private->setValue(value);
}

WebString WebAXObject::stringValue() const
{
    if (isDetached())
        return WebString();

    return m_private->stringValue();
}

WebString WebAXObject::title() const
{
    if (isDetached())
        return WebString();

    ASSERT(isLayoutClean(m_private->document()));

    return m_private->title();
}

WebString WebAXObject::language() const
{
    if (isDetached())
        return WebString();

    return m_private->language();
}

WebAXObject WebAXObject::titleUIElement() const
{
    if (isDetached())
        return WebAXObject();

    if (!m_private->exposesTitleUIElement())
        return WebAXObject();

    return WebAXObject(m_private->titleUIElement());
}

WebURL WebAXObject::url() const
{
    if (isDetached())
        return WebURL();

    return m_private->url();
}

bool WebAXObject::supportsRangeValue() const
{
    if (isDetached())
        return false;

    return m_private->supportsRangeValue();
}

WebString WebAXObject::valueDescription() const
{
    if (isDetached())
        return WebString();

    return m_private->valueDescription();
}

float WebAXObject::valueForRange() const
{
    if (isDetached())
        return 0.0;

    return m_private->valueForRange();
}

float WebAXObject::maxValueForRange() const
{
    if (isDetached())
        return 0.0;

    return m_private->maxValueForRange();
}

float WebAXObject::minValueForRange() const
{
    if (isDetached())
        return 0.0;

    return m_private->minValueForRange();
}

WebNode WebAXObject::node() const
{
    if (isDetached())
        return WebNode();

    Node* node = m_private->node();
    if (!node)
        return WebNode();

    return WebNode(node);
}

WebDocument WebAXObject::document() const
{
    if (isDetached())
        return WebDocument();

    Document* document = m_private->document();
    if (!document)
        return WebDocument();

    return WebDocument(document);
}

bool WebAXObject::hasComputedStyle() const
{
    if (isDetached())
        return false;

    Document* document = m_private->document();
    if (document)
        document->updateRenderTreeIfNeeded();

    Node* node = m_private->node();
    if (!node)
        return false;

    return node->ensureComputedStyle();
}

WebString WebAXObject::computedStyleDisplay() const
{
    if (isDetached())
        return WebString();

    Document* document = m_private->document();
    if (document)
        document->updateRenderTreeIfNeeded();

    Node* node = m_private->node();
    if (!node)
        return WebString();

    const ComputedStyle* computedStyle = node->ensureComputedStyle();
    if (!computedStyle)
        return WebString();

    return WebString(CSSPrimitiveValue::create(computedStyle->display())->getStringValue());
}

bool WebAXObject::accessibilityIsIgnored() const
{
    if (isDetached())
        return false;

    return m_private->accessibilityIsIgnored();
}

bool WebAXObject::lineBreaks(WebVector<int>& result) const
{
    if (isDetached())
        return false;

    Vector<int> lineBreaksVector;
    m_private->lineBreaks(lineBreaksVector);

    size_t vectorSize = lineBreaksVector.size();
    WebVector<int> lineBreaksWebVector(vectorSize);
    for (size_t i = 0; i< vectorSize; i++)
        lineBreaksWebVector[i] = lineBreaksVector[i];
    result.swap(lineBreaksWebVector);

    return true;
}

WebString WebAXObject::textInputType() const
{
    if (isDetached())
        return WebString();

    return WebString(m_private->textInputType());
}

unsigned WebAXObject::columnCount() const
{
    if (isDetached())
        return false;

    if (!m_private->isAXTable())
        return 0;

    return toAXTable(m_private.get())->columnCount();
}

unsigned WebAXObject::rowCount() const
{
    if (isDetached())
        return false;

    if (!m_private->isAXTable())
        return 0;

    return toAXTable(m_private.get())->rowCount();
}

WebAXObject WebAXObject::cellForColumnAndRow(unsigned column, unsigned row) const
{
    if (isDetached())
        return WebAXObject();

    if (!m_private->isAXTable())
        return WebAXObject();

    AXTableCell* cell = toAXTable(m_private.get())->cellForColumnAndRow(column, row);
    return WebAXObject(static_cast<AXObject*>(cell));
}

WebAXObject WebAXObject::headerContainerObject() const
{
    if (isDetached())
        return WebAXObject();

    if (!m_private->isAXTable())
        return WebAXObject();

    return WebAXObject(toAXTable(m_private.get())->headerContainer());
}

WebAXObject WebAXObject::rowAtIndex(unsigned rowIndex) const
{
    if (isDetached())
        return WebAXObject();

    if (!m_private->isAXTable())
        return WebAXObject();

    const AXObject::AccessibilityChildrenVector& rows = toAXTable(m_private.get())->rows();
    if (rowIndex < rows.size())
        return WebAXObject(rows[rowIndex]);

    return WebAXObject();
}

WebAXObject WebAXObject::columnAtIndex(unsigned columnIndex) const
{
    if (isDetached())
        return WebAXObject();

    if (!m_private->isAXTable())
        return WebAXObject();

    const AXObject::AccessibilityChildrenVector& columns = toAXTable(m_private.get())->columns();
    if (columnIndex < columns.size())
        return WebAXObject(columns[columnIndex]);

    return WebAXObject();
}

unsigned WebAXObject::rowIndex() const
{
    if (isDetached())
        return 0;

    if (!m_private->isTableRow())
        return 0;

    return toAXTableRow(m_private.get())->rowIndex();
}

WebAXObject WebAXObject::rowHeader() const
{
    if (isDetached())
        return WebAXObject();

    if (!m_private->isTableRow())
        return WebAXObject();

    return WebAXObject(toAXTableRow(m_private.get())->headerObject());
}

void WebAXObject::rowHeaders(WebVector<WebAXObject>& rowHeaderElements) const
{
    if (isDetached())
        return;

    if (!m_private->isAXTable())
        return;

    AXObject::AccessibilityChildrenVector headers;
    toAXTable(m_private.get())->rowHeaders(headers);

    size_t headerCount = headers.size();
    WebVector<WebAXObject> result(headerCount);

    for (size_t i = 0; i < headerCount; i++)
        result[i] = WebAXObject(headers[i]);

    rowHeaderElements.swap(result);
}

unsigned WebAXObject::columnIndex() const
{
    if (isDetached())
        return 0;

    if (m_private->roleValue() != ColumnRole)
        return 0;

    return toAXTableColumn(m_private.get())->columnIndex();
}

WebAXObject WebAXObject::columnHeader() const
{
    if (isDetached())
        return WebAXObject();

    if (m_private->roleValue() != ColumnRole)
        return WebAXObject();

    return WebAXObject(toAXTableColumn(m_private.get())->headerObject());
}

void WebAXObject::columnHeaders(WebVector<WebAXObject>& columnHeaderElements) const
{
    if (isDetached())
        return;

    if (!m_private->isAXTable())
        return;

    AXObject::AccessibilityChildrenVector headers;
    toAXTable(m_private.get())->columnHeaders(headers);

    size_t headerCount = headers.size();
    WebVector<WebAXObject> result(headerCount);

    for (size_t i = 0; i < headerCount; i++)
        result[i] = WebAXObject(headers[i]);

    columnHeaderElements.swap(result);
}

unsigned WebAXObject::cellColumnIndex() const
{
    if (isDetached())
        return 0;

    if (!m_private->isTableCell())
        return 0;

    pair<unsigned, unsigned> columnRange;
    toAXTableCell(m_private.get())->columnIndexRange(columnRange);
    return columnRange.first;
}

unsigned WebAXObject::cellColumnSpan() const
{
    if (isDetached())
        return 0;

    if (!m_private->isTableCell())
        return 0;

    pair<unsigned, unsigned> columnRange;
    toAXTableCell(m_private.get())->columnIndexRange(columnRange);
    return columnRange.second;
}

unsigned WebAXObject::cellRowIndex() const
{
    if (isDetached())
        return 0;

    if (!m_private->isTableCell())
        return 0;

    pair<unsigned, unsigned> rowRange;
    toAXTableCell(m_private.get())->rowIndexRange(rowRange);
    return rowRange.first;
}

unsigned WebAXObject::cellRowSpan() const
{
    if (isDetached())
        return 0;

    if (!m_private->isTableCell())
        return 0;

    pair<unsigned, unsigned> rowRange;
    toAXTableCell(m_private.get())->rowIndexRange(rowRange);
    return rowRange.second;
}

WebAXSortDirection WebAXObject::sortDirection() const
{
    if (isDetached())
        return WebAXSortDirectionUndefined;

    return static_cast<WebAXSortDirection>(m_private->sortDirection());
}

void WebAXObject::loadInlineTextBoxes() const
{
    if (isDetached())
        return;

    m_private->loadInlineTextBoxes();
}

WebAXTextDirection WebAXObject::textDirection() const
{
    if (isDetached())
        return WebAXTextDirectionLR;

    return static_cast<WebAXTextDirection>(m_private->textDirection());
}

void WebAXObject::characterOffsets(WebVector<int>& offsets) const
{
    if (isDetached())
        return;

    Vector<int> offsetsVector;
    m_private->textCharacterOffsets(offsetsVector);

    size_t vectorSize = offsetsVector.size();
    WebVector<int> offsetsWebVector(vectorSize);
    for (size_t i = 0; i < vectorSize; i++)
        offsetsWebVector[i] = offsetsVector[i];
    offsets.swap(offsetsWebVector);
}

void WebAXObject::wordBoundaries(WebVector<int>& starts, WebVector<int>& ends) const
{
    if (isDetached())
        return;

    Vector<AXObject::PlainTextRange> words;
    m_private->wordBoundaries(words);

    WebVector<int> startsWebVector(words.size());
    WebVector<int> endsWebVector(words.size());
    for (size_t i = 0; i < words.size(); i++) {
        startsWebVector[i] = words[i].start;
        endsWebVector[i] = words[i].start + words[i].length;
    }
    starts.swap(startsWebVector);
    ends.swap(endsWebVector);
}

void WebAXObject::scrollToMakeVisible() const
{
    if (!isDetached())
        m_private->scrollToMakeVisible();
}

void WebAXObject::scrollToMakeVisibleWithSubFocus(const WebRect& subfocus) const
{
    if (!isDetached())
        m_private->scrollToMakeVisibleWithSubFocus(subfocus);
}

void WebAXObject::scrollToGlobalPoint(const WebPoint& point) const
{
    if (!isDetached())
        m_private->scrollToGlobalPoint(point);
}

WebAXObject::WebAXObject(const WTF::PassRefPtr<AXObject>& object)
    : m_private(object)
{
}

WebAXObject& WebAXObject::operator=(const WTF::PassRefPtr<AXObject>& object)
{
    m_private = object;
    return *this;
}

WebAXObject::operator WTF::PassRefPtr<AXObject>() const
{
    return m_private.get();
}

} // namespace blink
