/*
 * Copyright (C) 2007, 2008 Apple Inc. All rights reserved.
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

#ifndef PositionIterator_h
#define PositionIterator_h

#include "core/dom/Node.h"
#include "core/editing/EditingStrategy.h"
#include "core/editing/htmlediting.h"
#include "core/html/HTMLHtmlElement.h"
#include "core/layout/LayoutBlock.h"

namespace blink {

class Position;

// A Position iterator with constant-time
// increment, decrement, and several predicates on the Position it is at.
// Conversion to/from Position is O(n) in the offset.
template <typename Strategy>
class PositionIteratorAlgorithm {
    STACK_ALLOCATED();
public:
    using PositionType = typename Strategy::PositionType;

    PositionIteratorAlgorithm(const PositionType&);
    PositionIteratorAlgorithm();

    operator PositionType() const;

    void increment();
    void decrement();

    Node* node() const { return m_anchorNode; }
    int offsetInLeafNode() const { return m_offsetInAnchor; }

    bool atStart() const;
    bool atEnd() const;
    bool atStartOfNode() const;
    bool atEndOfNode() const;
    bool isCandidate() const;

private:
    PositionIteratorAlgorithm(Node*, int);

    RawPtrWillBeMember<Node> m_anchorNode;
    RawPtrWillBeMember<Node> m_nodeAfterPositionInAnchor; // If this is non-null, Strategy::parent(*m_nodeAfterPositionInAnchor) == m_anchorNode;
    int m_offsetInAnchor;
};

// Node traversal for |PositionIteratorAlgorithm|
class PositionIteratorStrategy : public EditingStrategy {
public:
    static bool atEditingBoundary(const PositionType&);
    static PositionType createLegacyEditingPosition(Node*, int);
    static int editingOffset(const PositionType&);
    static bool editingIgnoresContent(Node*);
    static bool inRenderedText(const PositionType&);
    static int lastOffsetForEditing(Node*);
    static Position lastPositionInOrAfterNode(Node*);
    static bool nodeIsUserSelectNone(Node*);
    static PositionType positionBeforeNode(Node*);
    static PositionType positionInParentBeforeNode(Node*);
    static int uncheckedNextOffset(const Node*, int);
    static int uncheckedPreviousOffset(const Node*, int);
};

extern template class PositionIteratorAlgorithm<PositionIteratorStrategy>;

using PositionIterator = PositionIteratorAlgorithm<PositionIteratorStrategy>;

} // namespace blink

#endif // PositionIterator_h
