/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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
#include "core/dom/Text.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/SVGNames.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/LayoutTreeBuilder.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/dom/NodeRenderingTraversal.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/events/ScopedEventQueue.h"
#include "core/layout/LayoutText.h"
#include "core/layout/LayoutTextCombine.h"
#include "core/layout/svg/LayoutSVGInlineText.h"
#include "core/svg/SVGForeignObjectElement.h"
#include "wtf/text/CString.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

#if ENABLE(OILPAN)
namespace {
// If the external string kept by a Text node exceed this threshold length,
// Oilpan is informed. External allocation amounts owned by heap objects are
// taken into account when scheduling urgent Oilpan GCs.
//
// FIXME: having only Text nodes with strings above an ad-hoc local threshold
// influence Oilpan's GC behavior isn't a satisfactory long-term solution.
// But code that allocates a lot of Text nodes in tight loops, and not much more,
// we do have to trigger Oilpan GCs to avoid PartitionAlloc OOMs. The accounting
// does add overhead on the allocation of every Text node however, so for now, only
// register those above the given threshold. TBC.
const size_t stringLengthThreshold = 256;

void increaseExternallyAllocatedBytesIfNeeded(size_t length)
{
    if (length > stringLengthThreshold)
        Heap::increaseExternallyAllocatedBytes(length);
}

void increaseExternallyAllocatedBytesAliveIfNeeded(size_t length)
{
    if (length > stringLengthThreshold)
        Heap::increaseExternallyAllocatedBytesAlive(length);
}

} // namespace
#endif

PassRefPtrWillBeRawPtr<Text> Text::create(Document& document, const String& data)
{
#if ENABLE(OILPAN)
    increaseExternallyAllocatedBytesIfNeeded(data.length());
#endif
    return adoptRefWillBeNoop(new Text(document, data, CreateText));
}

PassRefPtrWillBeRawPtr<Text> Text::createEditingText(Document& document, const String& data)
{
#if ENABLE(OILPAN)
    increaseExternallyAllocatedBytesIfNeeded(data.length());
#endif
    return adoptRefWillBeNoop(new Text(document, data, CreateEditingText));
}

PassRefPtrWillBeRawPtr<Node> Text::mergeNextSiblingNodesIfPossible()
{
    RefPtrWillBeRawPtr<Node> protect(this);

    // Remove empty text nodes.
    if (!length()) {
        // Care must be taken to get the next node before removing the current node.
        RefPtrWillBeRawPtr<Node> nextNode(NodeTraversal::nextPostOrder(*this));
        remove(IGNORE_EXCEPTION);
        return nextNode.release();
    }

    // Merge text nodes.
    while (Node* nextSibling = this->nextSibling()) {
        if (nextSibling->nodeType() != TEXT_NODE)
            break;

        RefPtrWillBeRawPtr<Text> nextText = toText(nextSibling);

        // Remove empty text nodes.
        if (!nextText->length()) {
            nextText->remove(IGNORE_EXCEPTION);
            continue;
        }

        // Both non-empty text nodes. Merge them.
        unsigned offset = length();
        String nextTextData = nextText->data();
        String oldTextData = data();
        setDataWithoutUpdate(data() + nextTextData);
        updateTextRenderer(oldTextData.length(), 0);

        // Empty nextText for layout update.
        nextText->setDataWithoutUpdate(emptyString());
        nextText->updateTextRenderer(0, nextTextData.length());

        document().didMergeTextNodes(*nextText, offset);

        // Restore nextText for mutation event.
        nextText->setDataWithoutUpdate(nextTextData);
        nextText->updateTextRenderer(0, 0);

        document().incDOMTreeVersion();
        didModifyData(oldTextData, CharacterData::UpdateFromNonParser);
        nextText->remove(IGNORE_EXCEPTION);
    }

    return NodeTraversal::nextPostOrder(*this);
}

PassRefPtrWillBeRawPtr<Text> Text::splitText(unsigned offset, ExceptionState& exceptionState)
{
    // IndexSizeError: Raised if the specified offset is negative or greater than
    // the number of 16-bit units in data.
    if (offset > length()) {
        exceptionState.throwDOMException(IndexSizeError, "The offset " + String::number(offset) + " is larger than the Text node's length.");
        return nullptr;
    }

    EventQueueScope scope;
    String oldStr = data();
    RefPtrWillBeRawPtr<Text> newText = cloneWithData(oldStr.substring(offset));
    setDataWithoutUpdate(oldStr.substring(0, offset));

    didModifyData(oldStr, CharacterData::UpdateFromNonParser);

    if (parentNode())
        parentNode()->insertBefore(newText.get(), nextSibling(), exceptionState);
    if (exceptionState.hadException())
        return nullptr;

    if (layoutObject())
        layoutObject()->setTextWithOffset(dataImpl(), 0, oldStr.length());

    if (parentNode())
        document().didSplitTextNode(*this);

    return newText.release();
}

static const Text* earliestLogicallyAdjacentTextNode(const Text* t)
{
    for (const Node* n = t->previousSibling(); n; n = n->previousSibling()) {
        Node::NodeType type = n->nodeType();
        if (type == Node::TEXT_NODE || type == Node::CDATA_SECTION_NODE) {
            t = toText(n);
            continue;
        }

        break;
    }
    return t;
}

static const Text* latestLogicallyAdjacentTextNode(const Text* t)
{
    for (const Node* n = t->nextSibling(); n; n = n->nextSibling()) {
        Node::NodeType type = n->nodeType();
        if (type == Node::TEXT_NODE || type == Node::CDATA_SECTION_NODE) {
            t = toText(n);
            continue;
        }

        break;
    }
    return t;
}

String Text::wholeText() const
{
    const Text* startText = earliestLogicallyAdjacentTextNode(this);
    const Text* endText = latestLogicallyAdjacentTextNode(this);

    Node* onePastEndText = endText->nextSibling();
    unsigned resultLength = 0;
    for (const Node* n = startText; n != onePastEndText; n = n->nextSibling()) {
        if (!n->isTextNode())
            continue;
        const String& data = toText(n)->data();
        if (std::numeric_limits<unsigned>::max() - data.length() < resultLength)
            CRASH();
        resultLength += data.length();
    }
    StringBuilder result;
    result.reserveCapacity(resultLength);
    for (const Node* n = startText; n != onePastEndText; n = n->nextSibling()) {
        if (!n->isTextNode())
            continue;
        result.append(toText(n)->data());
    }
    ASSERT(result.length() == resultLength);

    return result.toString();
}

PassRefPtrWillBeRawPtr<Text> Text::replaceWholeText(const String& newText)
{
    // Remove all adjacent text nodes, and replace the contents of this one.

    // Protect startText and endText against mutation event handlers removing the last ref
    RefPtrWillBeRawPtr<Text> startText = const_cast<Text*>(earliestLogicallyAdjacentTextNode(this));
    RefPtrWillBeRawPtr<Text> endText = const_cast<Text*>(latestLogicallyAdjacentTextNode(this));

    RefPtrWillBeRawPtr<Text> protectedThis(this); // Mutation event handlers could cause our last ref to go away
    RefPtrWillBeRawPtr<ContainerNode> parent = parentNode(); // Protect against mutation handlers moving this node during traversal
    for (RefPtrWillBeRawPtr<Node> n = startText; n && n != this && n->isTextNode() && n->parentNode() == parent;) {
        RefPtrWillBeRawPtr<Node> nodeToRemove(n.release());
        n = nodeToRemove->nextSibling();
        parent->removeChild(nodeToRemove.get(), IGNORE_EXCEPTION);
    }

    if (this != endText) {
        Node* onePastEndText = endText->nextSibling();
        for (RefPtrWillBeRawPtr<Node> n = nextSibling(); n && n != onePastEndText && n->isTextNode() && n->parentNode() == parent;) {
            RefPtrWillBeRawPtr<Node> nodeToRemove(n.release());
            n = nodeToRemove->nextSibling();
            parent->removeChild(nodeToRemove.get(), IGNORE_EXCEPTION);
        }
    }

    if (newText.isEmpty()) {
        if (parent && parentNode() == parent)
            parent->removeChild(this, IGNORE_EXCEPTION);
        return nullptr;
    }

    setData(newText);
    return protectedThis.release();
}

String Text::nodeName() const
{
    return "#text";
}

Node::NodeType Text::nodeType() const
{
    return TEXT_NODE;
}

PassRefPtrWillBeRawPtr<Node> Text::cloneNode(bool /*deep*/)
{
    return cloneWithData(data());
}

static inline bool canHaveWhitespaceChildren(const LayoutObject& parent)
{
    // <button> should allow whitespace even though LayoutFlexibleBox doesn't.
    if (parent.isLayoutButton())
        return true;

    if (parent.isTable() || parent.isTableRow() || parent.isTableSection()
        || parent.isLayoutTableCol() || parent.isFrameSet()
        || parent.isFlexibleBox() || parent.isLayoutGrid()
        || parent.isSVGRoot()
        || parent.isSVGContainer()
        || parent.isSVGImage()
        || parent.isSVGShape())
        return false;
    return true;
}

bool Text::textRendererIsNeeded(const ComputedStyle& style, const LayoutObject& parent)
{
    if (!parent.canHaveChildren())
        return false;

    if (isEditingText())
        return true;

    if (!length())
        return false;

    if (style.display() == NONE)
        return false;

    if (!containsOnlyWhitespace())
        return true;

    if (!canHaveWhitespaceChildren(parent))
        return false;

    if (style.preserveNewline()) // pre/pre-wrap/pre-line always make renderers.
        return true;

    // childNeedsDistributionRecalc() here is rare, only happens JS calling surroundContents() etc. from DOMNodeInsertedIntoDocument etc.
    if (document().childNeedsDistributionRecalc())
        return true;

    const LayoutObject* prev = NodeRenderingTraversal::previousSiblingRenderer(*this);
    if (prev && prev->isBR()) // <span><br/> <br/></span>
        return false;

    if (parent.isLayoutInline()) {
        // <span><div/> <div/></span>
        if (prev && !prev->isInline())
            return false;
    } else {
        if (parent.isLayoutBlock() && !parent.childrenInline() && (!prev || !prev->isInline()))
            return false;

        // Avoiding creation of a Renderer for the text node is a non-essential memory optimization.
        // So to avoid blowing up on very wide DOMs, we limit the number of siblings to visit.
        unsigned maxSiblingsToVisit = 50;

        LayoutObject* first = parent.slowFirstChild();
        while (first && first->isFloatingOrOutOfFlowPositioned() && maxSiblingsToVisit--)
            first = first->nextSibling();
        if (!first || first == layoutObject() || NodeRenderingTraversal::nextSiblingRenderer(*this) == first)
            // Whitespace at the start of a block just goes away.  Don't even
            // make a render object for this text.
            return false;
    }
    return true;
}

static bool isSVGText(Text* text)
{
    Node* parentOrShadowHostNode = text->parentOrShadowHostNode();
    ASSERT(parentOrShadowHostNode);
    return parentOrShadowHostNode->isSVGElement() && !isSVGForeignObjectElement(*parentOrShadowHostNode);
}

LayoutText* Text::createTextRenderer(const ComputedStyle& style)
{
    if (isSVGText(this))
        return new LayoutSVGInlineText(this, dataImpl());

    if (style.hasTextCombine())
        return new LayoutTextCombine(this, dataImpl());

    return new LayoutText(this, dataImpl());
}

void Text::attach(const AttachContext& context)
{
    if (ContainerNode* renderingParent = NodeRenderingTraversal::parent(*this)) {
        if (LayoutObject* parentRenderer = renderingParent->layoutObject()) {
            if (textRendererIsNeeded(*parentRenderer->style(), *parentRenderer))
                LayoutTreeBuilderForText(*this, parentRenderer).createLayoutObject();
        }
    }
    CharacterData::attach(context);
}

void Text::reattachIfNeeded(const AttachContext& context)
{
    bool layoutObjectIsNeeded = false;
    ContainerNode* renderingParent = NodeRenderingTraversal::parent(*this);
    if (renderingParent) {
        if (LayoutObject* parentRenderer = renderingParent->layoutObject()) {
            if (textRendererIsNeeded(*parentRenderer->style(), *parentRenderer))
                layoutObjectIsNeeded = true;
        }
    }

    if (layoutObjectIsNeeded == !!layoutObject())
        return;

    // The following is almost the same as Node::reattach() except that we create renderer only if needed.
    // Not calling reattach() to avoid repeated calls to Text::textRendererIsNeeded().
    AttachContext reattachContext(context);
    reattachContext.performingReattach = true;

    if (styleChangeType() < NeedsReattachStyleChange)
        detach(reattachContext);
    if (layoutObjectIsNeeded)
        LayoutTreeBuilderForText(*this, renderingParent->layoutObject()).createLayoutObject();
    CharacterData::attach(reattachContext);
}

void Text::recalcTextStyle(StyleRecalcChange change, Text* nextTextSibling)
{
    if (LayoutText* renderer = this->layoutObject()) {
        if (change != NoChange || needsStyleRecalc())
            renderer->setStyle(document().ensureStyleResolver().styleForText(this));
        if (needsStyleRecalc())
            renderer->setText(dataImpl());
        clearNeedsStyleRecalc();
    } else if (needsStyleRecalc() || needsWhitespaceRenderer()) {
        reattach();
        if (this->layoutObject())
            reattachWhitespaceSiblingsIfNeeded(nextTextSibling);
    }
}

// If a whitespace node had no renderer and goes through a recalcStyle it may
// need to create one if the parent style now has white-space: pre.
bool Text::needsWhitespaceRenderer()
{
    ASSERT(!layoutObject());
    if (const ComputedStyle* style = parentComputedStyle())
        return style->preserveNewline();
    return false;
}

void Text::updateTextRenderer(unsigned offsetOfReplacedData, unsigned lengthOfReplacedData, RecalcStyleBehavior recalcStyleBehavior)
{
    if (!inActiveDocument())
        return;
    LayoutText* textRenderer = layoutObject();
    if (!textRenderer || !textRendererIsNeeded(*textRenderer->style(), *textRenderer->parent())) {
        lazyReattachIfAttached();
        // FIXME: Editing should be updated so this is not neccesary.
        if (recalcStyleBehavior == DeprecatedRecalcStyleImmediatlelyForEditing)
            document().updateRenderTreeIfNeeded();
        return;
    }
    textRenderer->setTextWithOffset(dataImpl(), offsetOfReplacedData, lengthOfReplacedData);
}

PassRefPtrWillBeRawPtr<Text> Text::cloneWithData(const String& data)
{
    return create(document(), data);
}

DEFINE_TRACE(Text)
{
#if ENABLE(OILPAN)
    increaseExternallyAllocatedBytesAliveIfNeeded(m_data.length());
#endif
    CharacterData::trace(visitor);
}

#ifndef NDEBUG
void Text::formatForDebugger(char *buffer, unsigned length) const
{
    StringBuilder result;
    String s;

    result.append(nodeName());

    s = data();
    if (s.length() > 0) {
        if (result.length())
            result.appendLiteral("; ");
        result.appendLiteral("value=");
        result.append(s);
    }

    strncpy(buffer, result.toString().utf8().data(), length - 1);
}
#endif

} // namespace blink
