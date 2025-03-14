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

#ifndef InspectorInputAgent_h
#define InspectorInputAgent_h

#include "core/InspectorFrontend.h"
#include "core/inspector/InspectorBaseAgent.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {
class InspectorPageAgent;
class PlatformKeyboardEvent;
class PlatformMouseEvent;

typedef String ErrorString;

class InspectorInputAgent final : public InspectorBaseAgent<InspectorInputAgent, InspectorFrontend::Input>, public InspectorBackendDispatcher::InputCommandHandler {
    WTF_MAKE_NONCOPYABLE(InspectorInputAgent);
public:
    class Client {
    public:
        virtual ~Client() { }

        virtual void dispatchKeyEvent(const PlatformKeyboardEvent&) { }
        virtual void dispatchMouseEvent(const PlatformMouseEvent&) { }
    };

    static PassOwnPtrWillBeRawPtr<InspectorInputAgent> create(InspectorPageAgent* pageAgent, Client* client)
    {
        return adoptPtrWillBeNoop(new InspectorInputAgent(pageAgent, client));
    }

    virtual ~InspectorInputAgent();
    DECLARE_VIRTUAL_TRACE();

    // Methods called from the frontend for simulating input.
    virtual void dispatchKeyEvent(ErrorString*, const String& type, const int* modifiers, const double* timestamp, const String* text, const String* unmodifiedText, const String* keyIdentifier, const String* code, const int* windowsVirtualKeyCode, const int* nativeVirtualKeyCode, const bool* autoRepeat, const bool* isKeypad, const bool* isSystemKey) override;
    virtual void dispatchMouseEvent(ErrorString*, const String& type, int x, int y, const int* modifiers, const double* timestamp, const String* button, const int* clickCount) override;
    virtual void dispatchTouchEvent(ErrorString*, const String& type, const RefPtr<JSONArray>& touchPoints, const int* modifiers, const double* timestamp) override;
private:
    InspectorInputAgent(InspectorPageAgent*, Client*);

    RawPtrWillBeMember<InspectorPageAgent> m_pageAgent;
    Client* m_client;
};


} // namespace blink

#endif // !defined(InspectorInputAgent_h)
