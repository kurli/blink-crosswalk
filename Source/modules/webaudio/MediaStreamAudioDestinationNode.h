/*
 * Copyright (C) 2012, Google Inc. All rights reserved.
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

#ifndef MediaStreamAudioDestinationNode_h
#define MediaStreamAudioDestinationNode_h

#if ENABLE(WEB_AUDIO)

#include "modules/mediastream/MediaStream.h"
#include "modules/webaudio/AudioBasicInspectorNode.h"
#include "platform/audio/AudioBus.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class AudioContext;

class MediaStreamAudioDestinationHandler final : public AudioBasicInspectorHandler {
public:
    MediaStreamAudioDestinationHandler(AudioNode&, size_t numberOfChannels);
    virtual ~MediaStreamAudioDestinationHandler();
    DECLARE_VIRTUAL_TRACE();

    MediaStream* stream() { return m_stream.get(); }

    // AudioHandler.
    virtual void dispose() override;
    virtual void process(size_t framesToProcess) override;

private:
    // As an audio source, we will never propagate silence.
    virtual bool propagatesSilence() const override { return false; }

    Member<MediaStream> m_stream;
    RefPtr<MediaStreamSource> m_source;
    RefPtr<AudioBus> m_mixBus;
};

class MediaStreamAudioDestinationNode final : public AudioNode {
    DEFINE_WRAPPERTYPEINFO();
public:
    static MediaStreamAudioDestinationNode* create(AudioContext*, size_t numberOfChannels);
    MediaStream* stream() const;

private:
    MediaStreamAudioDestinationNode(AudioContext&, size_t numberOfChannels);
};

} // namespace blink

#endif // ENABLE(WEB_AUDIO)

#endif // MediaStreamAudioDestinationNode_h
