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
#include "modules/webaudio/AudioDestinationNode.h"

#include "modules/webaudio/AudioContext.h"
#include "modules/webaudio/AudioNodeInput.h"
#include "modules/webaudio/AudioNodeOutput.h"
#include "platform/audio/AudioUtilities.h"
#include "platform/audio/DenormalDisabler.h"

namespace blink {

AudioDestinationHandler::AudioDestinationHandler(AudioNode& node, float sampleRate)
    : AudioHandler(NodeTypeDestination, node, sampleRate)
    , m_currentSampleFrame(0)
{
    addInput();
}

AudioDestinationHandler::~AudioDestinationHandler()
{
    ASSERT(!isInitialized());
}

void AudioDestinationHandler::dispose()
{
    uninitialize();
    AudioHandler::dispose();
}

void AudioDestinationHandler::render(AudioBus* sourceBus, AudioBus* destinationBus, size_t numberOfFrames)
{
    // We don't want denormals slowing down any of the audio processing
    // since they can very seriously hurt performance.
    // This will take care of all AudioNodes because they all process within this scope.
    DenormalDisabler denormalDisabler;

    context()->handler().setAudioThread(currentThread());

    if (!context()->isInitialized()) {
        destinationBus->zero();
        return;
    }

    // Let the context take care of any business at the start of each render quantum.
    context()->handlePreRenderTasks();

    // Prepare the local audio input provider for this render quantum.
    if (sourceBus)
        m_localAudioInputProvider.set(sourceBus);

    ASSERT(numberOfInputs() >= 1);
    if (numberOfInputs() < 1) {
        destinationBus->zero();
        return;
    }
    // This will cause the node(s) connected to us to process, which in turn will pull on their input(s),
    // all the way backwards through the rendering graph.
    AudioBus* renderedBus = input(0)->pull(destinationBus, numberOfFrames);

    if (!renderedBus) {
        destinationBus->zero();
    } else if (renderedBus != destinationBus) {
        // in-place processing was not possible - so copy
        destinationBus->copyFrom(*renderedBus);
    }

    // Process nodes which need a little extra help because they are not connected to anything, but still need to process.
    context()->handler().processAutomaticPullNodes(numberOfFrames);

    // Let the context take care of any business at the end of each render quantum.
    context()->handlePostRenderTasks();

    // Advance current sample-frame.
    m_currentSampleFrame += numberOfFrames;
}

// ----------------------------------------------------------------

AudioDestinationNode::AudioDestinationNode(AudioContext& context)
    : AudioNode(context)
{
}

AudioDestinationHandler& AudioDestinationNode::audioDestinationHandler() const
{
    return static_cast<AudioDestinationHandler&>(handler());
}

unsigned long AudioDestinationNode::maxChannelCount() const
{
    return audioDestinationHandler().maxChannelCount();
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
