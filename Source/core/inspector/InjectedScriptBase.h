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

#ifndef InjectedScriptBase_h
#define InjectedScriptBase_h

#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ScriptValue.h"
#include "core/InspectorTypeBuilder.h"
#include "wtf/Forward.h"

namespace blink {

class JSONValue;
class ScriptFunctionCall;

typedef String ErrorString;
PassRefPtr<JSONValue> toJSONValue(const ScriptValue&);

class InjectedScriptBase {
public:
    virtual ~InjectedScriptBase() { }

    const String& name() const { return m_name; }
    bool isEmpty() const { return m_injectedScriptObject.isEmpty(); }
    ScriptState* scriptState() const
    {
        ASSERT(!isEmpty());
        return m_injectedScriptObject.scriptState();
    }

protected:
    typedef bool (*InspectedStateAccessCheck)(ScriptState*);
    InjectedScriptBase(const String& name);
    InjectedScriptBase(const String& name, ScriptValue, InspectedStateAccessCheck);

    void initialize(ScriptValue, InspectedStateAccessCheck);
    bool canAccessInspectedWindow() const;
    const ScriptValue& injectedScriptObject() const;
    ScriptValue callFunctionWithEvalEnabled(ScriptFunctionCall&, bool& hadException) const;
    void makeCall(ScriptFunctionCall&, RefPtr<JSONValue>* result);
    void makeEvalCall(ErrorString*, ScriptFunctionCall&, RefPtr<TypeBuilder::Runtime::RemoteObject>* result, TypeBuilder::OptOutput<bool>* wasThrown, RefPtr<TypeBuilder::Debugger::ExceptionDetails>* = 0);

private:
    String m_name;
    ScriptValue m_injectedScriptObject;
    InspectedStateAccessCheck m_inspectedStateAccessCheck;
};


} // namespace blink

#endif
