// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FrameClient_h
#define FrameClient_h

namespace blink {

class Frame;
class LocalFrame;
class MessageEvent;
class SecurityOrigin;

class FrameClient {
public:
    virtual void willBeDetached() = 0;
    virtual void detached() = 0;

    virtual Frame* opener() const = 0;
    virtual void setOpener(Frame*) = 0;

    virtual Frame* parent() const = 0;
    virtual Frame* top() const = 0;
    virtual Frame* previousSibling() const = 0;
    virtual Frame* nextSibling() const = 0;
    virtual Frame* firstChild() const = 0;
    virtual Frame* lastChild() const = 0;

    // Returns true if the embedder intercepted the postMessage call
    virtual bool willCheckAndDispatchMessageEvent(SecurityOrigin* /*target*/, MessageEvent*, LocalFrame* /*sourceFrame*/) const { return false; }

    virtual ~FrameClient() { }
};

} // namespace blink

#endif // FrameClient_h
