/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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

#ifndef MarkupAccumulator_h
#define MarkupAccumulator_h

#include "core/dom/Position.h"
#include "core/editing/markup.h"
#include "wtf/HashMap.h"
#include "wtf/Vector.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

class Attribute;
class DocumentType;
class Element;
class Node;

typedef HashMap<AtomicString, AtomicString> Namespaces;

enum EntityMask {
    EntityAmp = 0x0001,
    EntityLt = 0x0002,
    EntityGt = 0x0004,
    EntityQuot = 0x0008,
    EntityNbsp = 0x0010,

    // Non-breaking space needs to be escaped in innerHTML for compatibility reason. See http://trac.webkit.org/changeset/32879
    // However, we cannot do this in a XML document because it does not have the entity reference defined (See the bug 19215).
    EntityMaskInCDATA = 0,
    EntityMaskInPCDATA = EntityAmp | EntityLt | EntityGt,
    EntityMaskInHTMLPCDATA = EntityMaskInPCDATA | EntityNbsp,
    EntityMaskInAttributeValue = EntityAmp | EntityQuot | EntityLt | EntityGt,
    EntityMaskInHTMLAttributeValue = EntityAmp | EntityQuot | EntityNbsp,
};

enum SerializationType {
    AsOwnerDocument,
    ForcedXML
};

class MarkupAccumulator {
    WTF_MAKE_NONCOPYABLE(MarkupAccumulator);
    STACK_ALLOCATED();
public:
    MarkupAccumulator(WillBeHeapVector<RawPtrWillBeMember<Node>>*, EAbsoluteURLs, const Position& start, const Position& end, SerializationType = AsOwnerDocument);
    virtual ~MarkupAccumulator();

    String serializeNodes(Node& targetNode, EChildrenOnly);

    static void appendComment(StringBuilder&, const String&);

    static void appendCharactersReplacingEntities(StringBuilder&, const String&, unsigned, unsigned, EntityMask);

protected:
    void appendString(const String&);
    void appendStartTag(Node&, Namespaces* = nullptr);
    virtual void appendEndTag(const Element&);
    static size_t totalLength(const Vector<String>&);
    size_t length() const { return m_markup.length(); }
    const Position& startPosition() const { return m_start; }
    const Position& endPosition() const { return m_end; }
    void concatenateMarkup(StringBuilder&);
    void appendAttributeValue(StringBuilder&, const String&, bool);
    virtual void appendCustomAttributes(StringBuilder&, const Element&, Namespaces*);
    bool shouldAddNamespaceElement(const Element&, Namespaces&);
    bool shouldAddNamespaceAttribute(const Attribute&, const Element&);

    void appendNamespace(StringBuilder&, const AtomicString& prefix, const AtomicString& namespaceURI, Namespaces&);
    EntityMask entityMaskForText(const Text&) const;
    virtual void appendText(StringBuilder&, Text&);
    void appendXMLDeclaration(StringBuilder&, const Document&);
    void appendDocumentType(StringBuilder&, const DocumentType&);
    void appendProcessingInstruction(StringBuilder&, const String& target, const String& data);
    virtual void appendElement(StringBuilder&, Element&, Namespaces*);
    void appendOpenTag(StringBuilder&, const Element&, Namespaces*);
    void appendCloseTag(StringBuilder&, const Element&);
    void appendAttribute(StringBuilder&, const Element&, const Attribute&, Namespaces*);
    void appendCDATASection(StringBuilder&, const String&);
    void appendStartMarkup(StringBuilder&, Node&, Namespaces*);
    bool shouldSelfClose(const Element&);
    bool elementCannotHaveEndTag(const Node&);
    void appendEndMarkup(StringBuilder&, const Element&);

    // FIXME: |PageSerializer| uses |m_nodes| for collecting nodes in document
    // included into serialized text then extracts image, object, etc. The size
    // of this vector isn't small for large document. It is better to use
    // callback like functionality.
    RawPtrWillBeMember<WillBeHeapVector<RawPtrWillBeMember<Node>>> const m_nodes;

    String renderedText(Text&);
    String stringValueForRange(const Node&);

private:
    String resolveURLIfNeeded(const Element&, const String&) const;
    void appendQuotedURLAttributeValue(StringBuilder&, const Element&, const Attribute&);
    void serializeNodesWithNamespaces(Node& targetNode, EChildrenOnly, const Namespaces*);
    bool serializeAsHTMLDocument(const Node&) const;

    StringBuilder m_markup;
    const EAbsoluteURLs m_resolveURLsMethod;
    SerializationType m_serializationType;
    const Position m_start;
    const Position m_end;
};

}

#endif
