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

#ifndef AudioNode_h
#define AudioNode_h

#include "modules/EventTargetModules.h"
#include "platform/audio/AudioBus.h"
#include "wtf/Forward.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"

#define DEBUG_AUDIONODE_REFERENCES 0

namespace blink {

class AudioContext;
class AudioNode;
class AudioNodeInput;
class AudioNodeOutput;
class AudioParam;
class ExceptionState;

// An AudioNode is the basic building block for handling audio within an AudioContext.
// It may be an audio source, an intermediate processing module, or an audio destination.
// Each AudioNode can have inputs and/or outputs. An AudioSourceNode has no inputs and a single output.
// An AudioDestinationNode has one input and no outputs and represents the final destination to the audio hardware.
// Most processing nodes such as filters will have one input and one output, although multiple inputs and outputs are possible.

class AudioHandler : public GarbageCollectedFinalized<AudioHandler> {
public:
    enum { ProcessingSizeInFrames = 128 };

    enum NodeType {
        NodeTypeUnknown,
        NodeTypeDestination,
        NodeTypeOscillator,
        NodeTypeAudioBufferSource,
        NodeTypeMediaElementAudioSource,
        NodeTypeMediaStreamAudioDestination,
        NodeTypeMediaStreamAudioSource,
        NodeTypeJavaScript,
        NodeTypeBiquadFilter,
        NodeTypePanner,
        NodeTypeStereoPanner,
        NodeTypeConvolver,
        NodeTypeDelay,
        NodeTypeGain,
        NodeTypeChannelSplitter,
        NodeTypeChannelMerger,
        NodeTypeAnalyser,
        NodeTypeDynamicsCompressor,
        NodeTypeWaveShaper,
        NodeTypeEnd
    };

    AudioHandler(NodeType, AudioNode&, float sampleRate);
    virtual ~AudioHandler();
    // dispose() is called just before the destructor. This must be called in
    // the main thread, and while the graph lock is held.
    virtual void dispose();
    static unsigned instanceCount() { return s_instanceCount; }

    // node() always returns a valid pointer for now.
    AudioNode* node() const { return m_node; }
    AudioContext* context() { return m_context.get(); }
    const AudioContext* context() const { return m_context.get(); }

    enum ChannelCountMode {
        Max,
        ClampedMax,
        Explicit
    };

    NodeType nodeType() const { return m_nodeType; }
    String nodeTypeName() const;

    // This object has been connected to another object. This might have
    // existing connections from others.
    // This function must be called after acquiring a connection reference.
    void makeConnection();
    // This object will be disconnected from another object. This might have
    // remaining connections from others.
    // This function must be called before releasing a connection reference.
    void breakConnection();

    // Can be called from main thread or context's audio thread.  It must be called while the context's graph lock is held.
    void breakConnectionWithLock();

    // The AudioNodeInput(s) (if any) will already have their input data available when process() is called.
    // Subclasses will take this input data and put the results in the AudioBus(s) of its AudioNodeOutput(s) (if any).
    // Called from context's audio thread.
    virtual void process(size_t framesToProcess) = 0;

    // No significant resources should be allocated until initialize() is called.
    // Processing may not occur until a node is initialized.
    virtual void initialize();
    virtual void uninitialize();

    // Clear internal state when the node is disabled. When a node is disabled,
    // it is no longer pulled so any internal state is never updated. But some
    // nodes (DynamicsCompressorNode) have internal state that is still
    // accessible by the user. Update the internal state as if the node were
    // still connected but processing all zeroes. This gives a consistent view
    // to the user.
    virtual void clearInternalStateWhenDisabled();

    bool isInitialized() const { return m_isInitialized; }

    unsigned numberOfInputs() const { return m_inputs.size(); }
    unsigned numberOfOutputs() const { return m_outputs.size(); }

    AudioNodeInput* input(unsigned);
    AudioNodeOutput* output(unsigned);

    // Called from main thread by corresponding JavaScript methods.
    virtual void connect(AudioHandler*, unsigned outputIndex, unsigned inputIndex, ExceptionState&);
    void connect(AudioParam*, unsigned outputIndex, ExceptionState&);

    virtual void disconnect();
    virtual void disconnect(unsigned outputIndex, ExceptionState&);
    virtual void disconnect(AudioHandler*, ExceptionState&);
    virtual void disconnect(AudioHandler*, unsigned outputIndex, ExceptionState&);
    virtual void disconnect(AudioHandler*, unsigned outputIndex, unsigned inputIndex, ExceptionState&);
    virtual void disconnect(AudioParam*, ExceptionState&);
    virtual void disconnect(AudioParam*, unsigned outputIndex, ExceptionState&);

    // Like disconnect, but no exception is thrown if the outputIndex is invalid.  Just do nothing
    // in that case.
    virtual void disconnectWithoutException(unsigned outputIndex);

    virtual float sampleRate() const { return m_sampleRate; }

    // processIfNecessary() is called by our output(s) when the rendering graph needs this AudioNode to process.
    // This method ensures that the AudioNode will only process once per rendering time quantum even if it's called repeatedly.
    // This handles the case of "fanout" where an output is connected to multiple AudioNode inputs.
    // Called from context's audio thread.
    void processIfNecessary(size_t framesToProcess);

    // Called when a new connection has been made to one of our inputs or the connection number of channels has changed.
    // This potentially gives us enough information to perform a lazy initialization or, if necessary, a re-initialization.
    // Called from main thread.
    virtual void checkNumberOfChannelsForInput(AudioNodeInput*);

#if DEBUG_AUDIONODE_REFERENCES
    static void printNodeCounts();
#endif

    // tailTime() is the length of time (not counting latency time) where
    // non-zero output may occur after continuous silent input.
    virtual double tailTime() const;

    // latencyTime() is the length of time it takes for non-zero output to
    // appear after non-zero input is provided. This only applies to processing
    // delay which is an artifact of the processing algorithm chosen and is
    // *not* part of the intrinsic desired effect. For example, a "delay" effect
    // is expected to delay the signal, and thus would not be considered
    // latency.
    virtual double latencyTime() const;

    // propagatesSilence() should return true if the node will generate silent output when given silent input. By default, AudioNode
    // will take tailTime() and latencyTime() into account when determining whether the node will propagate silence.
    virtual bool propagatesSilence() const;
    bool inputsAreSilent();
    void silenceOutputs();
    void unsilenceOutputs();

    void enableOutputsIfNecessary();
    void disableOutputsIfNecessary();

    unsigned long channelCount();
    virtual void setChannelCount(unsigned long, ExceptionState&);

    String channelCountMode();
    virtual void setChannelCountMode(const String&, ExceptionState&);

    String channelInterpretation();
    void setChannelInterpretation(const String&, ExceptionState&);

    ChannelCountMode internalChannelCountMode() const { return m_channelCountMode; }
    AudioBus::ChannelInterpretation internalChannelInterpretation() const { return m_channelInterpretation; }

    void updateChannelCountMode();

    DECLARE_VIRTUAL_TRACE();

protected:
    // Inputs and outputs must be created before the AudioNode is initialized.
    void addInput();
    void addOutput(unsigned numberOfChannels);

    // Called by processIfNecessary() to cause all parts of the rendering graph connected to us to process.
    // Each rendering quantum, the audio data for each of the AudioNode's inputs will be available after this method is called.
    // Called from context's audio thread.
    virtual void pullInputs(size_t framesToProcess);

    // Force all inputs to take any channel interpretation changes into account.
    void updateChannelsForInputs();

private:
    void setNodeType(NodeType);

    volatile bool m_isInitialized;
    NodeType m_nodeType;
    Member<AudioNode> m_node;
    Member<AudioContext> m_context;
    float m_sampleRate;
    Vector<OwnPtr<AudioNodeInput>> m_inputs;
    Vector<OwnPtr<AudioNodeOutput>> m_outputs;
    // Represents audio node graph with Oilpan references. N-th HeapHashSet
    // represents a set of AudioNode objects connected to this AudioNode's N-th
    // output.
    HeapVector<Member<HeapHashSet<Member<AudioHandler>>>> m_connectedNodes;
    // Represents audio node graph with Oilpan references. N-th HeapHashSet
    // represents a set of AudioParam objects connected to this AudioNode's N-th
    // output.
    HeapVector<Member<HeapHashSet<Member<AudioParam>>>> m_connectedParams;

    double m_lastProcessingTime;
    double m_lastNonSilentTime;

    volatile int m_connectionRefCount;

    bool m_isDisabled;

#if DEBUG_AUDIONODE_REFERENCES
    static bool s_isNodeCountInitialized;
    static int s_nodeCount[NodeTypeEnd];
#endif
    static unsigned s_instanceCount;

protected:
    unsigned m_channelCount;
    ChannelCountMode m_channelCountMode;
    AudioBus::ChannelInterpretation m_channelInterpretation;
    // The new channel count mode that will be used to set the actual mode in the pre or post
    // rendering phase.
    ChannelCountMode m_newChannelCountMode;
};

class AudioNode : public RefCountedGarbageCollectedEventTargetWithInlineData<AudioNode> {
    DEFINE_EVENT_TARGET_REFCOUNTING_WILL_BE_REMOVED(RefCountedGarbageCollected<AudioNode>);
    DEFINE_WRAPPERTYPEINFO();
    USING_PRE_FINALIZER(AudioNode, dispose);
public:
    DECLARE_VIRTUAL_TRACE();
    AudioHandler& handler() const;

    void connect(AudioNode*, unsigned outputIndex, unsigned inputIndex, ExceptionState&);
    void connect(AudioParam*, unsigned outputIndex, ExceptionState&);
    void disconnect();
    void disconnect(unsigned outputIndex, ExceptionState&);
    void disconnect(AudioNode*, ExceptionState&);
    void disconnect(AudioNode*, unsigned outputIndex, ExceptionState&);
    void disconnect(AudioNode*, unsigned outputIndex, unsigned inputIndex, ExceptionState&);
    void disconnect(AudioParam*, ExceptionState&);
    void disconnect(AudioParam*, unsigned outputIndex, ExceptionState&);
    AudioContext* context() const;
    unsigned numberOfInputs() const;
    unsigned numberOfOutputs() const;
    unsigned long channelCount() const;
    void setChannelCount(unsigned long, ExceptionState&);
    String channelCountMode() const;
    void setChannelCountMode(const String&, ExceptionState&);
    String channelInterpretation() const;
    void setChannelInterpretation(const String&, ExceptionState&);

    // EventTarget
    virtual const AtomicString& interfaceName() const override final;
    virtual ExecutionContext* executionContext() const override final;

protected:
    explicit AudioNode(AudioContext&);
    // This should be called in a constructor.
    void setHandler(AudioHandler*);

private:
    void dispose();

    Member<AudioContext> m_context;
    Member<AudioHandler> m_handler;
};

} // namespace blink

#endif // AudioNode_h
