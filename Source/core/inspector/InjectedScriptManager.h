/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef InjectedScriptManager_h
#define InjectedScriptManager_h

#include "bindings/core/v8/ScriptState.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/text/WTFString.h"
#include <v8.h>

namespace blink {

class InjectedScript;
class InjectedScriptHost;
class InjectedScriptNative;
class ScriptValue;

class InjectedScriptManager : public NoBaseWillBeGarbageCollectedFinalized<InjectedScriptManager> {
    WTF_MAKE_NONCOPYABLE(InjectedScriptManager);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(InjectedScriptManager);
public:
    class CallbackData final : public NoBaseWillBeGarbageCollectedFinalized<CallbackData> {
    public:
        static PassOwnPtrWillBeRawPtr<CallbackData> create(InjectedScriptManager*);
        void dispose();
        DECLARE_TRACE();

        ScopedPersistent<v8::Object> handle;
        RefPtrWillBeMember<InjectedScriptHost> host;
        RawPtrWillBeMember<InjectedScriptManager> injectedScriptManager;
    private:
        explicit CallbackData(InjectedScriptManager*);
    };

    static PassOwnPtrWillBeRawPtr<InjectedScriptManager> createForPage();
    static PassOwnPtrWillBeRawPtr<InjectedScriptManager> createForWorker();
    ~InjectedScriptManager();
    DECLARE_TRACE();

    void disconnect();

    InjectedScriptHost* injectedScriptHost();

    InjectedScript injectedScriptFor(ScriptState*);
    InjectedScript injectedScriptForId(int);
    int injectedScriptIdFor(ScriptState*);
    InjectedScript injectedScriptForObjectId(const String& objectId);
    void discardInjectedScripts();
    void discardInjectedScriptFor(ScriptState*);
    void releaseObjectGroup(const String& objectGroup);

    typedef bool (*InspectedStateAccessCheck)(ScriptState*);
    InspectedStateAccessCheck inspectedStateAccessCheck() const { return m_inspectedStateAccessCheck; }

    static void setWeakCallback(const v8::WeakCallbackData<v8::Object, CallbackData>&);
    CallbackData* createCallbackData(InjectedScriptManager*);
    void removeCallbackData(CallbackData*);
    void setCustomObjectFormatterEnabled(bool);

private:
    explicit InjectedScriptManager(InspectedStateAccessCheck);

    String injectedScriptSource();
    ScriptValue createInjectedScript(const String& source, ScriptState*, int id, InjectedScriptNative*);

    static bool canAccessInspectedWindow(ScriptState*);
    static bool canAccessInspectedWorkerGlobalScope(ScriptState*);

    int m_nextInjectedScriptId;
    typedef HashMap<int, InjectedScript> IdToInjectedScriptMap;
    IdToInjectedScriptMap m_idToInjectedScript;
    RefPtrWillBeMember<InjectedScriptHost> m_injectedScriptHost;
    InspectedStateAccessCheck m_inspectedStateAccessCheck;
    typedef HashMap<RefPtr<ScriptState>, int> ScriptStateToId;
    ScriptStateToId m_scriptStateToId;
    WillBeHeapHashSet<OwnPtrWillBeMember<CallbackData>> m_callbackDataSet;
    bool m_customObjectFormatterEnabled;
};

} // namespace blink

#endif // !defined(InjectedScriptManager_h)
