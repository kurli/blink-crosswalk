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

#ifndef AudioNodeInput_h
#define AudioNodeInput_h

#include "modules/webaudio/AudioNode.h"
#include "modules/webaudio/AudioSummingJunction.h"
#include "platform/audio/AudioBus.h"
#include "wtf/HashSet.h"

namespace blink {

class AudioNodeOutput;

// An AudioNodeInput represents an input to an AudioNode and can be connected from one or more AudioNodeOutputs.
// In the case of multiple connections, the input will act as a unity-gain summing junction, mixing all the outputs.
// The number of channels of the input's bus is the maximum of the number of channels of all its connections.

class AudioNodeInput final : public AudioSummingJunction {
public:
    static PassOwnPtr<AudioNodeInput> create(AudioHandler&);

    // AudioSummingJunction
    virtual void didUpdate() override;

    // Can be called from any thread.
    // TODO(tkent): Rename to m_handler.
    AudioHandler& node() const { return m_handler; }

    // Must be called with the context's graph lock.
    void connect(AudioNodeOutput&);
    void disconnect(AudioNodeOutput&);

    // disable() will take the output out of the active connections list and set aside in a disabled list.
    // enable() will put the output back into the active connections list.
    // Must be called with the context's graph lock.
    void enable(AudioNodeOutput&);
    void disable(AudioNodeOutput&);

    // pull() processes all of the AudioNodes connected to us.
    // In the case of multiple connections it sums the result into an internal summing bus.
    // In the single connection case, it allows in-place processing where possible using inPlaceBus.
    // It returns the bus which it rendered into, returning inPlaceBus if in-place processing was performed.
    // Called from context's audio thread.
    AudioBus* pull(AudioBus* inPlaceBus, size_t framesToProcess);

    // bus() contains the rendered audio after pull() has been called for each time quantum.
    // Called from context's audio thread.
    AudioBus* bus();

    // updateInternalBus() updates m_internalSummingBus appropriately for the number of channels.
    // This must be called when we own the context's graph lock in the audio thread at the very start or end of the render quantum.
    void updateInternalBus();

    // The number of channels of the connection with the largest number of channels.
    unsigned numberOfChannels() const;

private:
    explicit AudioNodeInput(AudioHandler&);

    // This reference to an Oilpan object is safe because the AudioNode owns
    // this AudioNodeInput object.
    AudioHandler& m_handler;

    // m_disabledOutputs contains the AudioNodeOutputs which are disabled (will
    // not be processed) by the audio graph rendering.  But, from JavaScript's
    // perspective, these outputs are still connected to us.
    // Generally, these represent disabled connections from "notes" which have
    // finished playing but are not yet garbage collected.
    // These raw pointers are safe. Owner AudioNodes of these AudioNodeOutputs
    // manage their lifetime, and AudioNode::dispose() disconnects all of
    // connections.
    HashSet<AudioNodeOutput*> m_disabledOutputs;

    // Called from context's audio thread.
    AudioBus* internalSummingBus();
    void sumAllConnections(AudioBus* summingBus, size_t framesToProcess);

    RefPtr<AudioBus> m_internalSummingBus;
};

} // namespace blink

#endif // AudioNodeInput_h
