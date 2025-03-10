/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2008, 2009, 2010, 2011 Google Inc. All rights reserved.
 * Copyright (C) 2011 Igalia S.L.
 * Copyright (C) 2011 Motorola Mobility. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/editing/markup.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/CSSPropertyNames.h"
#include "core/CSSValueKeywords.h"
#include "core/HTMLNames.h"
#include "core/css/CSSPrimitiveValue.h"
#include "core/css/CSSValue.h"
#include "core/css/StylePropertySet.h"
#include "core/dom/CDATASection.h"
#include "core/dom/ChildListMutationScope.h"
#include "core/dom/Comment.h"
#include "core/dom/ContextFeatures.h"
#include "core/dom/DocumentFragment.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/Range.h"
#include "core/editing/Editor.h"
#include "core/editing/StyledMarkupAccumulator.h"
#include "core/editing/VisibleSelection.h"
#include "core/editing/VisibleUnits.h"
#include "core/editing/htmlediting.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLAnchorElement.h"
#include "core/html/HTMLBRElement.h"
#include "core/html/HTMLBodyElement.h"
#include "core/html/HTMLDivElement.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLQuoteElement.h"
#include "core/html/HTMLSpanElement.h"
#include "core/html/HTMLTableCellElement.h"
#include "core/html/HTMLTableElement.h"
#include "core/html/HTMLTextFormControlElement.h"
#include "core/layout/LayoutObject.h"
#include "platform/weborigin/KURL.h"
#include "wtf/StdLibExtras.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

using namespace HTMLNames;

class AttributeChange {
    ALLOW_ONLY_INLINE_ALLOCATION();
public:
    AttributeChange()
        : m_name(nullAtom, nullAtom, nullAtom)
    {
    }

    AttributeChange(PassRefPtrWillBeRawPtr<Element> element, const QualifiedName& name, const String& value)
        : m_element(element), m_name(name), m_value(value)
    {
    }

    void apply()
    {
        m_element->setAttribute(m_name, AtomicString(m_value));
    }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_element);
    }

private:
    RefPtrWillBeMember<Element> m_element;
    QualifiedName m_name;
    String m_value;
};

} // namespace blink

WTF_ALLOW_INIT_WITH_MEM_FUNCTIONS(blink::AttributeChange);

namespace blink {

static void completeURLs(DocumentFragment& fragment, const String& baseURL)
{
    WillBeHeapVector<AttributeChange> changes;

    KURL parsedBaseURL(ParsedURLString, baseURL);

    for (Element& element : ElementTraversal::descendantsOf(fragment)) {
        AttributeCollection attributes = element.attributes();
        // AttributeCollection::iterator end = attributes.end();
        for (const auto& attribute : attributes) {
            if (element.isURLAttribute(attribute) && !attribute.value().isEmpty())
                changes.append(AttributeChange(&element, attribute.name(), KURL(parsedBaseURL, attribute.value()).string()));
        }
    }

    for (auto& change : changes)
        change.apply();
}

static bool isHTMLBlockElement(const Node* node)
{
    ASSERT(node);
    return isHTMLTableCellElement(*node)
        || isNonTableCellHTMLBlockElement(node);
}

static HTMLElement* ancestorToRetainStructureAndAppearanceForBlock(Element* commonAncestorBlock)
{
    if (!commonAncestorBlock)
        return 0;

    if (commonAncestorBlock->hasTagName(tbodyTag) || isHTMLTableRowElement(*commonAncestorBlock))
        return Traversal<HTMLTableElement>::firstAncestor(*commonAncestorBlock);

    if (isNonTableCellHTMLBlockElement(commonAncestorBlock))
        return toHTMLElement(commonAncestorBlock);

    return 0;
}

static inline HTMLElement* ancestorToRetainStructureAndAppearance(Node* commonAncestor)
{
    return ancestorToRetainStructureAndAppearanceForBlock(enclosingBlock(commonAncestor));
}

static inline HTMLElement* ancestorToRetainStructureAndAppearanceWithNoRenderer(Node* commonAncestor)
{
    HTMLElement* commonAncestorBlock = toHTMLElement(enclosingNodeOfType(firstPositionInOrBeforeNode(commonAncestor), isHTMLBlockElement));
    return ancestorToRetainStructureAndAppearanceForBlock(commonAncestorBlock);
}

bool propertyMissingOrEqualToNone(StylePropertySet* style, CSSPropertyID propertyID)
{
    if (!style)
        return false;
    RefPtrWillBeRawPtr<CSSValue> value = style->getPropertyCSSValue(propertyID);
    if (!value)
        return true;
    if (!value->isPrimitiveValue())
        return false;
    return toCSSPrimitiveValue(value.get())->getValueID() == CSSValueNone;
}

static bool needInterchangeNewlineAfter(const VisiblePosition& v)
{
    VisiblePosition next = v.next();
    Node* upstreamNode = next.deepEquivalent().upstream().deprecatedNode();
    Node* downstreamNode = v.deepEquivalent().downstream().deprecatedNode();
    // Add an interchange newline if a paragraph break is selected and a br won't already be added to the markup to represent it.
    return isEndOfParagraph(v) && isStartOfParagraph(next) && !(isHTMLBRElement(*upstreamNode) && upstreamNode == downstreamNode);
}

static PassRefPtrWillBeRawPtr<EditingStyle> styleFromMatchedRulesAndInlineDecl(const HTMLElement* element)
{
    RefPtrWillBeRawPtr<EditingStyle> style = EditingStyle::create(element->inlineStyle());
    // FIXME: Having to const_cast here is ugly, but it is quite a bit of work to untangle
    // the non-const-ness of styleFromMatchedRulesForElement.
    style->mergeStyleFromRules(const_cast<HTMLElement*>(element));
    return style.release();
}

static bool isPresentationalHTMLElement(const Node* node)
{
    if (!node->isHTMLElement())
        return false;

    const HTMLElement& element = toHTMLElement(*node);
    return element.hasTagName(uTag) || element.hasTagName(sTag) || element.hasTagName(strikeTag)
        || element.hasTagName(iTag) || element.hasTagName(emTag) || element.hasTagName(bTag) || element.hasTagName(strongTag);
}

static HTMLElement* highestAncestorToWrapMarkup(const Range* range, EAnnotateForInterchange shouldAnnotate, Node* constrainingAncestor)
{
    Node* commonAncestor = range->commonAncestorContainer();
    ASSERT(commonAncestor);
    HTMLElement* specialCommonAncestor = nullptr;
    if (shouldAnnotate == AnnotateForInterchange) {
        // Include ancestors that aren't completely inside the range but are required to retain
        // the structure and appearance of the copied markup.
        specialCommonAncestor = ancestorToRetainStructureAndAppearance(commonAncestor);

        if (Node* parentListNode = enclosingNodeOfType(firstPositionInOrBeforeNode(range->firstNode()), isListItem)) {
            if (blink::areRangesEqual(VisibleSelection::selectionFromContentsOfNode(parentListNode).toNormalizedRange().get(), range)) {
                ContainerNode* ancestor = parentListNode->parentNode();
                while (ancestor && !isHTMLListElement(ancestor))
                    ancestor = ancestor->parentNode();
                specialCommonAncestor = toHTMLElement(ancestor);
            }
        }

        // Retain the Mail quote level by including all ancestor mail block quotes.
        if (HTMLQuoteElement* highestMailBlockquote = toHTMLQuoteElement(highestEnclosingNodeOfType(firstPositionInOrBeforeNode(range->firstNode()), isMailHTMLBlockquoteElement, CanCrossEditingBoundary)))
            specialCommonAncestor = highestMailBlockquote;
    }

    Node* checkAncestor = specialCommonAncestor ? specialCommonAncestor : commonAncestor;
    if (checkAncestor->layoutObject()) {
        HTMLElement* newSpecialCommonAncestor = toHTMLElement(highestEnclosingNodeOfType(firstPositionInNode(checkAncestor), &isPresentationalHTMLElement, CanCrossEditingBoundary, constrainingAncestor));
        if (newSpecialCommonAncestor)
            specialCommonAncestor = newSpecialCommonAncestor;
    }

    // If a single tab is selected, commonAncestor will be a text node inside a tab span.
    // If two or more tabs are selected, commonAncestor will be the tab span.
    // In either case, if there is a specialCommonAncestor already, it will necessarily be above
    // any tab span that needs to be included.
    if (!specialCommonAncestor && isTabHTMLSpanElementTextNode(commonAncestor))
        specialCommonAncestor = toHTMLSpanElement(commonAncestor->parentNode());
    if (!specialCommonAncestor && isTabHTMLSpanElement(commonAncestor))
        specialCommonAncestor = toHTMLSpanElement(commonAncestor);

    if (HTMLAnchorElement* enclosingAnchor = toHTMLAnchorElement(enclosingElementWithTag(firstPositionInNode(specialCommonAncestor ? specialCommonAncestor : commonAncestor), aTag)))
        specialCommonAncestor = enclosingAnchor;

    return specialCommonAncestor;
}

// FIXME: Shouldn't we omit style info when annotate == DoNotAnnotateForInterchange?
// FIXME: At least, annotation and style info should probably not be included in range.markupString()
static String createMarkupInternal(Document& document, const Range* range, const Range* updatedRange,
    EAnnotateForInterchange shouldAnnotate, bool convertBlocksToInlines, EAbsoluteURLs shouldResolveURLs, Node* constrainingAncestor)
{
    ASSERT(range);
    ASSERT(updatedRange);
    DEFINE_STATIC_LOCAL(const String, interchangeNewlineString, ("<br class=\"" AppleInterchangeNewline "\">"));

    bool collapsed = updatedRange->collapsed();
    if (collapsed)
        return emptyString();
    Node* commonAncestor = updatedRange->commonAncestorContainer();
    if (!commonAncestor)
        return emptyString();

    document.updateLayoutIgnorePendingStylesheets();

    HTMLBodyElement* body = toHTMLBodyElement(enclosingElementWithTag(firstPositionInNode(commonAncestor), bodyTag));
    HTMLBodyElement* fullySelectedRoot = nullptr;
    // FIXME: Do this for all fully selected blocks, not just the body.
    if (body && areRangesEqual(VisibleSelection::selectionFromContentsOfNode(body).toNormalizedRange().get(), range))
        fullySelectedRoot = body;
    HTMLElement* specialCommonAncestor = highestAncestorToWrapMarkup(updatedRange, shouldAnnotate, constrainingAncestor);
    StyledMarkupAccumulator accumulator(shouldResolveURLs, shouldAnnotate, updatedRange->startPosition(), updatedRange->endPosition(), specialCommonAncestor);
    Node* pastEnd = updatedRange->pastLastNode();

    Node* startNode = updatedRange->firstNode();
    VisiblePosition visibleStart(updatedRange->startPosition(), VP_DEFAULT_AFFINITY);
    VisiblePosition visibleEnd(updatedRange->endPosition(), VP_DEFAULT_AFFINITY);
    if (shouldAnnotate == AnnotateForInterchange && needInterchangeNewlineAfter(visibleStart)) {
        if (visibleStart == visibleEnd.previous())
            return interchangeNewlineString;

        accumulator.appendString(interchangeNewlineString);
        startNode = visibleStart.next().deepEquivalent().deprecatedNode();

        if (pastEnd && Range::compareBoundaryPoints(startNode, 0, pastEnd, 0, ASSERT_NO_EXCEPTION) >= 0)
            return interchangeNewlineString;
    }

    Node* lastClosed = accumulator.serializeNodes(startNode, pastEnd);

    if (specialCommonAncestor && lastClosed) {
        // Also include all of the ancestors of lastClosed up to this special ancestor.
        for (ContainerNode* ancestor = lastClosed->parentNode(); ancestor; ancestor = ancestor->parentNode()) {
            if (ancestor == fullySelectedRoot && !convertBlocksToInlines) {
                RefPtrWillBeRawPtr<EditingStyle> fullySelectedRootStyle = styleFromMatchedRulesAndInlineDecl(fullySelectedRoot);

                // Bring the background attribute over, but not as an attribute because a background attribute on a div
                // appears to have no effect.
                if ((!fullySelectedRootStyle || !fullySelectedRootStyle->style() || !fullySelectedRootStyle->style()->getPropertyCSSValue(CSSPropertyBackgroundImage))
                    && fullySelectedRoot->hasAttribute(backgroundAttr))
                    fullySelectedRootStyle->style()->setProperty(CSSPropertyBackgroundImage, "url('" + fullySelectedRoot->getAttribute(backgroundAttr) + "')");

                if (fullySelectedRootStyle->style()) {
                    // Reset the CSS properties to avoid an assertion error in addStyleMarkup().
                    // This assertion is caused at least when we select all text of a <body> element whose
                    // 'text-decoration' property is "inherit", and copy it.
                    if (!propertyMissingOrEqualToNone(fullySelectedRootStyle->style(), CSSPropertyTextDecoration))
                        fullySelectedRootStyle->style()->setProperty(CSSPropertyTextDecoration, CSSValueNone);
                    if (!propertyMissingOrEqualToNone(fullySelectedRootStyle->style(), CSSPropertyWebkitTextDecorationsInEffect))
                        fullySelectedRootStyle->style()->setProperty(CSSPropertyWebkitTextDecorationsInEffect, CSSValueNone);
                    accumulator.wrapWithStyleNode(fullySelectedRootStyle->style(), document, true);
                }
            } else {
                // Since this node and all the other ancestors are not in the selection we want to set RangeFullySelectsNode to DoesNotFullySelectNode
                // so that styles that affect the exterior of the node are not included.
                accumulator.wrapWithNode(*ancestor, convertBlocksToInlines, StyledMarkupAccumulator::DoesNotFullySelectNode);
            }

            if (ancestor == specialCommonAncestor)
                break;
        }
    }

    // FIXME: The interchange newline should be placed in the block that it's in, not after all of the content, unconditionally.
    if (shouldAnnotate == AnnotateForInterchange && needInterchangeNewlineAfter(visibleEnd.previous()))
        accumulator.appendString(interchangeNewlineString);

    return accumulator.takeResults();
}

String createMarkup(const Range* range, EAnnotateForInterchange shouldAnnotate, bool convertBlocksToInlines, EAbsoluteURLs shouldResolveURLs, Node* constrainingAncestor)
{
    if (!range)
        return emptyString();

    Document& document = range->ownerDocument();
    const Range* updatedRange = range;

    return createMarkupInternal(document, range, updatedRange, shouldAnnotate, convertBlocksToInlines, shouldResolveURLs, constrainingAncestor);
}

PassRefPtrWillBeRawPtr<DocumentFragment> createFragmentFromMarkup(Document& document, const String& markup, const String& baseURL, ParserContentPolicy parserContentPolicy)
{
    // We use a fake body element here to trick the HTML parser to using the InBody insertion mode.
    RefPtrWillBeRawPtr<HTMLBodyElement> fakeBody = HTMLBodyElement::create(document);
    RefPtrWillBeRawPtr<DocumentFragment> fragment = DocumentFragment::create(document);

    fragment->parseHTML(markup, fakeBody.get(), parserContentPolicy);

    if (!baseURL.isEmpty() && baseURL != blankURL() && baseURL != document.baseURL())
        completeURLs(*fragment, baseURL);

    return fragment.release();
}

static const char fragmentMarkerTag[] = "webkit-fragment-marker";

static bool findNodesSurroundingContext(Document* document, RefPtrWillBeRawPtr<Comment>& nodeBeforeContext, RefPtrWillBeRawPtr<Comment>& nodeAfterContext)
{
    for (Node& node : NodeTraversal::startsAt(document->firstChild())) {
        if (node.nodeType() == Node::COMMENT_NODE && toComment(node).data() == fragmentMarkerTag) {
            if (!nodeBeforeContext)
                nodeBeforeContext = &toComment(node);
            else {
                nodeAfterContext = &toComment(node);
                return true;
            }
        }
    }
    return false;
}

static void trimFragment(DocumentFragment* fragment, Comment* nodeBeforeContext, Comment* nodeAfterContext)
{
    RefPtrWillBeRawPtr<Node> next = nullptr;
    for (RefPtrWillBeRawPtr<Node> node = fragment->firstChild(); node; node = next) {
        if (nodeBeforeContext->isDescendantOf(node.get())) {
            next = NodeTraversal::next(*node);
            continue;
        }
        next = NodeTraversal::nextSkippingChildren(*node);
        ASSERT(!node->contains(nodeAfterContext));
        node->parentNode()->removeChild(node.get(), ASSERT_NO_EXCEPTION);
        if (nodeBeforeContext == node)
            break;
    }

    ASSERT(nodeAfterContext->parentNode());
    for (RefPtrWillBeRawPtr<Node> node = nodeAfterContext; node; node = next) {
        next = NodeTraversal::nextSkippingChildren(*node);
        node->parentNode()->removeChild(node.get(), ASSERT_NO_EXCEPTION);
    }
}

PassRefPtrWillBeRawPtr<DocumentFragment> createFragmentFromMarkupWithContext(Document& document, const String& markup, unsigned fragmentStart, unsigned fragmentEnd,
    const String& baseURL, ParserContentPolicy parserContentPolicy)
{
    // FIXME: Need to handle the case where the markup already contains these markers.

    StringBuilder taggedMarkup;
    taggedMarkup.append(markup.left(fragmentStart));
    MarkupAccumulator::appendComment(taggedMarkup, fragmentMarkerTag);
    taggedMarkup.append(markup.substring(fragmentStart, fragmentEnd - fragmentStart));
    MarkupAccumulator::appendComment(taggedMarkup, fragmentMarkerTag);
    taggedMarkup.append(markup.substring(fragmentEnd));

    RefPtrWillBeRawPtr<DocumentFragment> taggedFragment = createFragmentFromMarkup(document, taggedMarkup.toString(), baseURL, parserContentPolicy);
    RefPtrWillBeRawPtr<Document> taggedDocument = Document::create();
    taggedDocument->setContextFeatures(document.contextFeatures());

    // FIXME: It's not clear what this code is trying to do. It puts nodes as direct children of a
    // Document that are not normally allowed by using the parser machinery.
    taggedDocument->parserTakeAllChildrenFrom(*taggedFragment);

    RefPtrWillBeRawPtr<Comment> nodeBeforeContext = nullptr;
    RefPtrWillBeRawPtr<Comment> nodeAfterContext = nullptr;
    if (!findNodesSurroundingContext(taggedDocument.get(), nodeBeforeContext, nodeAfterContext))
        return nullptr;

    RefPtrWillBeRawPtr<Range> range = Range::create(*taggedDocument.get(),
        positionAfterNode(nodeBeforeContext.get()).parentAnchoredEquivalent(),
        positionBeforeNode(nodeAfterContext.get()).parentAnchoredEquivalent());

    Node* commonAncestor = range->commonAncestorContainer();
    HTMLElement* specialCommonAncestor = ancestorToRetainStructureAndAppearanceWithNoRenderer(commonAncestor);

    // When there's a special common ancestor outside of the fragment, we must include it as well to
    // preserve the structure and appearance of the fragment. For example, if the fragment contains
    // TD, we need to include the enclosing TABLE tag as well.
    RefPtrWillBeRawPtr<DocumentFragment> fragment = DocumentFragment::create(document);
    if (specialCommonAncestor)
        fragment->appendChild(specialCommonAncestor);
    else
        fragment->parserTakeAllChildrenFrom(toContainerNode(*commonAncestor));

    trimFragment(fragment.get(), nodeBeforeContext.get(), nodeAfterContext.get());

    return fragment;
}

String createMarkup(const Node* node, EChildrenOnly childrenOnly, EAbsoluteURLs shouldResolveURLs)
{
    if (!node)
        return "";

    MarkupAccumulator accumulator(nullptr, shouldResolveURLs, Position(), Position());
    return accumulator.serializeNodes(const_cast<Node&>(*node), childrenOnly);
}

static void fillContainerFromString(ContainerNode* paragraph, const String& string)
{
    Document& document = paragraph->document();

    if (string.isEmpty()) {
        paragraph->appendChild(createBlockPlaceholderElement(document));
        return;
    }

    ASSERT(string.find('\n') == kNotFound);

    Vector<String> tabList;
    string.split('\t', true, tabList);
    StringBuilder tabText;
    bool first = true;
    size_t numEntries = tabList.size();
    for (size_t i = 0; i < numEntries; ++i) {
        const String& s = tabList[i];

        // append the non-tab textual part
        if (!s.isEmpty()) {
            if (!tabText.isEmpty()) {
                paragraph->appendChild(createTabSpanElement(document, tabText.toString()));
                tabText.clear();
            }
            RefPtrWillBeRawPtr<Text> textNode = document.createTextNode(stringWithRebalancedWhitespace(s, first, i + 1 == numEntries));
            paragraph->appendChild(textNode.release());
        }

        // there is a tab after every entry, except the last entry
        // (if the last character is a tab, the list gets an extra empty entry)
        if (i + 1 != numEntries)
            tabText.append('\t');
        else if (!tabText.isEmpty())
            paragraph->appendChild(createTabSpanElement(document, tabText.toString()));

        first = false;
    }
}

bool isPlainTextMarkup(Node* node)
{
    ASSERT(node);
    if (!isHTMLDivElement(*node))
        return false;

    HTMLDivElement& element = toHTMLDivElement(*node);
    if (!element.hasAttributes())
        return false;

    if (element.hasOneChild())
        return element.firstChild()->isTextNode() || element.firstChild()->hasChildren();

    return element.hasChildCount(2) && isTabHTMLSpanElementTextNode(element.firstChild()->firstChild()) && element.lastChild()->isTextNode();
}

static bool shouldPreserveNewline(const Range& range)
{
    if (Node* node = range.firstNode()) {
        if (LayoutObject* renderer = node->layoutObject())
            return renderer->style()->preserveNewline();
    }

    if (Node* node = range.startPosition().anchorNode()) {
        if (LayoutObject* renderer = node->layoutObject())
            return renderer->style()->preserveNewline();
    }

    return false;
}

PassRefPtrWillBeRawPtr<DocumentFragment> createFragmentFromText(Range* context, const String& text)
{
    if (!context)
        return nullptr;

    Document& document = context->ownerDocument();
    RefPtrWillBeRawPtr<DocumentFragment> fragment = document.createDocumentFragment();

    if (text.isEmpty())
        return fragment.release();

    String string = text;
    string.replace("\r\n", "\n");
    string.replace('\r', '\n');

    if (shouldPreserveNewline(*context)) {
        fragment->appendChild(document.createTextNode(string));
        if (string.endsWith('\n')) {
            RefPtrWillBeRawPtr<HTMLBRElement> element = createBreakElement(document);
            element->setAttribute(classAttr, AppleInterchangeNewline);
            fragment->appendChild(element.release());
        }
        return fragment.release();
    }

    // A string with no newlines gets added inline, rather than being put into a paragraph.
    if (string.find('\n') == kNotFound) {
        fillContainerFromString(fragment.get(), string);
        return fragment.release();
    }

    // Break string into paragraphs. Extra line breaks turn into empty paragraphs.
    Element* block = enclosingBlock(context->firstNode());
    bool useClonesOfEnclosingBlock = block
        && !isHTMLBodyElement(*block)
        && !isHTMLHtmlElement(*block)
        && block != editableRootForPosition(context->startPosition());
    bool useLineBreak = enclosingTextFormControl(context->startPosition());

    Vector<String> list;
    string.split('\n', true, list); // true gets us empty strings in the list
    size_t numLines = list.size();
    for (size_t i = 0; i < numLines; ++i) {
        const String& s = list[i];

        RefPtrWillBeRawPtr<Element> element = nullptr;
        if (s.isEmpty() && i + 1 == numLines) {
            // For last line, use the "magic BR" rather than a P.
            element = createBreakElement(document);
            element->setAttribute(classAttr, AppleInterchangeNewline);
        } else if (useLineBreak) {
            element = createBreakElement(document);
            fillContainerFromString(fragment.get(), s);
        } else {
            if (useClonesOfEnclosingBlock)
                element = block->cloneElementWithoutChildren();
            else
                element = createDefaultParagraphElement(document);
            fillContainerFromString(element.get(), s);
        }
        fragment->appendChild(element.release());
    }
    return fragment.release();
}

String urlToMarkup(const KURL& url, const String& title)
{
    StringBuilder markup;
    markup.appendLiteral("<a href=\"");
    markup.append(url.string());
    markup.appendLiteral("\">");
    MarkupAccumulator::appendCharactersReplacingEntities(markup, title, 0, title.length(), EntityMaskInPCDATA);
    markup.appendLiteral("</a>");
    return markup.toString();
}

PassRefPtrWillBeRawPtr<DocumentFragment> createFragmentForInnerOuterHTML(const String& markup, Element* contextElement, ParserContentPolicy parserContentPolicy, const char* method, ExceptionState& exceptionState)
{
    ASSERT(contextElement);
    Document& document = isHTMLTemplateElement(*contextElement) ? contextElement->document().ensureTemplateDocument() : contextElement->document();
    RefPtrWillBeRawPtr<DocumentFragment> fragment = DocumentFragment::create(document);

    if (document.isHTMLDocument()) {
        fragment->parseHTML(markup, contextElement, parserContentPolicy);
        return fragment;
    }

    bool wasValid = fragment->parseXML(markup, contextElement, parserContentPolicy);
    if (!wasValid) {
        exceptionState.throwDOMException(SyntaxError, "The provided markup is invalid XML, and therefore cannot be inserted into an XML document.");
        return nullptr;
    }
    return fragment.release();
}

PassRefPtrWillBeRawPtr<DocumentFragment> createFragmentForTransformToFragment(const String& sourceString, const String& sourceMIMEType, Document& outputDoc)
{
    RefPtrWillBeRawPtr<DocumentFragment> fragment = outputDoc.createDocumentFragment();

    if (sourceMIMEType == "text/html") {
        // As far as I can tell, there isn't a spec for how transformToFragment is supposed to work.
        // Based on the documentation I can find, it looks like we want to start parsing the fragment in the InBody insertion mode.
        // Unfortunately, that's an implementation detail of the parser.
        // We achieve that effect here by passing in a fake body element as context for the fragment.
        RefPtrWillBeRawPtr<HTMLBodyElement> fakeBody = HTMLBodyElement::create(outputDoc);
        fragment->parseHTML(sourceString, fakeBody.get());
    } else if (sourceMIMEType == "text/plain") {
        fragment->parserAppendChild(Text::create(outputDoc, sourceString));
    } else {
        bool successfulParse = fragment->parseXML(sourceString, 0);
        if (!successfulParse)
            return nullptr;
    }

    // FIXME: Do we need to mess with URLs here?

    return fragment.release();
}

static inline void removeElementPreservingChildren(PassRefPtrWillBeRawPtr<DocumentFragment> fragment, HTMLElement* element)
{
    RefPtrWillBeRawPtr<Node> nextChild = nullptr;
    for (RefPtrWillBeRawPtr<Node> child = element->firstChild(); child; child = nextChild) {
        nextChild = child->nextSibling();
        element->removeChild(child.get());
        fragment->insertBefore(child, element);
    }
    fragment->removeChild(element);
}

static inline bool isSupportedContainer(Element* element)
{
    ASSERT(element);
    if (!element->isHTMLElement())
        return true;

    HTMLElement& htmlElement = toHTMLElement(*element);
    if (htmlElement.hasTagName(colTag) || htmlElement.hasTagName(colgroupTag) || htmlElement.hasTagName(framesetTag)
        || htmlElement.hasTagName(headTag) || htmlElement.hasTagName(styleTag) || htmlElement.hasTagName(titleTag)) {
        return false;
    }
    return !htmlElement.ieForbidsInsertHTML();
}

PassRefPtrWillBeRawPtr<DocumentFragment> createContextualFragment(const String& markup, Element* element, ParserContentPolicy parserContentPolicy, ExceptionState& exceptionState)
{
    ASSERT(element);
    if (!isSupportedContainer(element)) {
        exceptionState.throwDOMException(NotSupportedError, "The range's container is '" + element->localName() + "', which is not supported.");
        return nullptr;
    }

    RefPtrWillBeRawPtr<DocumentFragment> fragment = createFragmentForInnerOuterHTML(markup, element, parserContentPolicy, "createContextualFragment", exceptionState);
    if (!fragment)
        return nullptr;

    // We need to pop <html> and <body> elements and remove <head> to
    // accommodate folks passing complete HTML documents to make the
    // child of an element.

    RefPtrWillBeRawPtr<Node> nextNode = nullptr;
    for (RefPtrWillBeRawPtr<Node> node = fragment->firstChild(); node; node = nextNode) {
        nextNode = node->nextSibling();
        if (isHTMLHtmlElement(*node) || isHTMLHeadElement(*node) || isHTMLBodyElement(*node)) {
            HTMLElement* element = toHTMLElement(node);
            if (Node* firstChild = element->firstChild())
                nextNode = firstChild;
            removeElementPreservingChildren(fragment, element);
        }
    }
    return fragment.release();
}

void replaceChildrenWithFragment(ContainerNode* container, PassRefPtrWillBeRawPtr<DocumentFragment> fragment, ExceptionState& exceptionState)
{
    ASSERT(container);
    RefPtrWillBeRawPtr<ContainerNode> containerNode(container);

    ChildListMutationScope mutation(*containerNode);

    if (!fragment->firstChild()) {
        containerNode->removeChildren();
        return;
    }

    // FIXME: This is wrong if containerNode->firstChild() has more than one ref!
    if (containerNode->hasOneTextChild() && fragment->hasOneTextChild()) {
        toText(containerNode->firstChild())->setData(toText(fragment->firstChild())->data());
        return;
    }

    // FIXME: No need to replace the child it is a text node and its contents are already == text.
    if (containerNode->hasOneChild()) {
        containerNode->replaceChild(fragment, containerNode->firstChild(), exceptionState);
        return;
    }

    containerNode->removeChildren();
    containerNode->appendChild(fragment, exceptionState);
}

void replaceChildrenWithText(ContainerNode* container, const String& text, ExceptionState& exceptionState)
{
    ASSERT(container);
    RefPtrWillBeRawPtr<ContainerNode> containerNode(container);

    ChildListMutationScope mutation(*containerNode);

    // FIXME: This is wrong if containerNode->firstChild() has more than one ref! Example:
    // <div>foo</div>
    // <script>
    // var oldText = div.firstChild;
    // console.log(oldText.data); // foo
    // div.innerText = "bar";
    // console.log(oldText.data); // bar!?!
    // </script>
    // I believe this is an intentional benchmark cheat from years ago.
    // We should re-visit if we actually want this still.
    if (containerNode->hasOneTextChild()) {
        toText(containerNode->firstChild())->setData(text);
        return;
    }

    // NOTE: This method currently always creates a text node, even if that text node will be empty.
    RefPtrWillBeRawPtr<Text> textNode = Text::create(containerNode->document(), text);

    // FIXME: No need to replace the child it is a text node and its contents are already == text.
    if (containerNode->hasOneChild()) {
        containerNode->replaceChild(textNode.release(), containerNode->firstChild(), exceptionState);
        return;
    }

    containerNode->removeChildren();
    containerNode->appendChild(textNode.release(), exceptionState);
}

void mergeWithNextTextNode(Text* textNode, ExceptionState& exceptionState)
{
    ASSERT(textNode);
    Node* next = textNode->nextSibling();
    if (!next || !next->isTextNode())
        return;

    RefPtrWillBeRawPtr<Text> textNext = toText(next);
    textNode->appendData(textNext->data());
    if (textNext->parentNode()) // Might have been removed by mutation event.
        textNext->remove(exceptionState);
}

String createStyledMarkupForNavigationTransition(Node* node)
{
    node->document().updateLayoutIgnorePendingStylesheets();

    StyledMarkupAccumulator accumulator(ResolveAllURLs, AnnotateForNavigationTransition, Position(), Position(), 0);
    accumulator.serializeNodes(node, NodeTraversal::nextSkippingChildren(*node));

    static const char* documentMarkup = "<!DOCTYPE html><meta name=\"viewport\" content=\"width=device-width, user-scalable=0\">";
    return documentMarkup + accumulator.takeResults();
}

}
