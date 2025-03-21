/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef InspectorPageAgent_h
#define InspectorPageAgent_h


#include "core/InspectorFrontend.h"
#include "core/inspector/InspectorBaseAgent.h"
#include "core/inspector/InspectorResourceContentLoader.h"
#include "wtf/HashMap.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Resource;
class Document;
class DocumentLoader;
class FrameHost;
class InjectedScriptManager;
class InspectorCSSAgent;
class InspectorDebuggerAgent;
class InspectorOverlay;
class InspectorResourceContentLoader;
class KURL;
class LocalFrame;
class SharedBuffer;
class TextResourceDecoder;

typedef String ErrorString;

class InspectorPageAgent final : public InspectorBaseAgent<InspectorPageAgent, InspectorFrontend::Page>, public InspectorBackendDispatcher::PageCommandHandler {
    WTF_MAKE_NONCOPYABLE(InspectorPageAgent);
public:
    enum ResourceType {
        DocumentResource,
        StylesheetResource,
        ImageResource,
        FontResource,
        MediaResource,
        ScriptResource,
        TextTrackResource,
        XHRResource,
        WebSocketResource,
        OtherResource
    };

    static PassOwnPtrWillBeRawPtr<InspectorPageAgent> create(LocalFrame* inspectedFrame, InjectedScriptManager*, InspectorOverlay*);
    void setDeferredAgents(InspectorDebuggerAgent*, InspectorCSSAgent*);

    static Vector<Document*> importsForFrame(LocalFrame*);
    static bool cachedResourceContent(Resource*, String* result, bool* base64Encoded);
    static bool sharedBufferContent(PassRefPtr<SharedBuffer>, const String& textEncodingName, bool withBase64Encode, String* result);

    static PassRefPtr<SharedBuffer> resourceData(LocalFrame*, const KURL&, String* textEncodingName);
    static Resource* cachedResource(LocalFrame*, const KURL&);
    static TypeBuilder::Page::ResourceType::Enum resourceTypeJson(ResourceType);
    static ResourceType cachedResourceType(const Resource&);
    static TypeBuilder::Page::ResourceType::Enum cachedResourceTypeJson(const Resource&);
    static PassOwnPtr<TextResourceDecoder> createResourceTextDecoder(const String& mimeType, const String& textEncodingName);

    // Page API for InspectorFrontend
    void enable(ErrorString*) override;
    void addScriptToEvaluateOnLoad(ErrorString*, const String& source, String* result) override;
    void removeScriptToEvaluateOnLoad(ErrorString*, const String& identifier) override;
    void reload(ErrorString*, const bool* optionalIgnoreCache, const String* optionalScriptToEvaluateOnLoad) override;
    void navigate(ErrorString*, const String& url, String* frameId) override;
    void getCookies(ErrorString*, RefPtr<TypeBuilder::Array<TypeBuilder::Page::Cookie> >& cookies) override;
    void deleteCookie(ErrorString*, const String& cookieName, const String& url) override;
    void getResourceTree(ErrorString*, RefPtr<TypeBuilder::Page::FrameResourceTree>&) override;
    void getResourceContent(ErrorString*, const String& frameId, const String& url, PassRefPtrWillBeRawPtr<GetResourceContentCallback>) override;
    void searchInResource(ErrorString*, const String& frameId, const String& url, const String& query, const bool* optionalCaseSensitive, const bool* optionalIsRegex, RefPtr<TypeBuilder::Array<TypeBuilder::Debugger::SearchMatch>>&) override;
    void setDocumentContent(ErrorString*, const String& frameId, const String& html) override;
    void startScreencast(ErrorString*, const String* format, const int* quality, const int* maxWidth, const int* maxHeight) override;
    void stopScreencast(ErrorString*) override;
    void setShowViewportSizeOnResize(ErrorString*, bool show, const bool* showGrid) override;
    void setOverlayMessage(ErrorString*, const String*) override;

    // InspectorInstrumentation API
    void didClearDocumentOfWindowObject(LocalFrame*);
    void domContentLoadedEventFired(LocalFrame*);
    void loadEventFired(LocalFrame*);
    void didCommitLoad(LocalFrame*, DocumentLoader*);
    void frameAttachedToParent(LocalFrame*);
    void frameDetachedFromParent(LocalFrame*);
    void loaderDetachedFromFrame(DocumentLoader*);
    void frameStartedLoading(LocalFrame*);
    void frameStoppedLoading(LocalFrame*);
    void frameScheduledNavigation(LocalFrame*, double delay);
    void frameClearedScheduledNavigation(LocalFrame*);
    void willRunJavaScriptDialog(const String& message);
    void didRunJavaScriptDialog();
    void didLayout();
    void didScroll();
    void didResizeMainFrame();
    void didRecalculateStyle(int);

    // Inspector Controller API
    void disable(ErrorString*) override;
    void restore() override;
    void discardAgent() override;

    // Cross-agents API
    FrameHost* frameHost();
    LocalFrame* inspectedFrame() const { return m_inspectedFrame.get(); }
    String createIdentifier();
    LocalFrame* frameForId(const String& frameId);
    String frameId(LocalFrame*);
    bool hasIdForFrame(LocalFrame*) const;
    String loaderId(DocumentLoader*);
    LocalFrame* findFrameWithSecurityOrigin(const String& originRawString);
    LocalFrame* assertFrame(ErrorString*, const String& frameId);
    bool screencastEnabled();
    static DocumentLoader* assertDocumentLoader(ErrorString*, LocalFrame*);
    InspectorResourceContentLoader* resourceContentLoader() { return m_inspectorResourceContentLoader.get(); }

    DECLARE_VIRTUAL_TRACE();

private:
    class GetResourceContentLoadListener;

    InspectorPageAgent(LocalFrame* inspectedFrame, InjectedScriptManager*, InspectorOverlay*);

    void finishReload();
    void getResourceContentAfterResourcesContentLoaded(const String& frameId, const String& url, PassRefPtrWillBeRawPtr<GetResourceContentCallback>);

    static bool dataContent(const char* data, unsigned size, const String& textEncodingName, bool withBase64Encode, String* result);

    PassRefPtr<TypeBuilder::Page::Frame> buildObjectForFrame(LocalFrame*);
    PassRefPtr<TypeBuilder::Page::FrameResourceTree> buildObjectForFrameTree(LocalFrame*);
    RawPtrWillBeMember<LocalFrame> m_inspectedFrame;
    RawPtrWillBeMember<InjectedScriptManager> m_injectedScriptManager;
    RawPtrWillBeMember<InspectorDebuggerAgent> m_debuggerAgent;
    RawPtrWillBeMember<InspectorCSSAgent> m_cssAgent;
    RawPtrWillBeMember<InspectorOverlay> m_overlay;
    long m_lastScriptIdentifier;
    String m_pendingScriptToEvaluateOnLoadOnce;
    String m_scriptToEvaluateOnLoadOnce;
    HashMap<LocalFrame*, String> m_frameToIdentifier;
    HashMap<String, LocalFrame*> m_identifierToFrame;
    HashMap<DocumentLoader*, String> m_loaderToIdentifier;
    bool m_enabled;
    bool m_reloading;

    OwnPtrWillBeMember<InspectorResourceContentLoader> m_inspectorResourceContentLoader;
};


} // namespace blink


#endif // !defined(InspectorPagerAgent_h)
