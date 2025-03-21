/*
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2008 Collabora, Ltd.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PlatformKeyboardEvent_h
#define PlatformKeyboardEvent_h

#include "platform/PlatformEvent.h"
#include "platform/PlatformExport.h"
#include "wtf/text/WTFString.h"

namespace blink {

class PlatformKeyboardEvent : public PlatformEvent {
    WTF_MAKE_FAST_ALLOCATED(PlatformKeyboardEvent);
public:
    PlatformKeyboardEvent()
        : PlatformEvent(PlatformEvent::KeyDown)
        , m_windowsVirtualKeyCode(0)
        , m_nativeVirtualKeyCode(0)
        , m_autoRepeat(false)
        , m_isKeypad(false)
        , m_isSystemKey(false)
    {
    }

    PlatformKeyboardEvent(Type type, const String& text, const String& unmodifiedText, const String& keyIdentifier, const String& code, int windowsVirtualKeyCode, int nativeVirtualKeyCode, bool isAutoRepeat, bool isKeypad, bool isSystemKey, Modifiers modifiers, double timestamp)
        : PlatformEvent(type, modifiers, timestamp)
        , m_text(text)
        , m_unmodifiedText(unmodifiedText)
        , m_keyIdentifier(keyIdentifier)
        , m_code(code)
        , m_windowsVirtualKeyCode(windowsVirtualKeyCode)
        , m_nativeVirtualKeyCode(nativeVirtualKeyCode)
        , m_autoRepeat(isAutoRepeat)
        , m_isKeypad(isKeypad)
        , m_isSystemKey(isSystemKey)
    {
    }

    PLATFORM_EXPORT void disambiguateKeyDownEvent(Type);

    // Text as as generated by processing a virtual key code with a keyboard layout
    // (in most cases, just a character code, but the layout can emit several
    // characters in a single keypress event on some platforms).
    // This may bear no resemblance to the ultimately inserted text if an input method
    // processes the input.
    // Will be null for KeyUp and RawKeyDown events.
    String text() const { return m_text; }

    // Text that would have been generated by the keyboard if no modifiers were pressed
    // (except for Shift); useful for shortcut (accelerator) key handling.
    // Otherwise, same as text().
    String unmodifiedText() const { return m_unmodifiedText; }

    String keyIdentifier() const { return m_keyIdentifier; }

    String code() const { return m_code; }

    // Most compatible Windows virtual key code associated with the event.
    // Zero for Char events.
    int windowsVirtualKeyCode() const { return m_windowsVirtualKeyCode; }

    int nativeVirtualKeyCode() const { return m_nativeVirtualKeyCode; }

    bool isAutoRepeat() const { return m_autoRepeat; }
    bool isKeypad() const { return m_isKeypad; }
    bool isSystemKey() const { return m_isSystemKey; }

    PLATFORM_EXPORT static bool currentCapsLockState();
    PLATFORM_EXPORT static void getCurrentModifierState(bool& shiftKey, bool& ctrlKey, bool& altKey, bool& metaKey);

protected:
    String m_text;
    String m_unmodifiedText;
    String m_keyIdentifier;
    String m_code;
    int m_windowsVirtualKeyCode;
    int m_nativeVirtualKeyCode;
    bool m_autoRepeat;
    bool m_isKeypad;
    bool m_isSystemKey;
};

} // namespace blink

#endif // PlatformKeyboardEvent_h
