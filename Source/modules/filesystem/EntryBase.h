/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef EntryBase_h
#define EntryBase_h

#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

class DOMFileSystemBase;
class EntrySync;

// A common base class for Entry and EntrySync.
class EntryBase : public GarbageCollectedFinalized<EntryBase> {
public:
    virtual ~EntryBase();

    DOMFileSystemBase* filesystem() const { return m_fileSystem.get(); }

    virtual bool isFile() const { return false; }
    virtual bool isDirectory() const { return false; }

    const String& fullPath() const { return m_fullPath; }
    const String& name() const { return m_name; }

    String toURL() const;

    DECLARE_VIRTUAL_TRACE();

protected:
    EntryBase(DOMFileSystemBase*, const String& fullPath);
    friend class EntrySync;

    Member<DOMFileSystemBase> m_fileSystem;

    // This is a virtual path.
    const String m_fullPath;
    const String m_name;

    mutable String m_cachedURL;
};

}

#endif // EntryBase_h
