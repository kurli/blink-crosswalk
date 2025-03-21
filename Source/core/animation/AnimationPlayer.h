/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef AnimationPlayer_h
#define AnimationPlayer_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseProperty.h"
#include "core/CSSPropertyNames.h"
#include "core/animation/AnimationNode.h"
#include "core/dom/ActiveDOMObject.h"
#include "core/dom/DOMException.h"
#include "core/events/EventTarget.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebCompositorAnimationDelegate.h"
#include "public/platform/WebCompositorAnimationPlayerClient.h"
#include "wtf/RefPtr.h"

namespace blink {

class AnimationTimeline;
class Element;
class ExceptionState;
class WebCompositorAnimationPlayer;

class AnimationPlayer final
    : public EventTargetWithInlineData
    , public RefCountedWillBeNoBase<AnimationPlayer>
    , public ActiveDOMObject
    , public WebCompositorAnimationDelegate
    , public WebCompositorAnimationPlayerClient {
    DEFINE_WRAPPERTYPEINFO();
    REFCOUNTED_EVENT_TARGET(AnimationPlayer);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(AnimationPlayer);
public:
    enum AnimationPlayState {
        Idle,
        Pending,
        Running,
        Paused,
        Finished
    };

    ~AnimationPlayer();
    static PassRefPtrWillBeRawPtr<AnimationPlayer> create(AnimationNode*, AnimationTimeline*);

    // Returns whether the player is finished.
    bool update(TimingUpdateReason);

    // timeToEffectChange returns:
    //  infinity  - if this player is no longer in effect
    //  0         - if this player requires an update on the next frame
    //  n         - if this player requires an update after 'n' units of time
    double timeToEffectChange();

    void cancel();

    double currentTime(bool& isNull);
    double currentTime();
    void setCurrentTime(double newCurrentTime);

    double currentTimeInternal() const;
    double unlimitedCurrentTimeInternal() const;

    void setCurrentTimeInternal(double newCurrentTime, TimingUpdateReason = TimingUpdateOnDemand);
    bool paused() const { return m_paused && !m_isPausedForTesting; }
    static const char* playStateString(AnimationPlayState);
    String playState() const { return playStateString(playStateInternal()); }
    AnimationPlayState playStateInternal() const;

    void pause();
    void play();
    void reverse();
    void finish(ExceptionState&);

    ScriptPromise finished(ScriptState*);
    ScriptPromise ready(ScriptState*);

    bool playing() const { return !(playStateInternal() == Idle || limited() || m_paused || m_isPausedForTesting); }
    bool limited() const { return limited(currentTimeInternal()); }
    bool finishedInternal() const { return m_finished; }

    DEFINE_ATTRIBUTE_EVENT_LISTENER(finish);

    virtual const AtomicString& interfaceName() const override;
    virtual ExecutionContext* executionContext() const override;
    virtual bool hasPendingActivity() const override;
    virtual void stop() override;
    virtual bool dispatchEvent(PassRefPtrWillBeRawPtr<Event>) override;

    double playbackRate() const;
    void setPlaybackRate(double);
    const AnimationTimeline* timeline() const { return m_timeline; }
    AnimationTimeline* timeline() { return m_timeline; }

#if !ENABLE(OILPAN)
    void timelineDestroyed() { m_timeline = nullptr; }
#endif

    double calculateStartTime(double currentTime) const;
    bool hasStartTime() const { return !isNull(m_startTime); }
    double startTime(bool& isNull) const;
    double startTime() const;
    double startTimeInternal() const { return m_startTime; }
    void setStartTime(double);
    void setStartTimeInternal(double);

    const AnimationNode* source() const { return m_content.get(); }
    AnimationNode* source() { return m_content.get(); }
    void setSource(AnimationNode*);

    // Pausing via this method is not reflected in the value returned by
    // paused() and must never overlap with pausing via pause().
    void pauseForTesting(double pauseTime);
    // This should only be used for CSS
    void unpause();

    void setOutdated();
    bool outdated() { return m_outdated; }

    bool canStartAnimationOnCompositor() const;
    bool isCandidateForAnimationOnCompositor() const;
    bool maybeStartAnimationOnCompositor();
    void cancelAnimationOnCompositor();
    void restartAnimationOnCompositor();
    void cancelIncompatibleAnimationsOnCompositor();
    bool hasActiveAnimationsOnCompositor();
    void setCompositorPending(bool sourceChanged = false);
    void notifyCompositorStartTime(double timelineTime);
    void notifyStartTime(double timelineTime);
    // WebCompositorAnimationPlayerClient implementation.
    WebCompositorAnimationPlayer* compositorPlayer() const override { return m_compositorPlayer.get(); }

    bool affects(const Element&, CSSPropertyID) const;

    void preCommit(int compositorGroup, bool startOnCompositor);
    void postCommit(double timelineTime);

    unsigned sequenceNumber() const { return m_sequenceNumber; }
    int compositorGroup() const { return m_compositorGroup; }

    static bool hasLowerPriority(const AnimationPlayer* player1, const AnimationPlayer* player2)
    {
        return player1->sequenceNumber() < player2->sequenceNumber();
    }

    virtual bool addEventListener(const AtomicString& eventType, PassRefPtr<EventListener>, bool useCapture = false) override;

    DECLARE_VIRTUAL_TRACE();

private:
    AnimationPlayer(ExecutionContext*, AnimationTimeline&, AnimationNode*);

    double sourceEnd() const;
    bool limited(double currentTime) const;

    AnimationPlayState calculatePlayState();
    double calculateCurrentTime() const;

    void unpauseInternal();
    void setPlaybackRateInternal(double);
    void updateCurrentTimingState(TimingUpdateReason);

    void beginUpdatingState();
    void endUpdatingState();

    void createCompositorPlayer();
    void destroyCompositorPlayer();
    void attachCompositorTimeline();
    void detachCompositorTimeline();
    void attachCompositedLayers();
    void detachCompositedLayers();
    // WebCompositorAnimationDelegate implementation.
    void notifyAnimationStarted(double monotonicTime, int group) override;
    void notifyAnimationFinished(double monotonicTime, int group) override { }

    AnimationPlayState m_playState;
    double m_playbackRate;
    double m_startTime;
    double m_holdTime;

    unsigned m_sequenceNumber;

    typedef ScriptPromiseProperty<RawPtrWillBeMember<AnimationPlayer>, RawPtrWillBeMember<AnimationPlayer>, RefPtrWillBeMember<DOMException>> AnimationPlayerPromise;
    PersistentWillBeMember<AnimationPlayerPromise> m_finishedPromise;
    PersistentWillBeMember<AnimationPlayerPromise> m_readyPromise;

    RefPtrWillBeMember<AnimationNode> m_content;
    RawPtrWillBeMember<AnimationTimeline> m_timeline;
    // Reflects all pausing, including via pauseForTesting().
    bool m_paused;
    bool m_held;
    bool m_isPausedForTesting;

    // This indicates timing information relevant to the player's effect
    // has changed by means other than the ordinary progression of time
    bool m_outdated;

    bool m_finished;
    // Holds a 'finished' event queued for asynchronous dispatch via the
    // ScriptedAnimationController. This object remains active until the
    // event is actually dispatched.
    RefPtrWillBeMember<Event> m_pendingFinishedEvent;

    enum CompositorAction {
        None,
        Pause,
        Start,
        PauseThenStart
    };

    class CompositorState {
    public:
        CompositorState(AnimationPlayer& player)
            : startTime(player.m_startTime)
            , holdTime(player.m_holdTime)
            , playbackRate(player.m_playbackRate)
            , sourceChanged(false)
            , pendingAction(Start)
        { }
        double startTime;
        double holdTime;
        double playbackRate;
        bool sourceChanged;
        CompositorAction pendingAction;
    };

    enum CompositorPendingChange {
        SetCompositorPending,
        SetCompositorPendingWithSourceChanged,
        DoNotSetCompositorPending,
    };

    class PlayStateUpdateScope {
        STACK_ALLOCATED();
    public:
        PlayStateUpdateScope(AnimationPlayer&, TimingUpdateReason, CompositorPendingChange = SetCompositorPending);
        ~PlayStateUpdateScope();
    private:
        RawPtrWillBeMember<AnimationPlayer> m_player;
        AnimationPlayState m_initialPlayState;
        CompositorPendingChange m_compositorPendingChange;
    };

    // This mirrors the known compositor state. It is created when a compositor
    // animation is started. Updated once the start time is known and each time
    // modifications are pushed to the compositor.
    OwnPtr<CompositorState> m_compositorState;
    bool m_compositorPending;
    int m_compositorGroup;

    OwnPtr<WebCompositorAnimationPlayer> m_compositorPlayer;

    bool m_currentTimePending;
    bool m_stateIsBeingUpdated;
};

} // namespace blink

#endif // AnimationPlayer_h
