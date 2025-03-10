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
#include "modules/webaudio/AudioContext.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContextTask.h"
#include "core/html/HTMLMediaElement.h"
#include "core/inspector/ScriptCallStack.h"
#include "modules/mediastream/MediaStream.h"
#include "modules/webaudio/AnalyserNode.h"
#include "modules/webaudio/AudioBuffer.h"
#include "modules/webaudio/AudioBufferCallback.h"
#include "modules/webaudio/AudioBufferSourceNode.h"
#include "modules/webaudio/AudioListener.h"
#include "modules/webaudio/AudioNodeInput.h"
#include "modules/webaudio/AudioNodeOutput.h"
#include "modules/webaudio/BiquadFilterNode.h"
#include "modules/webaudio/ChannelMergerNode.h"
#include "modules/webaudio/ChannelSplitterNode.h"
#include "modules/webaudio/ConvolverNode.h"
#include "modules/webaudio/DefaultAudioDestinationNode.h"
#include "modules/webaudio/DelayNode.h"
#include "modules/webaudio/DynamicsCompressorNode.h"
#include "modules/webaudio/GainNode.h"
#include "modules/webaudio/MediaElementAudioSourceNode.h"
#include "modules/webaudio/MediaStreamAudioDestinationNode.h"
#include "modules/webaudio/MediaStreamAudioSourceNode.h"
#include "modules/webaudio/OfflineAudioCompletionEvent.h"
#include "modules/webaudio/OfflineAudioContext.h"
#include "modules/webaudio/OfflineAudioDestinationNode.h"
#include "modules/webaudio/OscillatorNode.h"
#include "modules/webaudio/PannerNode.h"
#include "modules/webaudio/PeriodicWave.h"
#include "modules/webaudio/ScriptProcessorNode.h"
#include "modules/webaudio/StereoPannerNode.h"
#include "modules/webaudio/WaveShaperNode.h"
#include "platform/audio/FFTFrame.h"
#include "platform/audio/HRTFPanner.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/Platform.h"
#include "wtf/Atomics.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

#if DEBUG_AUDIONODE_REFERENCES
#include <stdio.h>
#endif

namespace blink {

// Don't allow more than this number of simultaneous AudioContexts talking to hardware.
const unsigned MaxHardwareContexts = 6;
unsigned AudioContext::s_hardwareContextCount = 0;
unsigned AudioContext::s_contextId = 0;

AudioContext* AudioContext::create(Document& document, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (s_hardwareContextCount >= MaxHardwareContexts) {
        exceptionState.throwDOMException(
            NotSupportedError,
            ExceptionMessages::indexExceedsMaximumBound(
                "number of hardware contexts",
                s_hardwareContextCount,
                MaxHardwareContexts));
        return nullptr;
    }

    AudioContext* audioContext = new AudioContext(&document);
    audioContext->suspendIfNeeded();
    return audioContext;
}

// Constructor for rendering to the audio hardware.
AudioContext::AudioContext(Document* document)
    : ActiveDOMObject(document)
    , m_isStopScheduled(false)
    , m_isCleared(false)
    , m_isInitialized(false)
    , m_destinationNode(nullptr)
    , m_isResolvingResumePromises(false)
    , m_connectionCount(0)
    , m_didInitializeContextGraphMutex(false)
    , m_deferredTaskHandler(DeferredTaskHandler::create())
    , m_isOfflineContext(false)
    , m_contextState(Suspended)
    , m_cachedSampleFrame(0)
{
    m_didInitializeContextGraphMutex = true;
    m_destinationNode = DefaultAudioDestinationNode::create(this);

    initialize();
}

// Constructor for offline (non-realtime) rendering.
AudioContext::AudioContext(Document* document, unsigned numberOfChannels, size_t numberOfFrames, float sampleRate)
    : ActiveDOMObject(document)
    , m_isStopScheduled(false)
    , m_isCleared(false)
    , m_isInitialized(false)
    , m_destinationNode(nullptr)
    , m_isResolvingResumePromises(false)
    , m_connectionCount(0)
    , m_didInitializeContextGraphMutex(false)
    , m_deferredTaskHandler(DeferredTaskHandler::create())
    , m_isOfflineContext(true)
    , m_contextState(Suspended)
    , m_cachedSampleFrame(0)
{
    m_didInitializeContextGraphMutex = true;
    // Create a new destination for offline rendering.
    m_renderTarget = AudioBuffer::create(numberOfChannels, numberOfFrames, sampleRate);
    if (m_renderTarget.get())
        m_destinationNode = OfflineAudioDestinationNode::create(this, m_renderTarget.get());

    initialize();
}

AudioContext::~AudioContext()
{
#if DEBUG_AUDIONODE_REFERENCES
    fprintf(stderr, "%p: AudioContext::~AudioContext(): %u\n", this, m_contextId);
#endif
    // AudioNodes keep a reference to their context, so there should be no way to be in the destructor if there are still AudioNodes around.
    ASSERT(!m_isInitialized);
    ASSERT(!m_referencedNodes.size());
    ASSERT(!m_finishedNodes.size());
    ASSERT(!m_suspendResolvers.size());
    ASSERT(!m_isResolvingResumePromises);
    ASSERT(!m_resumeResolvers.size());
}

void AudioContext::initialize()
{
    if (isInitialized())
        return;

    FFTFrame::initialize();
    m_listener = AudioListener::create();

    if (m_destinationNode.get()) {
        m_destinationNode->handler().initialize();

        if (!isOfflineContext()) {
            // This starts the audio thread. The destination node's provideInput() method will now be called repeatedly to render audio.
            // Each time provideInput() is called, a portion of the audio stream is rendered. Let's call this time period a "render quantum".
            // NOTE: for now default AudioContext does not need an explicit startRendering() call from JavaScript.
            // We may want to consider requiring it for symmetry with OfflineAudioContext.
            startRendering();
            ++s_hardwareContextCount;
        }

        m_contextId = s_contextId++;
        m_isInitialized = true;
#if DEBUG_AUDIONODE_REFERENCES
        fprintf(stderr, "%p: AudioContext::AudioContext(): %u #%u\n",
            this, m_contextId, AudioContext::s_hardwareContextCount);
#endif
    }
}

void AudioContext::clear()
{
    // We need to run disposers before destructing m_contextGraphMutex.
    m_liveNodes.clear();
    m_destinationNode.clear();
    m_isCleared = true;
}

void AudioContext::uninitialize()
{
    ASSERT(isMainThread());

    if (!isInitialized())
        return;

    m_isInitialized = false;

    // This stops the audio thread and all audio rendering.
    if (m_destinationNode)
        m_destinationNode->handler().uninitialize();

    if (!isOfflineContext()) {
        ASSERT(s_hardwareContextCount);
        --s_hardwareContextCount;
    }

    // Get rid of the sources which may still be playing.
    derefUnfinishedSourceNodes();

    // Reject any pending resolvers before we go away.
    rejectPendingResolvers();

    // For an offline audio context, the completion event will set the state to closed.  For an
    // online context, we need to do it here.  We only want to set the closed state once.
    if (!isOfflineContext())
        setContextState(Closed);

    // Resolve the promise now, if any
    if (m_closeResolver)
        m_closeResolver->resolve();

    ASSERT(m_listener);
    m_listener->waitForHRTFDatabaseLoaderThreadCompletion();

    clear();
}

void AudioContext::stop()
{
    // Usually ExecutionContext calls stop twice.
    if (m_isStopScheduled)
        return;
    m_isStopScheduled = true;

    // Don't call uninitialize() immediately here because the ExecutionContext is in the middle
    // of dealing with all of its ActiveDOMObjects at this point. uninitialize() can de-reference other
    // ActiveDOMObjects so let's schedule uninitialize() to be called later.
    // FIXME: see if there's a more direct way to handle this issue.
    Platform::current()->mainThread()->postTask(FROM_HERE, bind(&AudioContext::uninitialize, this));
}

bool AudioContext::hasPendingActivity() const
{
    // There's no pending activity if the audio context has been cleared.
    return !m_isCleared;
}

void AudioContext::throwExceptionForClosedState(ExceptionState& exceptionState)
{
    exceptionState.throwDOMException(InvalidStateError, "AudioContext has been closed.");
}

AudioBuffer* AudioContext::createBuffer(unsigned numberOfChannels, size_t numberOfFrames, float sampleRate, ExceptionState& exceptionState)
{
    // It's ok to call createBuffer, even if the context is closed because the AudioBuffer doesn't
    // really "belong" to any particular context.

    return AudioBuffer::create(numberOfChannels, numberOfFrames, sampleRate, exceptionState);
}

void AudioContext::decodeAudioData(DOMArrayBuffer* audioData, AudioBufferCallback* successCallback, AudioBufferCallback* errorCallback, ExceptionState& exceptionState)
{
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return;
    }

    if (!audioData) {
        exceptionState.throwDOMException(
            SyntaxError,
            "invalid ArrayBuffer for audioData.");
        return;
    }
    m_audioDecoder.decodeAsync(audioData, sampleRate(), successCallback, errorCallback);
}

AudioBufferSourceNode* AudioContext::createBufferSource(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    AudioBufferSourceNode* node = AudioBufferSourceNode::create(this, sampleRate());

    // Do not add a reference to this source node now. The reference will be added when start() is
    // called.

    return node;
}

MediaElementAudioSourceNode* AudioContext::createMediaElementSource(HTMLMediaElement* mediaElement, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    if (!mediaElement) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "invalid HTMLMedialElement.");
        return nullptr;
    }

    // First check if this media element already has a source node.
    if (mediaElement->audioSourceNode()) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "HTMLMediaElement already connected previously to a different MediaElementSourceNode.");
        return nullptr;
    }

    MediaElementAudioSourceNode* node = MediaElementAudioSourceNode::create(this, mediaElement);

    mediaElement->setAudioSourceNode(&node->mediaElementAudioSourceHandler());

    refNode(node); // context keeps reference until node is disconnected
    return node;
}

MediaStreamAudioSourceNode* AudioContext::createMediaStreamSource(MediaStream* mediaStream, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    if (!mediaStream) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "invalid MediaStream source");
        return nullptr;
    }

    MediaStreamTrackVector audioTracks = mediaStream->getAudioTracks();
    if (audioTracks.isEmpty()) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "MediaStream has no audio track");
        return nullptr;
    }

    // Use the first audio track in the media stream.
    MediaStreamTrack* audioTrack = audioTracks[0];
    OwnPtr<AudioSourceProvider> provider = audioTrack->createWebAudioSource();
    MediaStreamAudioSourceNode* node = MediaStreamAudioSourceNode::create(this, mediaStream, audioTrack, provider.release());

    // FIXME: Only stereo streams are supported right now. We should be able to accept multi-channel streams.
    node->mediaStreamAudioSourceHandler().setFormat(2, sampleRate());

    refNode(node); // context keeps reference until node is disconnected
    return node;
}

MediaStreamAudioDestinationNode* AudioContext::createMediaStreamDestination(ExceptionState& exceptionState)
{
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    // Set number of output channels to stereo by default.
    return MediaStreamAudioDestinationNode::create(this, 2);
}

ScriptProcessorNode* AudioContext::createScriptProcessor(ExceptionState& exceptionState)
{
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    // Set number of input/output channels to stereo by default.
    return createScriptProcessor(0, 2, 2, exceptionState);
}

ScriptProcessorNode* AudioContext::createScriptProcessor(size_t bufferSize, ExceptionState& exceptionState)
{
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    // Set number of input/output channels to stereo by default.
    return createScriptProcessor(bufferSize, 2, 2, exceptionState);
}

ScriptProcessorNode* AudioContext::createScriptProcessor(size_t bufferSize, size_t numberOfInputChannels, ExceptionState& exceptionState)
{
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    // Set number of output channels to stereo by default.
    return createScriptProcessor(bufferSize, numberOfInputChannels, 2, exceptionState);
}

ScriptProcessorNode* AudioContext::createScriptProcessor(size_t bufferSize, size_t numberOfInputChannels, size_t numberOfOutputChannels, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    ScriptProcessorNode* node = ScriptProcessorNode::create(this, sampleRate(), bufferSize, numberOfInputChannels, numberOfOutputChannels);

    if (!node) {
        if (!numberOfInputChannels && !numberOfOutputChannels) {
            exceptionState.throwDOMException(
                IndexSizeError,
                "number of input channels and output channels cannot both be zero.");
        } else if (numberOfInputChannels > AudioContext::maxNumberOfChannels()) {
            exceptionState.throwDOMException(
                IndexSizeError,
                "number of input channels (" + String::number(numberOfInputChannels)
                + ") exceeds maximum ("
                + String::number(AudioContext::maxNumberOfChannels()) + ").");
        } else if (numberOfOutputChannels > AudioContext::maxNumberOfChannels()) {
            exceptionState.throwDOMException(
                IndexSizeError,
                "number of output channels (" + String::number(numberOfInputChannels)
                + ") exceeds maximum ("
                + String::number(AudioContext::maxNumberOfChannels()) + ").");
        } else {
            exceptionState.throwDOMException(
                IndexSizeError,
                "buffer size (" + String::number(bufferSize)
                + ") must be a power of two between 256 and 16384.");
        }
        return nullptr;
    }

    refNode(node); // context keeps reference until we stop making javascript rendering callbacks
    return node;
}

StereoPannerNode* AudioContext::createStereoPanner(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return StereoPannerNode::create(this, sampleRate());
}

BiquadFilterNode* AudioContext::createBiquadFilter(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return BiquadFilterNode::create(this, sampleRate());
}

WaveShaperNode* AudioContext::createWaveShaper(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return WaveShaperNode::create(this);
}

PannerNode* AudioContext::createPanner(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return PannerNode::create(this, sampleRate());
}

ConvolverNode* AudioContext::createConvolver(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return ConvolverNode::create(this, sampleRate());
}

DynamicsCompressorNode* AudioContext::createDynamicsCompressor(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return DynamicsCompressorNode::create(this, sampleRate());
}

AnalyserNode* AudioContext::createAnalyser(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return AnalyserNode::create(this, sampleRate());
}

GainNode* AudioContext::createGain(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return GainNode::create(this, sampleRate());
}

DelayNode* AudioContext::createDelay(ExceptionState& exceptionState)
{
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    const double defaultMaxDelayTime = 1;
    return createDelay(defaultMaxDelayTime, exceptionState);
}

DelayNode* AudioContext::createDelay(double maxDelayTime, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    DelayNode* node = DelayNode::create(this, sampleRate(), maxDelayTime, exceptionState);
    if (exceptionState.hadException())
        return nullptr;
    return node;
}

ChannelSplitterNode* AudioContext::createChannelSplitter(ExceptionState& exceptionState)
{
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    const unsigned ChannelSplitterDefaultNumberOfOutputs = 6;
    return createChannelSplitter(ChannelSplitterDefaultNumberOfOutputs, exceptionState);
}

ChannelSplitterNode* AudioContext::createChannelSplitter(size_t numberOfOutputs, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    ChannelSplitterNode* node = ChannelSplitterNode::create(this, sampleRate(), numberOfOutputs);

    if (!node) {
        exceptionState.throwDOMException(
            IndexSizeError,
            "number of outputs (" + String::number(numberOfOutputs)
            + ") must be between 1 and "
            + String::number(AudioContext::maxNumberOfChannels()) + ".");
        return nullptr;
    }

    return node;
}

ChannelMergerNode* AudioContext::createChannelMerger(ExceptionState& exceptionState)
{
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    const unsigned ChannelMergerDefaultNumberOfInputs = 6;
    return createChannelMerger(ChannelMergerDefaultNumberOfInputs, exceptionState);
}

ChannelMergerNode* AudioContext::createChannelMerger(size_t numberOfInputs, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    ChannelMergerNode* node = ChannelMergerNode::create(this, sampleRate(), numberOfInputs);

    if (!node) {
        exceptionState.throwDOMException(
            IndexSizeError,
            "number of inputs (" + String::number(numberOfInputs)
            + ") must be between 1 and "
            + String::number(AudioContext::maxNumberOfChannels()) + ".");
        return nullptr;
    }

    return node;
}

OscillatorNode* AudioContext::createOscillator(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    OscillatorNode* node = OscillatorNode::create(this, sampleRate());

    // Do not add a reference to this source node now. The reference will be added when start() is
    // called.

    return node;
}

PeriodicWave* AudioContext::createPeriodicWave(DOMFloat32Array* real, DOMFloat32Array* imag, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    if (!real) {
        exceptionState.throwDOMException(
            SyntaxError,
            "invalid real array");
        return nullptr;
    }

    if (!imag) {
        exceptionState.throwDOMException(
            SyntaxError,
            "invalid imaginary array");
        return nullptr;
    }

    if (real->length() > PeriodicWave::kMaxPeriodicWaveArraySize) {
        exceptionState.throwDOMException(
            IndexSizeError,
            ExceptionMessages::indexOutsideRange(
                "length of the real part array",
                real->length(),
                1u,
                ExceptionMessages::InclusiveBound,
                PeriodicWave::kMaxPeriodicWaveArraySize,
                ExceptionMessages::InclusiveBound));
        return nullptr;
    }

    if (imag->length() > PeriodicWave::kMaxPeriodicWaveArraySize) {
        exceptionState.throwDOMException(
            IndexSizeError,
            ExceptionMessages::indexOutsideRange(
                "length of the imaginary part array",
                imag->length(),
                1u,
                ExceptionMessages::InclusiveBound,
                PeriodicWave::kMaxPeriodicWaveArraySize,
                ExceptionMessages::InclusiveBound));
        return nullptr;
    }

    if (real->length() != imag->length()) {
        exceptionState.throwDOMException(
            IndexSizeError,
            "length of real array (" + String::number(real->length())
            + ") and length of imaginary array (" +  String::number(imag->length())
            + ") must match.");
        return nullptr;
    }

    return PeriodicWave::create(sampleRate(), real, imag);
}

size_t AudioContext::currentSampleFrame() const
{
    if (isAudioThread())
        return m_destinationNode ? m_destinationNode->audioDestinationHandler().currentSampleFrame() : 0;

    return m_cachedSampleFrame;
}

double AudioContext::currentTime() const
{
    if (isAudioThread())
        return m_destinationNode ? m_destinationNode->audioDestinationHandler().currentTime() : 0;

    return m_cachedSampleFrame / static_cast<double>(sampleRate());
}

String AudioContext::state() const
{
    // These strings had better match the strings for AudioContextState in AudioContext.idl.
    switch (m_contextState) {
    case Suspended:
        return "suspended";
    case Running:
        return "running";
    case Closed:
        return "closed";
    }
    ASSERT_NOT_REACHED();
    return "";
}

void AudioContext::setContextState(AudioContextState newState)
{
    ASSERT(isMainThread());

    // Validate the transitions.  The valid transitions are Suspended->Running, Running->Suspended,
    // and anything->Closed.
    switch (newState) {
    case Suspended:
        ASSERT(m_contextState == Running);
        break;
    case Running:
        ASSERT(m_contextState == Suspended);
        break;
    case Closed:
        ASSERT(m_contextState != Closed);
        break;
    }

    if (newState == m_contextState) {
        // ASSERTs above failed; just return.
        return;
    }

    m_contextState = newState;

    // Notify context that state changed
    if (executionContext())
        executionContext()->postTask(FROM_HERE, createSameThreadTask(&AudioContext::notifyStateChange, this));
}

void AudioContext::notifyStateChange()
{
    dispatchEvent(Event::create(EventTypeNames::statechange));
}

ScriptPromise AudioContext::suspendContext(ScriptState* scriptState)
{
    ASSERT(isMainThread());
    AutoLocker locker(this);

    if (isOfflineContext()) {
        return ScriptPromise::rejectWithDOMException(
            scriptState,
            DOMException::create(
                InvalidStateError,
                "cannot suspend an OfflineAudioContext"));
    }

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    // Save the resolver which will get resolved at the end of the rendering quantum.
    m_suspendResolvers.append(resolver);

    return promise;
}

ScriptPromise AudioContext::resumeContext(ScriptState* scriptState)
{
    ASSERT(isMainThread());
    AutoLocker locker(this);

    if (isOfflineContext()) {
        return ScriptPromise::rejectWithDOMException(
            scriptState,
            DOMException::create(
                InvalidStateError,
                "cannot resume an OfflineAudioContext"));
    }

    if (isContextClosed()) {
        return ScriptPromise::rejectWithDOMException(
            scriptState,
            DOMException::create(
                InvalidStateError,
                "cannot resume a closed AudioContext"));
    }

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    // Restart the destination node to pull on the audio graph.
    if (m_destinationNode)
        startRendering();

    // Save the resolver which will get resolved when the destination node starts pulling on the
    // graph again.
    m_resumeResolvers.append(resolver);

    return promise;
}

void AudioContext::notifyNodeStartedProcessing(AudioNode* node)
{
    refNode(node);
}

void AudioContext::notifyNodeFinishedProcessing(AudioNode* node)
{
    ASSERT(isAudioThread());
    m_finishedNodes.append(node);
}

void AudioContext::derefFinishedSourceNodes()
{
    ASSERT(isGraphOwner());
    ASSERT(isAudioThread());
    for (unsigned i = 0; i < m_finishedNodes.size(); ++i)
        derefNode(m_finishedNodes[i]);

    m_finishedNodes.clear();
}

void AudioContext::refNode(AudioNode* node)
{
    ASSERT(isMainThread());
    AutoLocker locker(this);

    m_referencedNodes.append(node);
    node->handler().makeConnection();
}

void AudioContext::derefNode(AudioNode* node)
{
    ASSERT(isGraphOwner());

    for (unsigned i = 0; i < m_referencedNodes.size(); ++i) {
        if (node == m_referencedNodes.at(i).get()) {
            node->handler().breakConnection();
            m_referencedNodes.remove(i);
            break;
        }
    }
}

void AudioContext::derefUnfinishedSourceNodes()
{
    ASSERT(isMainThread());
    for (unsigned i = 0; i < m_referencedNodes.size(); ++i)
        m_referencedNodes.at(i)->handler().breakConnection();

    m_referencedNodes.clear();
}

void DeferredTaskHandler::lock()
{
    // Don't allow regular lock in real-time audio thread.
    ASSERT(isMainThread());
    m_contextGraphMutex.lock();
}

bool DeferredTaskHandler::tryLock()
{
    // Try to catch cases of using try lock on main thread
    // - it should use regular lock.
    ASSERT(isAudioThread());
    if (!isAudioThread()) {
        // In release build treat tryLock() as lock() (since above
        // ASSERT(isAudioThread) never fires) - this is the best we can do.
        lock();
        return true;
    }
    return m_contextGraphMutex.tryLock();
}

void DeferredTaskHandler::unlock()
{
    m_contextGraphMutex.unlock();
}

bool DeferredTaskHandler::isAudioThread() const
{
    return currentThread() == m_audioThread;
}

#if ENABLE(ASSERT)
bool DeferredTaskHandler::isGraphOwner()
{
    return m_contextGraphMutex.locked();
}
#endif

void DeferredTaskHandler::addDeferredBreakConnection(AudioHandler& node)
{
    ASSERT(isAudioThread());
    m_deferredBreakConnectionList.append(&node);
}

void AudioContext::handleStoppableSourceNodes()
{
    ASSERT(isGraphOwner());

    // Find AudioBufferSourceNodes to see if we can stop playing them.
    for (unsigned i = 0; i < m_referencedNodes.size(); ++i) {
        AudioNode* node = m_referencedNodes.at(i).get();

        if (node->handler().nodeType() == AudioHandler::NodeTypeAudioBufferSource) {
            AudioBufferSourceNode* sourceNode = static_cast<AudioBufferSourceNode*>(node);
            sourceNode->audioBufferSourceHandler().handleStoppableSourceNode();
        }
    }
}
void AudioContext::handlePreRenderTasks()
{
    ASSERT(isAudioThread());

    // At the beginning of every render quantum, try to update the internal rendering graph state (from main thread changes).
    // It's OK if the tryLock() fails, we'll just take slightly longer to pick up the changes.
    if (tryLock()) {
        handler().handleDeferredTasks();

        resolvePromisesForResume();

        // Check to see if source nodes can be stopped because the end time has passed.
        handleStoppableSourceNodes();

        // Update the cached sample frame value.
        m_cachedSampleFrame = currentSampleFrame();

        unlock();
    }
}

void AudioContext::handlePostRenderTasks()
{
    ASSERT(isAudioThread());

    // Must use a tryLock() here too.  Don't worry, the lock will very rarely be contended and this method is called frequently.
    // The worst that can happen is that there will be some nodes which will take slightly longer than usual to be deleted or removed
    // from the render graph (in which case they'll render silence).
    if (tryLock()) {
        // Take care of AudioNode tasks where the tryLock() failed previously.
        handler().breakConnections();

        // Dynamically clean up nodes which are no longer needed.
        derefFinishedSourceNodes();

        handler().handleDeferredTasks();

        resolvePromisesForSuspend();

        unlock();
    }
}

void DeferredTaskHandler::breakConnections()
{
    ASSERT(isAudioThread());
    ASSERT(isGraphOwner());

    for (unsigned i = 0; i < m_deferredBreakConnectionList.size(); ++i)
        m_deferredBreakConnectionList[i]->breakConnectionWithLock();
    m_deferredBreakConnectionList.clear();
}

void AudioContext::registerLiveNode(AudioNode& node)
{
    ASSERT(isMainThread());
    m_liveNodes.add(&node);
}

void AudioContext::unregisterLiveNode(AudioNode& node)
{
    ASSERT(isMainThread());
    m_liveNodes.remove(&node);
}

void DeferredTaskHandler::disposeOutputs(AudioHandler& node)
{
    ASSERT(isGraphOwner());
    ASSERT(isMainThread());
    for (unsigned i = 0; i < node.numberOfOutputs(); ++i)
        node.output(i)->dispose();
}

void DeferredTaskHandler::markSummingJunctionDirty(AudioSummingJunction* summingJunction)
{
    ASSERT(isGraphOwner());
    m_dirtySummingJunctions.add(summingJunction);
}

void DeferredTaskHandler::removeMarkedSummingJunction(AudioSummingJunction* summingJunction)
{
    ASSERT(isMainThread());
    AutoLocker locker(*this);
    m_dirtySummingJunctions.remove(summingJunction);
}

void DeferredTaskHandler::markAudioNodeOutputDirty(AudioNodeOutput* output)
{
    ASSERT(isGraphOwner());
    ASSERT(isMainThread());
    m_dirtyAudioNodeOutputs.add(output);
}

void DeferredTaskHandler::removeMarkedAudioNodeOutput(AudioNodeOutput* output)
{
    ASSERT(isGraphOwner());
    ASSERT(isMainThread());
    m_dirtyAudioNodeOutputs.remove(output);
}

void DeferredTaskHandler::handleDirtyAudioSummingJunctions()
{
    ASSERT(isGraphOwner());

    for (AudioSummingJunction* junction : m_dirtySummingJunctions)
        junction->updateRenderingState();
    m_dirtySummingJunctions.clear();
}

void DeferredTaskHandler::handleDirtyAudioNodeOutputs()
{
    ASSERT(isGraphOwner());

    for (AudioNodeOutput* output : m_dirtyAudioNodeOutputs)
        output->updateRenderingState();
    m_dirtyAudioNodeOutputs.clear();
}

void DeferredTaskHandler::addAutomaticPullNode(AudioHandler* node)
{
    ASSERT(isGraphOwner());

    if (!m_automaticPullNodes.contains(node)) {
        m_automaticPullNodes.add(node);
        m_automaticPullNodesNeedUpdating = true;
    }
}

void DeferredTaskHandler::removeAutomaticPullNode(AudioHandler* node)
{
    ASSERT(isGraphOwner());

    if (m_automaticPullNodes.contains(node)) {
        m_automaticPullNodes.remove(node);
        m_automaticPullNodesNeedUpdating = true;
    }
}

void DeferredTaskHandler::updateAutomaticPullNodes()
{
    ASSERT(isGraphOwner());

    if (m_automaticPullNodesNeedUpdating) {
        copyToVector(m_automaticPullNodes, m_renderingAutomaticPullNodes);
        m_automaticPullNodesNeedUpdating = false;
    }
}

void DeferredTaskHandler::processAutomaticPullNodes(size_t framesToProcess)
{
    ASSERT(isAudioThread());

    for (unsigned i = 0; i < m_renderingAutomaticPullNodes.size(); ++i)
        m_renderingAutomaticPullNodes[i]->processIfNecessary(framesToProcess);
}

void AudioContext::resolvePromisesForResumeOnMainThread()
{
    ASSERT(isMainThread());
    AutoLocker locker(this);

    for (auto& resolver : m_resumeResolvers) {
        if (m_contextState == Closed) {
            resolver->reject(
                DOMException::create(InvalidStateError, "Cannot resume a context that has been closed"));
        } else {
            resolver->resolve();
        }
    }

    m_resumeResolvers.clear();
    m_isResolvingResumePromises = false;
}

void AudioContext::resolvePromisesForResume()
{
    // This runs inside the AudioContext's lock when handling pre-render tasks.
    ASSERT(isAudioThread());
    ASSERT(isGraphOwner());

    // Resolve any pending promises created by resume(). Only do this if we haven't already started
    // resolving these promises. This gets called very often and it takes some time to resolve the
    // promises in the main thread.
    if (!m_isResolvingResumePromises && m_resumeResolvers.size() > 0) {
        m_isResolvingResumePromises = true;
        Platform::current()->mainThread()->postTask(FROM_HERE, bind(&AudioContext::resolvePromisesForResumeOnMainThread, this));
    }
}

void AudioContext::resolvePromisesForSuspendOnMainThread()
{
    ASSERT(isMainThread());
    AutoLocker locker(this);

    // We can stop rendering now.
    if (m_destinationNode)
        stopRendering();

    for (auto& resolver : m_suspendResolvers) {
        if (m_contextState == Closed) {
            resolver->reject(
                DOMException::create(InvalidStateError, "Cannot suspend a context that has been closed"));
        } else {
            resolver->resolve();
        }
    }

    m_suspendResolvers.clear();
}

void AudioContext::resolvePromisesForSuspend()
{
    // This runs inside the AudioContext's lock when handling pre-render tasks.
    ASSERT(isAudioThread());
    ASSERT(isGraphOwner());

    // Resolve any pending promises created by suspend()
    if (m_suspendResolvers.size() > 0)
        Platform::current()->mainThread()->postTask(FROM_HERE, bind(&AudioContext::resolvePromisesForSuspendOnMainThread, this));
}

void AudioContext::rejectPendingResolvers()
{
    ASSERT(isMainThread());

    // Audio context is closing down so reject any suspend or resume promises that are still
    // pending.

    for (auto& resolver : m_suspendResolvers) {
        resolver->reject(DOMException::create(InvalidStateError, "Audio context is going away"));
    }
    m_suspendResolvers.clear();

    for (auto& resolver : m_resumeResolvers) {
        resolver->reject(DOMException::create(InvalidStateError, "Audio context is going away"));
    }
    m_resumeResolvers.clear();
    m_isResolvingResumePromises = false;
}

const AtomicString& AudioContext::interfaceName() const
{
    return EventTargetNames::AudioContext;
}

ExecutionContext* AudioContext::executionContext() const
{
    return m_isStopScheduled ? 0 : ActiveDOMObject::executionContext();
}

void AudioContext::startRendering()
{
    // This is called for both online and offline contexts.
    ASSERT(isMainThread());
    ASSERT(m_destinationNode);

    if (m_contextState == Suspended) {
        destination()->audioDestinationHandler().startRendering();
        setContextState(Running);
    }
}

void AudioContext::stopRendering()
{
    ASSERT(isMainThread());
    ASSERT(m_destinationNode);
    ASSERT(!isOfflineContext());

    if (m_contextState == Running) {
        destination()->audioDestinationHandler().stopRendering();
        setContextState(Suspended);
    }
}

void AudioContext::fireCompletionEvent()
{
    ASSERT(isMainThread());
    if (!isMainThread())
        return;

    AudioBuffer* renderedBuffer = m_renderTarget.get();

    // For an offline context, we set the state to closed here so that the oncomplete handler sees
    // that the context has been closed.
    setContextState(Closed);

    ASSERT(renderedBuffer);
    if (!renderedBuffer)
        return;

    // Avoid firing the event if the document has already gone away.
    if (executionContext()) {
        // Call the offline rendering completion event listener and resolve the promise too.
        dispatchEvent(OfflineAudioCompletionEvent::create(renderedBuffer));
        m_offlineResolver->resolve(renderedBuffer);
    }
}

DEFINE_TRACE(AudioContext)
{
    visitor->trace(m_closeResolver);
    visitor->trace(m_offlineResolver);
    visitor->trace(m_renderTarget);
    visitor->trace(m_destinationNode);
    visitor->trace(m_listener);
    // trace() can be called in AudioContext constructor, and
    // m_contextGraphMutex might be unavailable.
    if (m_didInitializeContextGraphMutex) {
        AutoLocker lock(this);
        visitor->trace(m_referencedNodes);
    } else {
        visitor->trace(m_referencedNodes);
    }
    visitor->trace(m_resumeResolvers);
    visitor->trace(m_suspendResolvers);
    RefCountedGarbageCollectedEventTargetWithInlineData<AudioContext>::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

void DeferredTaskHandler::addChangedChannelCountMode(AudioHandler* node)
{
    ASSERT(isGraphOwner());
    ASSERT(isMainThread());
    m_deferredCountModeChange.add(node);
}

void DeferredTaskHandler::removeChangedChannelCountMode(AudioHandler* node)
{
    ASSERT(isGraphOwner());

    m_deferredCountModeChange.remove(node);
}

void DeferredTaskHandler::updateChangedChannelCountMode()
{
    ASSERT(isGraphOwner());

    for (AudioHandler* node : m_deferredCountModeChange)
        node->updateChannelCountMode();
    m_deferredCountModeChange.clear();
}

SecurityOrigin* AudioContext::securityOrigin() const
{
    if (executionContext())
        return executionContext()->securityOrigin();

    return nullptr;
}

ScriptPromise AudioContext::closeContext(ScriptState* scriptState)
{
    if (isOfflineContext()) {
        return ScriptPromise::rejectWithDOMException(
            scriptState,
            DOMException::create(InvalidStateError, "Cannot call close() on an OfflineAudioContext."));
    }

    if (isContextClosed()) {
        // We've already closed the context previously, but it hasn't yet been resolved, so just
        // create a new promise and reject it.
        return ScriptPromise::rejectWithDOMException(
            scriptState,
            DOMException::create(InvalidStateError,
                "Cannot close a context that is being closed or has already been closed."));
    }

    m_closeResolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = m_closeResolver->promise();

    // Before closing the context go and disconnect all nodes, allowing them to be collected. This
    // will also break any connections to the destination node. Any unfinished sourced nodes will
    // get stopped when the context is unitialized.
    for (auto& node : m_liveNodes) {
        if (node) {
            for (unsigned k = 0; k < node->numberOfOutputs(); ++k)
                node->handler().disconnectWithoutException(k);
        }
    }

    // Stop the audio context. This will stop the destination node from pulling audio anymore. And
    // since we have disconnected the destination from the audio graph, and thus has no references,
    // the destination node can GCed if JS has no references. stop() will also resolve the Promise
    // created here.
    stop();

    return promise;
}

DeferredTaskHandler::DeferredTaskHandler()
    : m_automaticPullNodesNeedUpdating(false)
    , m_audioThread(0)
{
}

PassRefPtr<DeferredTaskHandler> DeferredTaskHandler::create()
{
    return adoptRef(new DeferredTaskHandler());
}

DeferredTaskHandler::~DeferredTaskHandler()
{
    ASSERT(!m_automaticPullNodes.size());
    if (m_automaticPullNodesNeedUpdating)
        m_renderingAutomaticPullNodes.resize(m_automaticPullNodes.size());
    ASSERT(!m_renderingAutomaticPullNodes.size());
}

void DeferredTaskHandler::handleDeferredTasks()
{
    updateChangedChannelCountMode();
    handleDirtyAudioSummingJunctions();
    handleDirtyAudioNodeOutputs();
    updateAutomaticPullNodes();
}

DeferredTaskHandler::AutoLocker::AutoLocker(AudioContext* context)
    : m_handler(context->handler())
{
    m_handler.lock();
}


} // namespace blink

#endif // ENABLE(WEB_AUDIO)
