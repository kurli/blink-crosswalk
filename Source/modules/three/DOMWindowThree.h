// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMWindowThree_h
#define DOMWindowThree_h

#include "core/frame/DOMWindowProperty.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class Three;
class DOMWindow;

class DOMWindowThree : public NoBaseWillBeGarbageCollectedFinalized<DOMWindowThree>, public WillBeHeapSupplement<LocalDOMWindow>, public DOMWindowProperty {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DOMWindowThree);
public:
    virtual ~DOMWindowThree();
    static DOMWindowThree& from(LocalDOMWindow&);
    static Three* cthree(DOMWindow&);
    Three* cthree() const;

private:
    explicit DOMWindowThree(LocalDOMWindow&);
    static const char* supplementName();

    mutable PersistentWillBeMember<Three> m_three;
};

} // namespace blink

#endif // DOMWindowThree_h
