// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/three/DOMWindowThree.h"

#include "core/frame/LocalDOMWindow.h"
#include "modules/three/Three.h"

namespace blink {

DOMWindowThree::DOMWindowThree(LocalDOMWindow& window)
    : DOMWindowProperty(window.frame())
{
}

DOMWindowThree::~DOMWindowThree()
{
}

const char* DOMWindowThree::supplementName()
{
    return "DOMWindowThree";
}

DOMWindowThree& DOMWindowThree::from(LocalDOMWindow& window)
{
    DOMWindowThree* three = static_cast<DOMWindowThree*>(WillBeHeapSupplement<LocalDOMWindow>::from(window, supplementName()));
    if (!three) {
        three = new DOMWindowThree(window);
        provideTo(window, supplementName(), adoptPtrWillBeNoop(three));
    }
    return *three;
}

Three* DOMWindowThree::cthree(DOMWindow& window)
{
    return DOMWindowThree::from(toLocalDOMWindow(window)).cthree();
}

Three* DOMWindowThree::cthree() const
{
    if (!m_three && frame())
        m_three = Three::create();
    return m_three.get();
}

} // namespace blink