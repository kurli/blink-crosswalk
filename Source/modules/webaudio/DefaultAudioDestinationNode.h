/*
 * Copyright (C) 2011, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DefaultAudioDestinationNode_h
#define DefaultAudioDestinationNode_h

#include "modules/webaudio/AudioDestinationNode.h"
#include "platform/audio/AudioDestination.h"
#include "wtf/OwnPtr.h"

namespace blink {

class AudioContext;
class ExceptionState;

class DefaultAudioDestinationHandler final : public AudioDestinationHandler {
public:
    explicit DefaultAudioDestinationHandler(AudioNode&);
    virtual ~DefaultAudioDestinationHandler();

    // AudioHandler
    virtual void dispose() override;
    virtual void initialize() override;
    virtual void uninitialize() override;
    virtual void setChannelCount(unsigned long, ExceptionState&) override;

    // AudioDestinationHandler
    virtual void startRendering() override;
    virtual void stopRendering() override;
    virtual unsigned long maxChannelCount() const override;

private:
    void createDestination();

    OwnPtr<AudioDestination> m_destination;
    String m_inputDeviceId;
    unsigned m_numberOfInputChannels;
};

class DefaultAudioDestinationNode final : public AudioDestinationNode {
public:
    static DefaultAudioDestinationNode* create(AudioContext*);

private:
    explicit DefaultAudioDestinationNode(AudioContext&);
};

} // namespace blink

#endif // DefaultAudioDestinationNode_h
