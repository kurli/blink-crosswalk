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
#include "bindings/core/v8/V8MessageEvent.h"

#include "bindings/core/v8/SerializedScriptValue.h"
#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "bindings/core/v8/V8ArrayBuffer.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8Blob.h"
#include "bindings/core/v8/V8HiddenValue.h"
#include "bindings/core/v8/V8MessagePort.h"
#include "bindings/core/v8/V8Window.h"
#include "core/events/MessageEvent.h"

namespace blink {

void V8MessageEvent::dataAttributeGetterCustom(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    MessageEvent* event = V8MessageEvent::toImpl(info.Holder());

    v8::Handle<v8::Value> result;
    switch (event->dataType()) {
    case MessageEvent::DataTypeScriptValue: {
        result = V8HiddenValue::getHiddenValue(info.GetIsolate(), info.Holder(), V8HiddenValue::data(info.GetIsolate()));
        if (result.IsEmpty()) {
            if (!event->dataAsSerializedScriptValue()) {
                // If we're in an isolated world and the event was created in the main world,
                // we need to find the 'data' property on the main world wrapper and clone it.
                v8::Local<v8::Value> mainWorldData = V8HiddenValue::getHiddenValueFromMainWorldWrapper(info.GetIsolate(), event, V8HiddenValue::data(info.GetIsolate()));
                if (!mainWorldData.IsEmpty())
                    event->setSerializedData(SerializedScriptValueFactory::instance().createAndSwallowExceptions(info.GetIsolate(), mainWorldData));
            }
            if (event->dataAsSerializedScriptValue())
                result = event->dataAsSerializedScriptValue()->deserialize(info.GetIsolate());
            else
                result = v8::Null(info.GetIsolate());
        }
        break;
    }

    case MessageEvent::DataTypeSerializedScriptValue:
        if (SerializedScriptValue* serializedValue = event->dataAsSerializedScriptValue()) {
            MessagePortArray ports = event->ports();
            result = serializedValue->deserialize(info.GetIsolate(), &ports);
        } else {
            result = v8::Null(info.GetIsolate());
        }
        break;

    case MessageEvent::DataTypeString: {
        result = V8HiddenValue::getHiddenValue(info.GetIsolate(), info.Holder(), V8HiddenValue::stringData(info.GetIsolate()));
        if (result.IsEmpty()) {
            String stringValue = event->dataAsString();
            result = v8String(info.GetIsolate(), stringValue);
        }
        break;
    }

    case MessageEvent::DataTypeBlob:
        result = toV8(event->dataAsBlob(), info.Holder(), info.GetIsolate());
        break;

    case MessageEvent::DataTypeArrayBuffer:
        result = V8HiddenValue::getHiddenValue(info.GetIsolate(), info.Holder(), V8HiddenValue::arrayBufferData(info.GetIsolate()));
        if (result.IsEmpty())
            result = toV8(event->dataAsArrayBuffer(), info.Holder(), info.GetIsolate());
        break;
    }

    // Overwrite the data attribute so it returns the cached result in future invocations.
    // This custom getter handler will not be called again.
    v8::PropertyAttribute dataAttr = static_cast<v8::PropertyAttribute>(v8::DontDelete | v8::ReadOnly);
    if (!v8CallBoolean(info.Holder()->ForceSet(info.GetIsolate()->GetCurrentContext(), v8AtomicString(info.GetIsolate(), "data"), result, dataAttr))) {
        v8SetReturnValue(info, v8::Null(info.GetIsolate()));
        return;
    }
    v8SetReturnValue(info, result);
}

void V8MessageEvent::initMessageEventMethodCustom(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    ExceptionState exceptionState(ExceptionState::ExecutionContext, "initMessageEvent", "MessageEvent", info.Holder(), info.GetIsolate());
    MessageEvent* event = V8MessageEvent::toImpl(info.Holder());
    TOSTRING_VOID(V8StringResource<>, typeArg, info[0]);
    TONATIVE_VOID(bool, canBubbleArg, info[1]->BooleanValue());
    TONATIVE_VOID(bool, cancelableArg, info[2]->BooleanValue());
    v8::Handle<v8::Value> dataArg = info[3];
    TOSTRING_VOID(V8StringResource<>, originArg, info[4]);
    TOSTRING_VOID(V8StringResource<>, lastEventIdArg, info[5]);
    DOMWindow* sourceArg = toDOMWindow(info.GetIsolate(), info[6]);
    OwnPtrWillBeRawPtr<MessagePortArray> portArray = nullptr;
    const int portArrayIndex = 7;
    if (!isUndefinedOrNull(info[portArrayIndex])) {
        portArray = adoptPtrWillBeNoop(new MessagePortArray);
        *portArray = toRefPtrWillBeMemberNativeArray<MessagePort, V8MessagePort>(info[portArrayIndex], portArrayIndex + 1, info.GetIsolate(), exceptionState);
        if (exceptionState.throwIfNeeded())
            return;
    }
    event->initMessageEvent(typeArg, canBubbleArg, cancelableArg, originArg, lastEventIdArg, sourceArg, portArray.release());

    if (!dataArg.IsEmpty()) {
        V8HiddenValue::setHiddenValue(info.GetIsolate(), info.Holder(), V8HiddenValue::data(info.GetIsolate()), dataArg);
        if (DOMWrapperWorld::current(info.GetIsolate()).isIsolatedWorld())
            event->setSerializedData(SerializedScriptValueFactory::instance().createAndSwallowExceptions(info.GetIsolate(), dataArg));
    }
}

} // namespace blink
