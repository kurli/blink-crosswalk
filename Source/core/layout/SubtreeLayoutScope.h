/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef SubtreeLayoutScope_h
#define SubtreeLayoutScope_h

#include "core/inspector/InspectorTraceEvents.h"
#include "wtf/HashSet.h"

// This is the way to mark a subtree as needing layout during layout,
// e.g. for the purposes of doing a multipass layout.
//
// It should only be used during layout. Outside of layout, you should
// just call renderer->setNeedsLayout() directly.
//
// It ensures that you don't accidentally mark part of the tree as
// needing layout and not actually lay it out.

namespace blink {

class LayoutObject;

class SubtreeLayoutScope {
public:
    SubtreeLayoutScope(LayoutObject& root);
    ~SubtreeLayoutScope();

    void setNeedsLayout(LayoutObject* descendant, LayoutInvalidationReasonForTracing);
    void setChildNeedsLayout(LayoutObject* descendant);

    LayoutObject& root() { return m_root; }
    void addRendererToLayout(LayoutObject* renderer);

private:
    LayoutObject& m_root;

#if ENABLE(ASSERT)
    HashSet<LayoutObject*> m_renderersToLayout;
#endif
};

}

#endif
