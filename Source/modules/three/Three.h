// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright (c) 2015 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Three_h
#define Three_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"

namespace blink {
class WebGLRenderingContext;

class Three : public GarbageCollectedFinalized<Three>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static Three* create()
    {
        return new Three();
    }

    virtual ~Three();
    DECLARE_TRACE();
    void forceGC();
    void init(WebGLRenderingContext* context, int width, int height);
    void run();
    void step(double dt);

private:
    Three();
};

} // namespace blink

#endif // Three_h
