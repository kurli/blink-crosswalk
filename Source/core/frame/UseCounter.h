/*
 * Copyright (C) 2012 Google, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UseCounter_h
#define UseCounter_h

#include "core/CSSPropertyNames.h"
#include "core/CoreExport.h"
#include "wtf/BitVector.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"
#include <v8.h>

namespace blink {

class CSSStyleSheet;
class Document;
class ExecutionContext;
class Frame;
class LocalFrame;
class StyleSheetContents;

// UseCounter is used for counting the number of times features of
// Blink are used on real web pages and help us know commonly
// features are used and thus when it's safe to remove or change them.
//
// The Chromium Content layer controls what is done with this data.
// For instance, in Google Chrome, these counts are submitted
// anonymously through the Histogram recording system in Chrome
// for users who opt-in to "Usage Statistics" submission
// during their install of Google Chrome:
// http://www.google.com/chrome/intl/en/privacy.html

class CORE_EXPORT UseCounter {
    WTF_MAKE_NONCOPYABLE(UseCounter);
public:
    UseCounter();
    ~UseCounter();

    enum Feature {
        // Do not change assigned numbers of existing items: add new features
        // to the end of the list.
        PageDestruction = 0,
        PrefixedIndexedDB = 3,
        WorkerStart = 4,
        SharedWorkerStart = 5,
        UnprefixedIndexedDB = 9,
        OpenWebDatabase = 10,
        UnprefixedRequestAnimationFrame = 13,
        PrefixedRequestAnimationFrame = 14,
        ContentSecurityPolicy = 15,
        ContentSecurityPolicyReportOnly = 16,
        PrefixedTransitionEndEvent = 18,
        UnprefixedTransitionEndEvent = 19,
        PrefixedAndUnprefixedTransitionEndEvent = 20,
        AutoFocusAttribute = 21,
        DataListElement = 23,
        FormAttribute = 24,
        IncrementalAttribute = 25,
        InputTypeColor = 26,
        InputTypeDate = 27,
        InputTypeDateTimeFallback = 29,
        InputTypeDateTimeLocal = 30,
        InputTypeEmail = 31,
        InputTypeMonth = 32,
        InputTypeNumber = 33,
        InputTypeRange = 34,
        InputTypeSearch = 35,
        InputTypeTel = 36,
        InputTypeTime = 37,
        InputTypeURL = 38,
        InputTypeWeek = 39,
        InputTypeWeekFallback = 40,
        ListAttribute = 41,
        MaxAttribute = 42,
        MinAttribute = 43,
        PatternAttribute = 44,
        PlaceholderAttribute = 45,
        PrefixedDirectoryAttribute = 47,
        RequiredAttribute = 49,
        ResultsAttribute = 50,
        StepAttribute = 51,
        PageVisits = 52,
        HTMLMarqueeElement = 53,
        Reflection = 55,
        PrefixedStorageInfo = 57,
        XFrameOptions = 58,
        XFrameOptionsSameOrigin = 59,
        XFrameOptionsSameOriginWithBadAncestorChain = 60,
        DeprecatedFlexboxWebContent = 61,
        DeprecatedFlexboxChrome = 62,
        DeprecatedFlexboxChromeExtension = 63,
        UnprefixedPerformanceTimeline = 65,
        UnprefixedUserTiming = 67,
        WindowEvent = 69,
        ContentSecurityPolicyWithBaseElement = 70,
        PrefixedMediaAddKey = 71,
        PrefixedMediaGenerateKeyRequest = 72,
        DocumentClear = 74,
        XMLDocument = 77,
        XSLProcessingInstruction = 78,
        XSLTProcessor = 79,
        SVGSwitchElement = 80,
        DocumentAll = 83,
        FormElement = 84,
        DemotedFormElement = 85,
        SVGAnimationElement = 90,
        KeyboardEventKeyLocation = 91,
        LineClamp = 96,
        SubFrameBeforeUnloadRegistered = 97,
        SubFrameBeforeUnloadFired = 98,
        ConsoleMarkTimeline = 102,
        ElementGetAttributeNode = 107,
        ElementSetAttributeNode = 108,
        ElementRemoveAttributeNode = 109,
        ElementGetAttributeNodeNS = 110,
        DocumentCreateAttribute = 111,
        DocumentCreateAttributeNS = 112,
        DocumentCreateCDATASection = 113, // Removed from DOM4.
        DocumentInputEncoding = 114,
        DocumentXMLEncoding = 115, // Removed from DOM4.
        DocumentXMLStandalone = 116, // Removed from DOM4.
        DocumentXMLVersion = 117, // Removed from DOM4.
        NodeIsSameNode = 118, // Removed from DOM4.
        NodeNamespaceURI = 120, // Removed from DOM4.
        NodeLocalName = 122, // Removed from DOM4.
        NavigatorProductSub = 123,
        NavigatorVendor = 124,
        NavigatorVendorSub = 125,
        FileError = 126,
        DocumentCharset = 127, // Documented as IE extensions = 0, from KHTML days.
        PrefixedAnimationEndEvent = 128,
        UnprefixedAnimationEndEvent = 129,
        PrefixedAndUnprefixedAnimationEndEvent = 130,
        PrefixedAnimationStartEvent = 131,
        UnprefixedAnimationStartEvent = 132,
        PrefixedAndUnprefixedAnimationStartEvent = 133,
        PrefixedAnimationIterationEvent = 134,
        UnprefixedAnimationIterationEvent = 135,
        PrefixedAndUnprefixedAnimationIterationEvent = 136,
        EventReturnValue = 137, // Legacy IE extension.
        SVGSVGElement = 138,
        InsertAdjacentText = 140,
        InsertAdjacentElement = 141,
        HasAttributes = 142,
        DOMSubtreeModifiedEvent = 143,
        DOMNodeInsertedEvent = 144,
        DOMNodeRemovedEvent = 145,
        DOMNodeRemovedFromDocumentEvent = 146,
        DOMNodeInsertedIntoDocumentEvent = 147,
        DOMCharacterDataModifiedEvent = 148,
        DocumentAllLegacyCall = 150,
        HTMLEmbedElementLegacyCall = 152,
        HTMLObjectElementLegacyCall = 153,
        GetMatchedCSSRules = 155,
        AttributeOwnerElement = 160,
        AttributeSpecified = 162,
        PrefixedAudioDecodedByteCount = 164,
        PrefixedVideoDecodedByteCount = 165,
        PrefixedVideoSupportsFullscreen = 166,
        PrefixedVideoDisplayingFullscreen = 167,
        PrefixedVideoEnterFullscreen = 168,
        PrefixedVideoExitFullscreen = 169,
        PrefixedVideoEnterFullScreen = 170,
        PrefixedVideoExitFullScreen = 171,
        PrefixedVideoDecodedFrameCount = 172,
        PrefixedVideoDroppedFrameCount = 173,
        PrefixedElementRequestFullscreen = 176,
        PrefixedElementRequestFullScreen = 177,
        BarPropLocationbar = 178,
        BarPropMenubar = 179,
        BarPropPersonalbar = 180,
        BarPropScrollbars = 181,
        BarPropStatusbar = 182,
        BarPropToolbar = 183,
        InputTypeEmailMultiple = 184,
        InputTypeEmailMaxLength = 185,
        InputTypeEmailMultipleMaxLength = 186,
        InputTypeText = 190,
        InputTypeTextMaxLength = 191,
        InputTypePassword = 192,
        InputTypePasswordMaxLength = 193,
        PrefixedPageVisibility = 196,
        CSSStyleSheetInsertRuleOptionalArg = 198, // Inconsistent with the specification and other browsers.
        DocumentBeforeUnloadRegistered = 200,
        DocumentBeforeUnloadFired = 201,
        DocumentUnloadRegistered = 202,
        DocumentUnloadFired = 203,
        SVGLocatableNearestViewportElement = 204,
        SVGLocatableFarthestViewportElement = 205,
        OverflowChangedEvent = 208,
        SVGPointMatrixTransform = 209,
        DOMFocusInOutEvent = 211,
        FileGetLastModifiedDate = 212,
        HTMLElementInnerText = 213,
        HTMLElementOuterText = 214,
        ReplaceDocumentViaJavaScriptURL = 215,
        ElementSetAttributeNodeNS = 216,
        ElementPrefixedMatchesSelector = 217,
        CSSStyleSheetRules = 219,
        CSSStyleSheetAddRule = 220,
        CSSStyleSheetRemoveRule = 221,
        // The above items are available in M33 branch.

        InitMessageEvent = 222,
        PrefixedMediaCancelKeyRequest = 229,
        DOMImplementationHasFeature = 230,
        DOMImplementationHasFeatureReturnFalse = 231,
        CanPlayTypeKeySystem = 232,
        PrefixedDevicePixelRatioMediaFeature = 233,
        PrefixedMaxDevicePixelRatioMediaFeature = 234,
        PrefixedMinDevicePixelRatioMediaFeature = 235,
        PrefixedTransform3dMediaFeature = 237,
        PrefixedStorageQuota = 240,
        ResetReferrerPolicy = 243,
        CaseInsensitiveAttrSelectorMatch = 244, // Case-insensitivity dropped from specification.
        FormNameAccessForImageElement = 246,
        FormNameAccessForPastNamesMap = 247,
        FormAssociationByParser = 248,
        SVGSVGElementInDocument = 250,
        SVGDocumentRootElement = 251,
        WorkerSubjectToCSP = 257,
        WorkerAllowedByChildBlockedByScript = 258,
        DeprecatedWebKitGradient = 260,
        DeprecatedWebKitLinearGradient = 261,
        DeprecatedWebKitRepeatingLinearGradient = 262,
        DeprecatedWebKitRadialGradient = 263,
        DeprecatedWebKitRepeatingRadialGradient = 264,
        PrefixedImageSmoothingEnabled = 267,
        UnprefixedImageSmoothingEnabled = 268,
        // The above items are available in M34 branch.

        TextAutosizing = 274,
        HTMLAnchorElementPingAttribute = 276,
        InsertAdjacentHTML = 278,
        SVGClassName = 279,
        HTMLAppletElement = 280,
        HTMLMediaElementSeekToFragmentStart = 281,
        HTMLMediaElementPauseAtFragmentEnd = 282,
        PrefixedWindowURL = 283,
        WindowOrientation = 285,
        DOMStringListContains = 286,
        DocumentCaptureEvents = 287,
        DocumentReleaseEvents = 288,
        WindowCaptureEvents = 289,
        WindowReleaseEvents = 290,
        ElementAnimateKeyframeListEffectObjectTiming = 292,
        ElementAnimateKeyframeListEffectDoubleTiming = 293,
        ElementAnimateKeyframeListEffectNoTiming = 294,
        DocumentXPathCreateExpression = 295,
        DocumentXPathCreateNSResolver = 296,
        DocumentXPathEvaluate = 297,
        AttrGetValue = 298,
        AttrSetValue = 299,
        AnimationConstructorKeyframeListEffectObjectTiming = 300,
        AnimationConstructorKeyframeListEffectDoubleTiming = 301,
        AnimationConstructorKeyframeListEffectNoTiming = 302,
        AttrSetValueWithElement = 303,
        PrefixedCancelAnimationFrame = 304,
        PrefixedCancelRequestAnimationFrame = 305,
        NamedNodeMapGetNamedItem = 306,
        NamedNodeMapSetNamedItem = 307,
        NamedNodeMapRemoveNamedItem = 308,
        NamedNodeMapItem = 309,
        NamedNodeMapGetNamedItemNS = 310,
        NamedNodeMapSetNamedItemNS = 311,
        NamedNodeMapRemoveNamedItemNS = 312,
        XHRProgressEventPosition = 316,
        XHRProgressEventTotalSize = 317,
        PrefixedDocumentIsFullscreen = 318,
        PrefixedDocumentCurrentFullScreenElement = 320,
        PrefixedDocumentCancelFullScreen = 321,
        PrefixedDocumentFullscreenEnabled = 322,
        PrefixedDocumentFullscreenElement = 323,
        PrefixedDocumentExitFullscreen = 324,
        // The above items are available in M35 branch.

        SVGForeignObjectElement = 325,
        SelectionSetPosition = 327,
        AnimationPlayerFinishEvent = 328,
        SVGSVGElementInXMLDocument = 329,
        PrefixedPerformanceClearResourceTimings = 341,
        PrefixedPerformanceSetResourceTimingBufferSize = 342,
        EventSrcElement = 343,
        EventCancelBubble = 344,
        EventPath = 345,
        NodeIteratorDetach = 347,
        AttrNodeValue = 348,
        AttrTextContent = 349,
        EventGetReturnValueTrue = 350,
        EventGetReturnValueFalse = 351,
        EventSetReturnValueTrue = 352,
        EventSetReturnValueFalse = 353,
        WindowOffscreenBuffering = 356,
        WindowDefaultStatus = 357,
        WindowDefaultstatus = 358,
        PrefixedTransitionEventConstructor = 361,
        PrefixedMutationObserverConstructor = 362,
        PrefixedIDBCursorConstructor = 363,
        PrefixedIDBDatabaseConstructor = 364,
        PrefixedIDBFactoryConstructor = 365,
        PrefixedIDBIndexConstructor = 366,
        PrefixedIDBKeyRangeConstructor = 367,
        PrefixedIDBObjectStoreConstructor = 368,
        PrefixedIDBRequestConstructor = 369,
        PrefixedIDBTransactionConstructor = 370,
        NotificationPermission = 371,
        RangeDetach = 372,
        HTMLTableElementVspace = 374,
        HTMLTableElementHspace = 375,
        PrefixedTouchRadiusX = 378,
        PrefixedTouchRadiusY = 379,
        PrefixedTouchRotationAngle = 380,
        PrefixedTouchForce = 381,
        PrefixedMouseEventMovementX = 382,
        PrefixedMouseEventMovementY = 383,
        PrefixedFileRelativePath = 386,
        DocumentCaretRangeFromPoint = 387,
        DocumentGetCSSCanvasContext = 388,
        ElementScrollIntoViewIfNeeded = 389,
        RangeCompareNode = 392,
        RangeExpand = 393,
        HTMLImageElementX = 396,
        HTMLImageElementY = 397,
        SelectionBaseNode = 400,
        SelectionBaseOffset = 401,
        SelectionExtentNode = 402,
        SelectionExtentOffset = 403,
        SelectionType = 404,
        SelectionModify = 405,
        SelectionSetBaseAndExtent = 406,
        SelectionEmpty = 407,
        VTTCue = 409,
        VTTCueRender = 410,
        VTTCueRenderVertical = 411,
        VTTCueRenderSnapToLinesFalse = 412,
        VTTCueRenderLineNotAuto = 413,
        VTTCueRenderPositionNot50 = 414,
        VTTCueRenderSizeNot100 = 415,
        VTTCueRenderAlignNotMiddle = 416,
        // The above items are available in M36 branch.

        ElementRequestPointerLock = 417,
        VTTCueRenderRtl = 418,
        PostMessageFromSecureToInsecure = 419,
        PostMessageFromInsecureToSecure = 420,
        DocumentExitPointerLock = 421,
        DocumentPointerLockElement = 422,
        PrefixedCursorZoomIn = 424,
        PrefixedCursorZoomOut = 425,
        DocumentSetCharset = 427,
        DocumentDefaultCharset = 428,
        TextEncoderConstructor = 429,
        TextEncoderEncode = 430,
        TextDecoderConstructor = 431,
        TextDecoderDecode = 432,
        FocusInOutEvent = 433,
        MouseEventMovementX = 434,
        MouseEventMovementY = 435,
        DocumentFonts = 440,
        MixedContentFormsSubmitted = 441,
        FormsSubmitted = 442,
        TextInputEventOnInput = 443,
        TextInputEventOnTextArea = 444,
        TextInputEventOnContentEditable = 445,
        TextInputEventOnNotNode = 446,
        WebkitBeforeTextInsertedOnInput = 447,
        WebkitBeforeTextInsertedOnTextArea = 448,
        WebkitBeforeTextInsertedOnContentEditable = 449,
        WebkitBeforeTextInsertedOnNotNode = 450,
        WebkitEditableContentChangedOnInput = 451,
        WebkitEditableContentChangedOnTextArea = 452,
        WebkitEditableContentChangedOnContentEditable = 453,
        WebkitEditableContentChangedOnNotNode = 454,
        HTMLImports = 455,
        ElementCreateShadowRoot = 456,
        DocumentRegisterElement = 457,
        EditingAppleInterchangeNewline = 458,
        EditingAppleConvertedSpace = 459,
        EditingApplePasteAsQuotation = 460,
        EditingAppleStyleSpanClass = 461,
        EditingAppleTabSpanClass = 462,
        HTMLImportsAsyncAttribute = 463,
        FontFaceSetReady = 464,
        XMLHttpRequestSynchronous = 465,
        CSSSelectorPseudoUnresolved = 466,
        CSSSelectorPseudoShadow = 467,
        CSSSelectorPseudoContent = 468,
        CSSSelectorPseudoHost = 469,
        CSSSelectorPseudoHostContext = 470,
        CSSDeepCombinator = 471,
        SyncXHRWithCredentials = 472,
        // The above items are available in M37 branch.

        UseAsm = 473,
        DOMWindowOpen = 475,
        DOMWindowOpenFeatures = 476,
        MediaStreamTrackGetSources = 478,
        AspectRatioFlexItem = 479,
        DetailsElement = 480,
        DialogElement = 481,
        MapElement = 482,
        MeterElement = 483,
        ProgressElement = 484,
        VideoFullscreenAllowedExemption = 485,
        PrefixedHTMLElementDropzone = 490,
        WheelEventWheelDeltaX = 491,
        WheelEventWheelDeltaY = 492,
        WheelEventWheelDelta = 493,
        SendBeacon = 494,
        SendBeaconQuotaExceeded = 495,
        SVGSMILElementInDocument = 501,
        MouseEventOffsetX = 502,
        MouseEventOffsetY = 503,
        MouseEventX = 504,
        MouseEventY = 505,
        MouseEventFromElement = 506,
        MouseEventToElement = 507,
        RequestFileSystem = 508,
        RequestFileSystemWorker = 509,
        RequestFileSystemSyncWorker = 510,
        UIEventLayerX = 511,
        UIEventLayerY = 512,
        UIEventPageX = 513,
        UIEventPageY = 514,
        DevToolsConsoleTimeline = 517,
        DevToolsConsoleProfile = 518,
        SVGStyleElementTitle = 519,
        PictureSourceSrc = 520,
        // The above items are available in M38 branch.

        Picture = 521,
        Sizes = 522,
        SrcsetXDescriptor = 523,
        SrcsetWDescriptor = 524,
        SelectionContainsNode = 525,
        MediaStreamEnded = 526,
        MixedContentPrivateIPInPublicWebsitePassive = 527,
        MixedContentPrivateIPInPublicWebsiteActive = 528,
        XMLExternalResourceLoad = 529,
        MixedContentPrivateHostnameInPublicHostname = 530,
        LegacyProtocolEmbeddedAsSubresource = 531,
        RequestedSubresourceWithEmbeddedCredentials = 532,
        NotificationCreated = 533,
        NotificationClosed = 534,
        NotificationPermissionRequested = 535,
        MediaStreamLabel = 536,
        MediaStreamStop = 537,
        ConsoleTimeline = 538,
        ConsoleTimelineEnd = 539,
        SRIElementWithMatchingIntegrityAttribute = 540,
        SRIElementWithNonMatchingIntegrityAttribute = 541,
        SRIElementWithUnparsableIntegrityAttribute = 542,
        V8AnimationPlayer_StartTime_AttributeGetter = 545,
        V8AnimationPlayer_StartTime_AttributeSetter = 546,
        V8AnimationPlayer_CurrentTime_AttributeGetter = 547,
        V8AnimationPlayer_CurrentTime_AttributeSetter = 548,
        V8AnimationPlayer_PlaybackRate_AttributeGetter = 549,
        V8AnimationPlayer_PlaybackRate_AttributeSetter = 550,
        V8AnimationPlayer_PlayState_AttributeGetter = 551,
        V8AnimationPlayer_Finish_Method = 552,
        V8AnimationPlayer_Play_Method = 553,
        V8AnimationPlayer_Pause_Method = 554,
        V8AnimationPlayer_Reverse_Method = 555,
        // The above items are available in M39 branch.

        BreakIterator = 556,
        ScreenOrientationAngle = 557,
        ScreenOrientationType = 558,
        ScreenOrientationLock = 559,
        ScreenOrientationUnlock = 560,
        GeolocationSecureOrigin = 561,
        GeolocationInsecureOrigin = 562,
        NotificationSecureOrigin = 563,
        NotificationInsecureOrigin = 564,
        NotificationShowEvent = 565,
        CSSXGetComputedStyleQueries = 566,
        SVG1DOM = 567,
        SVGPathSegDOM = 568,
        SVGTransformListConsolidate = 569,
        SVGAnimatedTransformListBaseVal = 570,
        QuotedAnimationName = 571,
        QuotedKeyframesRule = 572,
        SrcsetDroppedCandidate = 573,
        WindowPostMessage = 574,
        WindowPostMessageWithLegacyTargetOriginArgument = 575,
        RenderRuby = 576,
        CanvasRenderingContext2DCompositeOperationDarker = 577,
        ScriptElementWithInvalidTypeHasSrc = 578,
        TimelineStart = 579,
        ElementBaseURIFromXMLBase = 580,
        XMLHttpRequestSynchronousInNonWorkerOutsideBeforeUnload = 581,
        CSSSelectorPseudoScrollbar = 582,
        CSSSelectorPseudoScrollbarButton = 583,
        CSSSelectorPseudoScrollbarThumb = 584,
        CSSSelectorPseudoScrollbarTrack = 585,
        CSSSelectorPseudoScrollbarTrackPiece = 586,
        LangAttribute = 587,
        LangAttributeOnHTML = 588,
        LangAttributeOnBody = 589,
        LangAttributeDoesNotMatchToUILocale = 590,
        InputTypeSubmit = 591,
        InputTypeSubmitWithValue = 592,
        // The above items are available in M40 branch.

        SetReferrerPolicy = 593,
        DOMImplementationHasFeatureReturnFalseInternal = 594,
        MouseEventWhich = 595,
        UIEventCharCode = 596,
        UIEventKeyCode = 597,
        UIEventWhich = 598,
        TextWholeText = 599,
        AttrChildAccess = 600,
        AttrChildChange = 601,
        NotificationCloseEvent = 603,
        CSSKeyframesRuleAppendRule = 604,
        CSSKeyframesRuleInsertRule = 605,
        StyleMedia = 606,
        StyleMediaType = 607,
        StyleMediaMatchMedium = 608,
        MixedContentPresent = 609,
        MixedContentBlockable = 610,
        MixedContentAudio = 611,
        MixedContentDownload = 612,
        MixedContentFavicon = 613,
        MixedContentImage = 614,
        MixedContentInternal = 615,
        MixedContentPlugin = 616,
        MixedContentPrefetch = 617,
        MixedContentVideo = 618,
        AudioListenerDopplerFactor = 620,
        AudioListenerSpeedOfSound = 621,
        AudioListenerSetVelocity = 622,
        ShadowRootGetElementsByClassName = 623,
        ShadowRootGetElementsByTagName = 624,
        ShadowRootGetElementsByTagNameNS = 625,
        SVGSMILAnimationInImage = 626,
        CSSSelectorPseudoFullScreenDocument = 627,
        CSSSelectorPseudoFullScreenAncestor = 628,
        CSSSelectorPseudoFullScreen = 629,
        WebKitCSSMatrix = 630,
        AudioContextCreateAnalyser = 631,
        AudioContextCreateBiquadFilter = 632,
        AudioContextCreateBufferSource = 633,
        AudioContextCreateChannelMerger = 634,
        AudioContextCreateChannelSplitter = 635,
        AudioContextCreateConvolver = 636,
        AudioContextCreateDelay = 637,
        AudioContextCreateDynamicsCompressor = 638,
        AudioContextCreateGain = 639,
        AudioContextCreateMediaElementSource = 640,
        AudioContextCreateMediaStreamDestination = 641,
        AudioContextCreateMediaStreamSource = 642,
        AudioContextCreateOscillator = 643,
        AudioContextCreatePanner = 644,
        AudioContextCreatePeriodicWave = 645,
        AudioContextCreateScriptProcessor = 646,
        AudioContextCreateStereoPanner = 647,
        AudioContextCreateWaveShaper = 648,
        AudioContextDecodeAudioData = 649,
        AudioContextResume = 650,
        AudioContextSuspend = 651,
        AudioContext = 652,
        OfflineAudioContext = 653,
        PrefixedAudioContext = 654,
        PrefixedOfflineAudioContext = 655,
        AddEventListenerNoArguments = 656,
        RemoveEventListenerNoArguments = 658,
        SRIElementWithNonMatchingIntegrityType = 660,
        MixedContentInNonHTTPSFrameThatRestrictsMixedContent = 661,
        MixedContentInSecureFrameThatDoesNotRestrictMixedContent = 662,
        MixedContentWebSocket = 663,
        SyntheticKeyframesInCompositedCSSAnimation = 664,
        MixedContentFormPresent = 665,
        GetUserMediaInsecureOrigin = 666,
        GetUserMediaSecureOrigin = 667,
        // The above items are available in M41 branch.

        DeviceMotionInsecureOrigin = 668,
        DeviceMotionSecureOrigin = 669,
        DeviceOrientationInsecureOrigin = 670,
        DeviceOrientationSecureOrigin = 671,
        SandboxViaIFrame = 672,
        SandboxViaCSP = 673,
        BlockedSniffingImageToScript = 674,
        Fetch = 675,
        FetchBodyStream = 676,
        XMLHttpRequestAsynchronous = 677,
        AudioBufferSourceBufferOnce = 678,
        WhiteSpacePreFromXMLSpace = 679,
        WhiteSpaceNowrapFromXMLSpace = 680,
        SVGElementXmlbase = 681,
        SVGElementXmllang = 682,
        SVGElementXmlspace = 683,
        WindowMoveResizeMissingArguments = 684,
        SVGSVGElementForceRedraw = 685,
        SVGSVGElementSuspendRedraw = 686,
        SVGSVGElementUnsuspendRedraw = 687,
        SVGSVGElementUnsuspendRedrawAll = 688,
        AudioContextClose = 689,
        ServiceWorkerClientPostMessage = 690,
        CSSZoomNotEqualToOne = 691,
        SVGGraphicsElementGetTransformToElement = 692,
        ServiceWorkerClientsGetAll = 693,
        // The above items are available in M42 branch.

        ClientRectListItem = 694,
        WindowClientInformation = 695,
        WindowFind = 696,
        WindowScreenLeft = 697,
        WindowScreenTop = 698,
        V8AnimationPlayer_Cancel_Method = 699,
        V8AnimationPlayer_Onfinish_AttributeGetter = 700,
        V8AnimationPlayer_Onfinish_AttributeSetter = 701,
        ElementOffsetParent = 702,
        ElementOffsetTop = 703,
        ElementOffsetLeft = 704,
        ElementOffsetWidth = 705,
        ElementOffsetHeight = 706,
        V8Window_WebKitAnimationEvent_ConstructorGetter = 707,
        V8Window_WebKitAnimationEvent_AttributeSetter = 708,
        ResourceLoadedAfterRedirectWithCSP = 709,
        CryptoGetRandomValues = 710,
        SubtleCryptoEncrypt = 711,
        SubtleCryptoDecrypt = 712,
        SubtleCryptoSign = 713,
        SubtleCryptoVerify = 714,
        SubtleCryptoDigest = 715,
        SubtleCryptoGenerateKey = 716,
        SubtleCryptoImportKey = 717,
        SubtleCryptoExportKey = 718,
        SubtleCryptoDeriveBits = 719,
        SubtleCryptoDeriveKey = 720,
        SubtleCryptoWrapKey = 721,
        SubtleCryptoUnwrapKey = 722,
        CryptoAlgorithmAesCbc = 723,
        CryptoAlgorithmHmac = 724,
        CryptoAlgorithmRsaSsaPkcs1v1_5 = 725,
        CryptoAlgorithmSha1 = 726,
        CryptoAlgorithmSha256 = 727,
        CryptoAlgorithmSha384 = 728,
        CryptoAlgorithmSha512 = 729,
        CryptoAlgorithmAesGcm = 730,
        CryptoAlgorithmRsaOaep = 731,
        CryptoAlgorithmAesCtr = 732,
        CryptoAlgorithmAesKw = 733,
        CryptoAlgorithmRsaPss = 734,
        CryptoAlgorithmEcdsa = 735,
        CryptoAlgorithmEcdh = 736,
        CryptoAlgorithmHkdf = 737,
        CryptoAlgorithmPbkdf2 = 738,
        DocumentSetDomain = 739,
        UpgradeInsecureRequestsEnabled = 740,
        UpgradeInsecureRequestsUpgradedRequest = 741,
        DocumentDesignMode = 742,
        GlobalCacheStorage = 743,
        NetInfo = 744,
        BackgroundSync = 745,
        TabStopProperty = 746,
        TabStopAttribute = 747,
        SVGFilterRes = 749,
        V8Permissions_Query_Method = 750,
        V8HTMLInputElement_Autocapitalize_AttributeGetter = 754,
        V8HTMLInputElement_Autocapitalize_AttributeSetter = 755,
        V8HTMLTextAreaElement_Autocapitalize_AttributeGetter = 756,
        V8HTMLTextAreaElement_Autocapitalize_AttributeSetter = 757,

        // Add new features immediately above this line. Don't change assigned
        // numbers of any item, and don't reuse removed slots.
        // Also, run update_use_counter_feature_enum.py in chromium/src/tools/metrics/histograms/
        // to update the UMA mapping.
        NumberOfFeatures, // This enum value must be last.
    };

    // "count" sets the bit for this feature to 1. Repeated calls are ignored.
    static void count(const Frame*, Feature);
    static void count(const Document&, Feature);
    // This doesn't count for ExecutionContexts for shared workers and service
    // workers.
    static void count(const ExecutionContext*, Feature);
    // Use countIfNotPrivateScript() instead of count() if you don't want
    // to count metrics in private scripts. You should use
    // countIfNotPrivateScript() in a binding layer.
    static void countIfNotPrivateScript(v8::Isolate*, const Frame*, Feature);
    static void countIfNotPrivateScript(v8::Isolate*, const Document&, Feature);
    static void countIfNotPrivateScript(v8::Isolate*, const ExecutionContext*, Feature);

    void count(CSSParserContext, CSSPropertyID);
    void count(Feature);

    // "countDeprecation" sets the bit for this feature to 1, and sends a deprecation
    // warning to the console. Repeated calls are ignored.
    //
    // Be considerate to developers' consoles: features should only send
    // deprecation warnings when we're actively interested in removing them from
    // the platform.
    //
    // For shared workers and service workers, the ExecutionContext* overload
    // doesn't count the usage but only sends a console warning.
    static void countDeprecation(const LocalFrame*, Feature);
    static void countDeprecation(ExecutionContext*, Feature);
    static void countDeprecation(const Document&, Feature);
    // Use countDeprecationIfNotPrivateScript() instead of countDeprecation()
    // if you don't want to count metrics in private scripts. You should use
    // countDeprecationIfNotPrivateScript() in a binding layer.
    static void countDeprecationIfNotPrivateScript(v8::Isolate*, ExecutionContext*, Feature);
    static String deprecationMessage(Feature);

    void didCommitLoad();

    static UseCounter* getFrom(const Document*);
    static UseCounter* getFrom(const CSSStyleSheet*);
    static UseCounter* getFrom(const StyleSheetContents*);

    static int mapCSSPropertyIdToCSSSampleIdForHistogram(int id);

    static void muteForInspector();
    static void unmuteForInspector();

    class CountBits {
    public:
        bool recordMeasurement(Feature feature)
        {
            if (UseCounter::m_muteCount)
                return false;
            ASSERT(feature != PageDestruction); // PageDestruction is reserved as a scaling factor.
            ASSERT(feature < NumberOfFeatures);
            if (!m_bits) {
                m_bits = adoptPtr(new BitVector(NumberOfFeatures));
                m_bits->clearAll();
            }

            if (m_bits->quickGet(feature))
                return false;

            m_bits->quickSet(feature);
            return true;
        }
        void updateMeasurements();

    private:
        OwnPtr<BitVector> m_bits;
    };

private:
    static int m_muteCount;

    bool recordMeasurement(Feature feature) { return m_countBits.recordMeasurement(feature); }
    void updateMeasurements();

    CountBits m_countBits;
    BitVector m_CSSFeatureBits;
};

} // namespace blink

#endif // UseCounter_h
