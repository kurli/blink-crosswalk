// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file has been auto-generated by code_generator_v8.py. DO NOT MODIFY!

#include "config.h"
#include "V8TestSpecialOperationsNotEnumerable.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/V8DOMConfiguration.h"
#include "bindings/core/v8/V8ObjectConstructor.h"
#include "core/dom/ContextFeatures.h"
#include "core/dom/Document.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "wtf/GetPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

const WrapperTypeInfo V8TestSpecialOperationsNotEnumerable::wrapperTypeInfo = { gin::kEmbedderBlink, V8TestSpecialOperationsNotEnumerable::domTemplate, V8TestSpecialOperationsNotEnumerable::refObject, V8TestSpecialOperationsNotEnumerable::derefObject, V8TestSpecialOperationsNotEnumerable::trace, 0, 0, V8TestSpecialOperationsNotEnumerable::installConditionallyEnabledMethods, V8TestSpecialOperationsNotEnumerable::installConditionallyEnabledProperties, "TestSpecialOperationsNotEnumerable", 0, WrapperTypeInfo::WrapperTypeObjectPrototype, WrapperTypeInfo::ObjectClassId, WrapperTypeInfo::NotInheritFromEventTarget, WrapperTypeInfo::Independent, WrapperTypeInfo::RefCountedObject };

// This static member must be declared by DEFINE_WRAPPERTYPEINFO in TestSpecialOperationsNotEnumerable.h.
// For details, see the comment of DEFINE_WRAPPERTYPEINFO in
// bindings/core/v8/ScriptWrappable.h.
const WrapperTypeInfo& TestSpecialOperationsNotEnumerable::s_wrapperTypeInfo = V8TestSpecialOperationsNotEnumerable::wrapperTypeInfo;

namespace TestSpecialOperationsNotEnumerableV8Internal {

static void indexedPropertyGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TestSpecialOperationsNotEnumerable* impl = V8TestSpecialOperationsNotEnumerable::toImpl(info.Holder());
    String result = impl->anonymousIndexedGetter(index);
    if (result.isNull())
        return;
    v8SetReturnValueString(info, result, info.GetIsolate());
}

static void indexedPropertyGetterCallback(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("blink", "DOMIndexedProperty");
    TestSpecialOperationsNotEnumerableV8Internal::indexedPropertyGetter(index, info);
    TRACE_EVENT_SET_SAMPLING_STATE("v8", "V8Execution");
}

static void namedPropertyGetter(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    auto nameString = name.As<v8::String>();
    TestSpecialOperationsNotEnumerable* impl = V8TestSpecialOperationsNotEnumerable::toImpl(info.Holder());
    AtomicString propertyName = toCoreAtomicString(nameString);
    String result = impl->anonymousNamedGetter(propertyName);
    if (result.isNull())
        return;
    v8SetReturnValueString(info, result, info.GetIsolate());
}

static void namedPropertyGetterCallback(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("blink", "DOMNamedProperty");
    TestSpecialOperationsNotEnumerableV8Internal::namedPropertyGetter(name, info);
    TRACE_EVENT_SET_SAMPLING_STATE("v8", "V8Execution");
}

} // namespace TestSpecialOperationsNotEnumerableV8Internal

static void installV8TestSpecialOperationsNotEnumerableTemplate(v8::Local<v8::FunctionTemplate> functionTemplate, v8::Isolate* isolate)
{
    functionTemplate->ReadOnlyPrototype();

    v8::Local<v8::Signature> defaultSignature;
    defaultSignature = V8DOMConfiguration::installDOMClassTemplate(isolate, functionTemplate, "TestSpecialOperationsNotEnumerable", v8::Local<v8::FunctionTemplate>(), V8TestSpecialOperationsNotEnumerable::internalFieldCount,
        0, 0,
        0, 0,
        0, 0);
    v8::Local<v8::ObjectTemplate> instanceTemplate = functionTemplate->InstanceTemplate();
    ALLOW_UNUSED_LOCAL(instanceTemplate);
    v8::Local<v8::ObjectTemplate> prototypeTemplate = functionTemplate->PrototypeTemplate();
    ALLOW_UNUSED_LOCAL(prototypeTemplate);
    {
        v8::IndexedPropertyHandlerConfiguration config(TestSpecialOperationsNotEnumerableV8Internal::indexedPropertyGetterCallback, 0, 0, 0, 0);
        functionTemplate->InstanceTemplate()->SetHandler(config);
    }
    {
        v8::NamedPropertyHandlerConfiguration config(TestSpecialOperationsNotEnumerableV8Internal::namedPropertyGetterCallback, 0, 0, 0, 0);
        config.flags = static_cast<v8::PropertyHandlerFlags>(static_cast<int>(config.flags) | static_cast<int>(v8::PropertyHandlerFlags::kOnlyInterceptStrings));
        config.flags = static_cast<v8::PropertyHandlerFlags>(static_cast<int>(config.flags) | static_cast<int>(v8::PropertyHandlerFlags::kNonMasking));
        functionTemplate->InstanceTemplate()->SetHandler(config);
    }

    // Custom toString template
    functionTemplate->Set(v8AtomicString(isolate, "toString"), V8PerIsolateData::from(isolate)->toStringTemplate());
}

v8::Local<v8::FunctionTemplate> V8TestSpecialOperationsNotEnumerable::domTemplate(v8::Isolate* isolate)
{
    return V8DOMConfiguration::domClassTemplate(isolate, const_cast<WrapperTypeInfo*>(&wrapperTypeInfo), installV8TestSpecialOperationsNotEnumerableTemplate);
}

bool V8TestSpecialOperationsNotEnumerable::hasInstance(v8::Local<v8::Value> v8Value, v8::Isolate* isolate)
{
    return V8PerIsolateData::from(isolate)->hasInstance(&wrapperTypeInfo, v8Value);
}

v8::Local<v8::Object> V8TestSpecialOperationsNotEnumerable::findInstanceInPrototypeChain(v8::Local<v8::Value> v8Value, v8::Isolate* isolate)
{
    return V8PerIsolateData::from(isolate)->findInstanceInPrototypeChain(&wrapperTypeInfo, v8Value);
}

TestSpecialOperationsNotEnumerable* V8TestSpecialOperationsNotEnumerable::toImplWithTypeCheck(v8::Isolate* isolate, v8::Local<v8::Value> value)
{
    return hasInstance(value, isolate) ? toImpl(v8::Local<v8::Object>::Cast(value)) : 0;
}

void V8TestSpecialOperationsNotEnumerable::refObject(ScriptWrappable* scriptWrappable)
{
    scriptWrappable->toImpl<TestSpecialOperationsNotEnumerable>()->ref();
}

void V8TestSpecialOperationsNotEnumerable::derefObject(ScriptWrappable* scriptWrappable)
{
    scriptWrappable->toImpl<TestSpecialOperationsNotEnumerable>()->deref();
}

} // namespace blink
