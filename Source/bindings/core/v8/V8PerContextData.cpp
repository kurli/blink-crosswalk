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
#include "bindings/core/v8/V8PerContextData.h"

#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8ObjectConstructor.h"
#include "wtf/StringExtras.h"

#include <stdlib.h>

namespace blink {

V8PerContextData::V8PerContextData(v8::Handle<v8::Context> context)
    : m_isolate(context->GetIsolate())
    , m_wrapperBoilerplates(m_isolate)
    , m_constructorMap(m_isolate)
    , m_contextHolder(adoptPtr(new gin::ContextHolder(m_isolate)))
    , m_context(m_isolate, context)
    , m_activityLogger(0)
    , m_compiledPrivateScript(m_isolate)
{
    m_contextHolder->SetContext(context);

    v8::Context::Scope contextScope(context);
    ASSERT(m_errorPrototype.isEmpty());
    v8::Handle<v8::Object> object = v8::Handle<v8::Object>::Cast(context->Global()->Get(v8AtomicString(m_isolate, "Error")));
    ASSERT(!object.IsEmpty());
    v8::Handle<v8::Value> prototypeValue = object->Get(v8AtomicString(m_isolate, "prototype"));
    ASSERT(!prototypeValue.IsEmpty());
    m_errorPrototype.set(m_isolate, prototypeValue);
}

V8PerContextData::~V8PerContextData()
{
}

PassOwnPtr<V8PerContextData> V8PerContextData::create(v8::Handle<v8::Context> context)
{
    return adoptPtr(new V8PerContextData(context));
}

V8PerContextData* V8PerContextData::from(v8::Handle<v8::Context> context)
{
    return ScriptState::from(context)->perContextData();
}

v8::Local<v8::Object> V8PerContextData::createWrapperFromCacheSlowCase(const WrapperTypeInfo* type)
{
    ASSERT(!m_errorPrototype.isEmpty());

    v8::Context::Scope scope(context());
    v8::Local<v8::Function> function = constructorForType(type);
    v8::Local<v8::Object> instanceTemplate = V8ObjectConstructor::newInstance(m_isolate, function);
    if (!instanceTemplate.IsEmpty()) {
        m_wrapperBoilerplates.Set(type, instanceTemplate);
        return instanceTemplate->Clone();
    }
    return v8::Local<v8::Object>();
}

v8::Local<v8::Function> V8PerContextData::constructorForTypeSlowCase(const WrapperTypeInfo* type)
{
    ASSERT(!m_errorPrototype.isEmpty());

    v8::Local<v8::Context> currentContext = context();
    v8::Context::Scope scope(currentContext);
    // We shouldn't reach this point for the types that are implemented in v8 suche as typed arrays and
    // hence don't have domTemplateFunction.
    ASSERT(type->domTemplateFunction);
    v8::Handle<v8::FunctionTemplate> functionTemplate = type->domTemplate(m_isolate);
    // Getting the function might fail if we're running out of stack or memory.
    v8::Local<v8::Function> function = functionTemplate->GetFunction();
    if (function.IsEmpty())
        return v8::Local<v8::Function>();

    if (type->parentClass) {
        v8::Local<v8::Object> prototypeTemplate = constructorForType(type->parentClass);
        if (prototypeTemplate.IsEmpty())
            return v8::Local<v8::Function>();
        if (!v8CallBoolean(function->SetPrototype(currentContext, prototypeTemplate)))
            return v8::Local<v8::Function>();
    }

    v8::Local<v8::Value> prototypeValue = function->Get(v8AtomicString(m_isolate, "prototype"));
    if (!prototypeValue.IsEmpty() && prototypeValue->IsObject()) {
        v8::Local<v8::Object> prototypeObject = v8::Local<v8::Object>::Cast(prototypeValue);
        if (prototypeObject->InternalFieldCount() == v8PrototypeInternalFieldcount
            && type->wrapperTypePrototype == WrapperTypeInfo::WrapperTypeObjectPrototype)
            prototypeObject->SetAlignedPointerInInternalField(v8PrototypeTypeIndex, const_cast<WrapperTypeInfo*>(type));
        type->installConditionallyEnabledMethods(prototypeObject, m_isolate);
        if (type->wrapperTypePrototype == WrapperTypeInfo::WrapperTypeExceptionPrototype) {
            if (!v8CallBoolean(prototypeObject->SetPrototype(currentContext, m_errorPrototype.newLocal(m_isolate))))
                return v8::Local<v8::Function>();
        }
    }

    m_constructorMap.Set(type, function);

    return function;
}

v8::Local<v8::Object> V8PerContextData::prototypeForType(const WrapperTypeInfo* type)
{
    v8::Local<v8::Object> constructor = constructorForType(type);
    if (constructor.IsEmpty())
        return v8::Local<v8::Object>();
    return constructor->Get(v8String(m_isolate, "prototype")).As<v8::Object>();
}

void V8PerContextData::addCustomElementBinding(CustomElementDefinition* definition, PassOwnPtr<CustomElementBinding> binding)
{
    m_customElementBindings.append(binding);
}

v8::Handle<v8::Value> V8PerContextData::compiledPrivateScript(String className)
{
    return m_compiledPrivateScript.Get(className);
}

void V8PerContextData::setCompiledPrivateScript(String className, v8::Handle<v8::Value> compiledObject)
{
    m_compiledPrivateScript.Set(className, compiledObject);
}

} // namespace blink
