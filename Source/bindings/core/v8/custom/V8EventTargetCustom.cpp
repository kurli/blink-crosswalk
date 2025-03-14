/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
#include "bindings/core/v8/V8EventTarget.h"

#include "bindings/core/v8/V8Window.h"
#include "core/EventTargetNames.h"

namespace blink {

v8::Handle<v8::Value> toV8(EventTarget* impl, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    if (UNLIKELY(!impl))
        return v8::Null(isolate);

    // TODO(yukishiino): Rename EventTargetNames::LocalDOMWindow to DOMWindow.
    if (impl->interfaceName() == EventTargetNames::LocalDOMWindow)
        return toV8(static_cast<DOMWindow*>(impl), creationContext, isolate);

    v8::Handle<v8::Value> wrapper = DOMDataStore::getWrapper(impl, isolate);
    if (!wrapper.IsEmpty())
        return wrapper;
    return impl->wrap(creationContext, isolate);
}

void V8EventTarget::addEventListenerMethodEpilogueCustom(const v8::FunctionCallbackInfo<v8::Value>& info, EventTarget* impl)
{
    if (info.Length() >= 2 && info[1]->IsObject() && !impl->toNode())
        addHiddenValueToArray(info.GetIsolate(), info.Holder(), info[1], V8EventTarget::eventListenerCacheIndex);
}

void V8EventTarget::removeEventListenerMethodEpilogueCustom(const v8::FunctionCallbackInfo<v8::Value>& info, EventTarget* impl)
{
    if (info.Length() >= 2 && info[1]->IsObject() && !impl->toNode())
        removeHiddenValueFromArray(info.GetIsolate(), info.Holder(), info[1], V8EventTarget::eventListenerCacheIndex);
}

} // namespace blink
