/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
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
#include "InstallEvent.h"

#include "modules/serviceworkers/WaitUntilObserver.h"
#include "wtf/RefPtr.h"

namespace blink {

PassRefPtrWillBeRawPtr<InstallEvent> InstallEvent::create()
{
    return adoptRefWillBeNoop(new InstallEvent());
}

PassRefPtrWillBeRawPtr<InstallEvent> InstallEvent::create(const AtomicString& type, const InstallEventInit& initializer)
{
    return adoptRefWillBeNoop(new InstallEvent(type, initializer));
}

PassRefPtrWillBeRawPtr<InstallEvent> InstallEvent::create(const AtomicString& type, const InstallEventInit& initializer, WaitUntilObserver* observer)
{
    return adoptRefWillBeNoop(new InstallEvent(type, initializer, observer));
}

const AtomicString& InstallEvent::interfaceName() const
{
    return EventNames::InstallEvent;
}

InstallEvent::InstallEvent()
{
}

InstallEvent::InstallEvent(const AtomicString& type, const InstallEventInit& initializer)
    : ExtendableEvent(type, initializer)
{
}

InstallEvent::InstallEvent(const AtomicString& type, const InstallEventInit& initializer, WaitUntilObserver* observer)
    : ExtendableEvent(type, initializer, observer)
{
}

DEFINE_TRACE(InstallEvent)
{
    ExtendableEvent::trace(visitor);
}

} // namespace blink
