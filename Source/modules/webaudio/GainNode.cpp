/*
 * Copyright (C) 2010, Google Inc. All rights reserved.
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

#include "config.h"
#if ENABLE(WEB_AUDIO)
#include "modules/webaudio/GainNode.h"

#include "modules/webaudio/AudioNodeInput.h"
#include "modules/webaudio/AudioNodeOutput.h"
#include "platform/audio/AudioBus.h"

namespace blink {

GainHandler::GainHandler(AudioNode& node, float sampleRate)
    : AudioHandler(NodeTypeGain, node, sampleRate)
    , m_lastGain(1.0)
    , m_sampleAccurateGainValues(ProcessingSizeInFrames) // FIXME: can probably share temp buffer in context
{
    m_gain = AudioParam::create(context(), 1.0);

    addInput();
    addOutput(1);

    initialize();
}

void GainHandler::process(size_t framesToProcess)
{
    // FIXME: for some cases there is a nice optimization to avoid processing here, and let the gain change
    // happen in the summing junction input of the AudioNode we're connected to.
    // Then we can avoid all of the following:

    AudioBus* outputBus = output(0)->bus();
    ASSERT(outputBus);

    if (!isInitialized() || !input(0)->isConnected()) {
        outputBus->zero();
    } else {
        AudioBus* inputBus = input(0)->bus();

        if (gain()->handler().hasSampleAccurateValues()) {
            // Apply sample-accurate gain scaling for precise envelopes, grain windows, etc.
            ASSERT(framesToProcess <= m_sampleAccurateGainValues.size());
            if (framesToProcess <= m_sampleAccurateGainValues.size()) {
                float* gainValues = m_sampleAccurateGainValues.data();
                gain()->handler().calculateSampleAccurateValues(gainValues, framesToProcess);
                outputBus->copyWithSampleAccurateGainValuesFrom(*inputBus, gainValues, framesToProcess);
            }
        } else {
            // Apply the gain with de-zippering into the output bus.
            outputBus->copyWithGainFrom(*inputBus, &m_lastGain, gain()->value());
        }
    }
}

// FIXME: this can go away when we do mixing with gain directly in summing junction of AudioNodeInput
//
// As soon as we know the channel count of our input, we can lazily initialize.
// Sometimes this may be called more than once with different channel counts, in which case we must safely
// uninitialize and then re-initialize with the new channel count.
void GainHandler::checkNumberOfChannelsForInput(AudioNodeInput* input)
{
    ASSERT(context()->isAudioThread());
    ASSERT(context()->isGraphOwner());

    ASSERT(input);
    ASSERT(input == this->input(0));
    if (input != this->input(0))
        return;

    unsigned numberOfChannels = input->numberOfChannels();

    if (isInitialized() && numberOfChannels != output(0)->numberOfChannels()) {
        // We're already initialized but the channel count has changed.
        uninitialize();
    }

    if (!isInitialized()) {
        // This will propagate the channel count to any nodes connected further downstream in the graph.
        output(0)->setNumberOfChannels(numberOfChannels);
        initialize();
    }

    AudioHandler::checkNumberOfChannelsForInput(input);
}

DEFINE_TRACE(GainHandler)
{
    visitor->trace(m_gain);
    AudioHandler::trace(visitor);
}

// ----------------------------------------------------------------

GainNode::GainNode(AudioContext& context, float sampelRate)
    : AudioNode(context)
{
    setHandler(new GainHandler(*this, sampelRate));
}

GainNode* GainNode::create(AudioContext* context, float sampleRate)
{
    return new GainNode(*context, sampleRate);
}

AudioParam* GainNode::gain() const
{
    return static_cast<GainHandler&>(handler()).gain();
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
