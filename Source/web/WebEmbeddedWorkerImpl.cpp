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

#include "config.h"
#include "web/WebEmbeddedWorkerImpl.h"

#include "core/dom/CrossThreadTask.h"
#include "core/dom/Document.h"
#include "core/fetch/SubstituteData.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/WorkerDebuggerAgent.h"
#include "core/inspector/WorkerInspectorController.h"
#include "core/loader/FrameLoadRequest.h"
#include "core/workers/WorkerClients.h"
#include "core/workers/WorkerGlobalScope.h"
#include "core/workers/WorkerInspectorProxy.h"
#include "core/workers/WorkerLoaderProxy.h"
#include "core/workers/WorkerScriptLoader.h"
#include "core/workers/WorkerScriptLoaderClient.h"
#include "core/workers/WorkerThreadStartupData.h"
#include "modules/serviceworkers/ServiceWorkerContainerClient.h"
#include "modules/serviceworkers/ServiceWorkerThread.h"
#include "platform/SharedBuffer.h"
#include "platform/heap/Handle.h"
#include "platform/network/ContentSecurityPolicyParsers.h"
#include "platform/network/ContentSecurityPolicyResponseHeaders.h"
#include "public/platform/Platform.h"
#include "public/platform/WebServiceWorkerProvider.h"
#include "public/platform/WebURLRequest.h"
#include "public/web/WebDevToolsAgent.h"
#include "public/web/WebServiceWorkerContextClient.h"
#include "public/web/WebServiceWorkerNetworkProvider.h"
#include "public/web/WebSettings.h"
#include "public/web/WebView.h"
#include "public/web/WebWorkerContentSettingsClientProxy.h"
#include "web/ServiceWorkerGlobalScopeClientImpl.h"
#include "web/ServiceWorkerGlobalScopeProxy.h"
#include "web/WebDataSourceImpl.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WorkerContentSettingsClient.h"
#include "wtf/Functional.h"

namespace blink {

// A thin wrapper for one-off script loading.
class WebEmbeddedWorkerImpl::Loader : public WorkerScriptLoaderClient {
public:
    static PassOwnPtr<Loader> create()
    {
        return adoptPtr(new Loader());
    }

    virtual ~Loader()
    {
        m_scriptLoader->setClient(0);
    }

    void load(ExecutionContext* loadingContext, const KURL& scriptURL, PassOwnPtr<Closure> callback)
    {
        ASSERT(loadingContext);
        m_callback = callback;
        m_scriptLoader->setRequestContext(WebURLRequest::RequestContextServiceWorker);
        m_scriptLoader->loadAsynchronously(
            *loadingContext, scriptURL, DenyCrossOriginRequests, this);
    }

    void didReceiveResponse(unsigned long identifier, const ResourceResponse& response) override
    {
        m_contentSecurityPolicy = ContentSecurityPolicy::create();
        m_contentSecurityPolicy->setOverrideURLForSelf(response.url());
        m_contentSecurityPolicy->didReceiveHeaders(ContentSecurityPolicyResponseHeaders(response));
    }

    virtual void notifyFinished() override
    {
        (*m_callback)();
    }

    void cancel()
    {
        m_scriptLoader->cancel();
    }

    bool failed() const { return m_scriptLoader->failed(); }
    const KURL& url() const { return m_scriptLoader->responseURL(); }
    String script() const { return m_scriptLoader->script(); }
    const Vector<char>* cachedMetadata() const { return m_scriptLoader->cachedMetadata(); }
    PassOwnPtr<Vector<char>> releaseCachedMetadata() const { return m_scriptLoader->releaseCachedMetadata(); }
    PassRefPtr<ContentSecurityPolicy> releaseContentSecurityPolicy() { return m_contentSecurityPolicy.release(); }

private:
    Loader() : m_scriptLoader(WorkerScriptLoader::create())
    {
    }

    RefPtr<WorkerScriptLoader> m_scriptLoader;
    OwnPtr<Closure> m_callback;
    RefPtr<ContentSecurityPolicy> m_contentSecurityPolicy;
};

WebEmbeddedWorker* WebEmbeddedWorker::create(WebServiceWorkerContextClient* client, WebWorkerContentSettingsClientProxy* contentSettingsClient)
{
    return new WebEmbeddedWorkerImpl(adoptPtr(client), adoptPtr(contentSettingsClient));
}

static HashSet<WebEmbeddedWorkerImpl*>& runningWorkerInstances()
{
    DEFINE_STATIC_LOCAL(HashSet<WebEmbeddedWorkerImpl*>, set, ());
    return set;
}

WebEmbeddedWorkerImpl::WebEmbeddedWorkerImpl(PassOwnPtr<WebServiceWorkerContextClient> client, PassOwnPtr<WebWorkerContentSettingsClientProxy> ContentSettingsClient)
    : m_workerContextClient(client)
    , m_contentSettingsClient(ContentSettingsClient)
    , m_workerInspectorProxy(WorkerInspectorProxy::create())
    , m_webView(0)
    , m_mainFrame(0)
    , m_askedToTerminate(false)
    , m_pauseAfterDownloadState(DontPauseAfterDownload)
    , m_waitingForDebuggerState(NotWaitingForDebugger)
{
    runningWorkerInstances().add(this);
}

WebEmbeddedWorkerImpl::~WebEmbeddedWorkerImpl()
{
    // Prevent onScriptLoaderFinished from deleting 'this'.
    m_askedToTerminate = true;

    if (m_workerThread)
        m_workerThread->terminateAndWait();

    ASSERT(runningWorkerInstances().contains(this));
    runningWorkerInstances().remove(this);
    ASSERT(m_webView);

    // Detach the client before closing the view to avoid getting called back.
    toWebLocalFrameImpl(m_mainFrame)->setClient(0);

    m_webView->close();
    m_mainFrame->close();
    if (m_loaderProxy)
        m_loaderProxy->detachProvider(this);
}

void WebEmbeddedWorkerImpl::terminateAll()
{
    HashSet<WebEmbeddedWorkerImpl*> instances = runningWorkerInstances();
    for (HashSet<WebEmbeddedWorkerImpl*>::iterator it = instances.begin(), itEnd = instances.end(); it != itEnd; ++it) {
        (*it)->terminateWorkerContext();
    }
}

void WebEmbeddedWorkerImpl::startWorkerContext(
    const WebEmbeddedWorkerStartData& data)
{
    ASSERT(!m_askedToTerminate);
    ASSERT(!m_mainScriptLoader);
    ASSERT(m_pauseAfterDownloadState == DontPauseAfterDownload);
    m_workerStartData = data;
    if (data.pauseAfterDownloadMode == WebEmbeddedWorkerStartData::PauseAfterDownload)
        m_pauseAfterDownloadState = DoPauseAfterDownload;
    prepareShadowPageForLoader();
}

void WebEmbeddedWorkerImpl::terminateWorkerContext()
{
    if (m_askedToTerminate)
        return;
    m_askedToTerminate = true;
    if (m_mainScriptLoader) {
        m_mainScriptLoader->cancel();
        m_mainScriptLoader.clear();
        // This may delete 'this'.
        m_workerContextClient->workerContextFailedToStart();
        return;
    }
    if (m_pauseAfterDownloadState == IsPausedAfterDownload) {
        // This may delete 'this'.
        m_workerContextClient->workerContextFailedToStart();
        return;
    }
    if (m_workerThread)
        m_workerThread->stop();
    m_workerInspectorProxy->workerThreadTerminated();
}

void WebEmbeddedWorkerImpl::resumeAfterDownload()
{
    ASSERT(!m_askedToTerminate);
    bool wasPaused = (m_pauseAfterDownloadState == IsPausedAfterDownload);
    m_pauseAfterDownloadState = DontPauseAfterDownload;

    // If we were asked to wait for debugger while updating service worker version then it is good time now.
    m_workerContextClient->workerReadyForInspection();
    if (m_workerStartData.waitForDebuggerMode == WebEmbeddedWorkerStartData::WaitForDebugger)
        m_waitingForDebuggerState = WaitingForDebuggerAfterScriptLoaded;
    else if (wasPaused)
        startWorkerThread();
}

void WebEmbeddedWorkerImpl::attachDevTools(const WebString& hostId)
{
    WebDevToolsAgent* devtoolsAgent = m_webView->devToolsAgent();
    if (devtoolsAgent)
        devtoolsAgent->attach(hostId);
}

void WebEmbeddedWorkerImpl::reattachDevTools(const WebString& hostId, const WebString& savedState)
{
    WebDevToolsAgent* devtoolsAgent = m_webView->devToolsAgent();
    if (devtoolsAgent)
        devtoolsAgent->reattach(hostId, savedState);
    resumeStartup();
}

void WebEmbeddedWorkerImpl::detachDevTools()
{
    WebDevToolsAgent* devtoolsAgent = m_webView->devToolsAgent();
    if (devtoolsAgent)
        devtoolsAgent->detach();
}

void WebEmbeddedWorkerImpl::dispatchDevToolsMessage(const WebString& message)
{
    if (m_askedToTerminate)
        return;
    WebDevToolsAgent* devtoolsAgent = m_webView->devToolsAgent();
    if (devtoolsAgent)
        devtoolsAgent->dispatchOnInspectorBackend(message);
}

void WebEmbeddedWorkerImpl::postMessageToPageInspector(const String& message)
{
    WorkerInspectorProxy::PageInspector* pageInspector = m_workerInspectorProxy->pageInspector();
    if (!pageInspector)
        return;
    pageInspector->dispatchMessageFromWorker(message);
}

void WebEmbeddedWorkerImpl::postTaskToLoader(PassOwnPtr<ExecutionContextTask> task)
{
    toWebLocalFrameImpl(m_mainFrame)->frame()->document()->postTask(FROM_HERE, task);
}

bool WebEmbeddedWorkerImpl::postTaskToWorkerGlobalScope(PassOwnPtr<ExecutionContextTask> task)
{
    if (m_askedToTerminate || !m_workerThread)
        return false;

    m_workerThread->postTask(FROM_HERE, task);
    return !m_workerThread->terminated();
}

void WebEmbeddedWorkerImpl::prepareShadowPageForLoader()
{
    // Create 'shadow page', which is never displayed and is used mainly to
    // provide a context for loading on the main thread.
    //
    // FIXME: This does mostly same as WebSharedWorkerImpl::initializeLoader.
    // This code, and probably most of the code in this class should be shared
    // with SharedWorker.
    ASSERT(!m_webView);
    m_webView = WebView::create(0);
    WebSettings* settings = m_webView->settings();
    // FIXME: http://crbug.com/363843. This needs to find a better way to
    // not create graphics layers.
    settings->setAcceleratedCompositingEnabled(false);
    // Currently we block all mixed-content requests from a ServiceWorker.
    // FIXME: When we support FetchEvent.default(), we should relax this
    // restriction.
    settings->setStrictMixedContentChecking(true);
    settings->setAllowDisplayOfInsecureContent(false);
    settings->setAllowRunningOfInsecureContent(false);
    m_mainFrame = WebLocalFrame::create(this);
    m_webView->setMainFrame(m_mainFrame);
    m_webView->setDevToolsAgentClient(this);

    // If we were asked to wait for debugger then it is the good time to do that.
    // However if we are updating service worker version (m_pauseAfterDownloadState is set)
    // Then we need to load the worker script to check the version, so in this case we wait for debugger
    // later in ::resumeAfterDownload().
    if (m_pauseAfterDownloadState != DoPauseAfterDownload) {
        m_workerContextClient->workerReadyForInspection();
        if (m_workerStartData.waitForDebuggerMode == WebEmbeddedWorkerStartData::WaitForDebugger) {
            m_waitingForDebuggerState = WaitingForDebuggerBeforeLoadingScript;
            return;
        }
    }

    loadShadowPage();
}

void WebEmbeddedWorkerImpl::loadShadowPage()
{
    WebLocalFrameImpl* webFrame = toWebLocalFrameImpl(m_webView->mainFrame());
    // Construct substitute data source for the 'shadow page'. We only need it
    // to have same origin as the worker so the loading checks work correctly.
    CString content("");
    int length = static_cast<int>(content.length());
    RefPtr<SharedBuffer> buffer(SharedBuffer::create(content.data(), length));
    webFrame->frame()->loader().load(FrameLoadRequest(0, ResourceRequest(m_workerStartData.scriptURL), SubstituteData(buffer, "text/html", "UTF-8", KURL())));
}

void WebEmbeddedWorkerImpl::willSendRequest(
    WebLocalFrame* frame, unsigned, WebURLRequest& request,
    const WebURLResponse& redirectResponse)
{
    if (m_networkProvider)
        m_networkProvider->willSendRequest(frame->dataSource(), request);
}

void WebEmbeddedWorkerImpl::didFinishDocumentLoad(WebLocalFrame* frame)
{
    ASSERT(!m_mainScriptLoader);
    ASSERT(!m_networkProvider);
    ASSERT(m_mainFrame);
    ASSERT(m_workerContextClient);
    m_networkProvider = adoptPtr(m_workerContextClient->createServiceWorkerNetworkProvider(frame->dataSource()));
    m_mainScriptLoader = Loader::create();
    m_mainScriptLoader->load(
        toWebLocalFrameImpl(m_mainFrame)->frame()->document(),
        m_workerStartData.scriptURL,
        bind(&WebEmbeddedWorkerImpl::onScriptLoaderFinished, this));
}

void WebEmbeddedWorkerImpl::sendProtocolMessage(int callId, const WebString& message, const WebString& state)
{
    m_workerContextClient->sendDevToolsMessage(callId, message, state);
}

void WebEmbeddedWorkerImpl::resumeStartup()
{
    WaitingForDebuggerState waitingForDebuggerState = m_waitingForDebuggerState;
    m_waitingForDebuggerState = NotWaitingForDebugger;
    if (waitingForDebuggerState == WaitingForDebuggerBeforeLoadingScript)
        loadShadowPage();
    else if (waitingForDebuggerState == WaitingForDebuggerAfterScriptLoaded)
        startWorkerThread();
}

void WebEmbeddedWorkerImpl::onScriptLoaderFinished()
{
    ASSERT(m_mainScriptLoader);

    if (m_askedToTerminate)
        return;

    if (m_mainScriptLoader->failed()) {
        m_mainScriptLoader.clear();
        // This may delete 'this'.
        m_workerContextClient->workerContextFailedToStart();
        return;
    }

    Platform::current()->histogramCustomCounts("ServiceWorker.ScriptSize", m_mainScriptLoader->script().length(), 1000, 5000000, 50);
    if (m_mainScriptLoader->cachedMetadata())
        Platform::current()->histogramCustomCounts("ServiceWorker.ScriptCachedMetadataSize", m_mainScriptLoader->cachedMetadata()->size(), 1000, 50000000, 50);

    if (m_pauseAfterDownloadState == DoPauseAfterDownload) {
        m_pauseAfterDownloadState = IsPausedAfterDownload;
        m_workerContextClient->didPauseAfterDownload();
        return;
    }
    startWorkerThread();
}

void WebEmbeddedWorkerImpl::startWorkerThread()
{
    ASSERT(m_pauseAfterDownloadState == DontPauseAfterDownload);
    ASSERT(!m_askedToTerminate);

    Document* document = toWebLocalFrameImpl(m_mainFrame)->frame()->document();

    WorkerThreadStartMode startMode = DontPauseWorkerGlobalScopeOnStart;
    if (InspectorInstrumentation::shouldPauseDedicatedWorkerOnStart(document))
        startMode = PauseWorkerGlobalScopeOnStart;

    // FIXME: this document's origin is pristine and without any extra privileges. (crbug.com/254993)
    SecurityOrigin* starterOrigin = document->securityOrigin();

    OwnPtrWillBeRawPtr<WorkerClients> workerClients = WorkerClients::create();
    provideContentSettingsClientToWorker(workerClients.get(), m_contentSettingsClient.release());
    provideServiceWorkerGlobalScopeClientToWorker(workerClients.get(), ServiceWorkerGlobalScopeClientImpl::create(*m_workerContextClient));
    provideServiceWorkerContainerClientToWorker(workerClients.get(), adoptPtr(m_workerContextClient->createServiceWorkerProvider()));

    // We need to set the CSP to both the shadow page's document and the ServiceWorkerGlobalScope.
    document->initContentSecurityPolicy(m_mainScriptLoader->releaseContentSecurityPolicy());

    KURL scriptURL = m_mainScriptLoader->url();
    OwnPtr<WorkerThreadStartupData> startupData =
        WorkerThreadStartupData::create(
            scriptURL,
            m_workerStartData.userAgent,
            m_mainScriptLoader->script(),
            m_mainScriptLoader->releaseCachedMetadata(),
            startMode,
            document->contentSecurityPolicy()->deprecatedHeader(),
            document->contentSecurityPolicy()->deprecatedHeaderType(),
            starterOrigin,
            workerClients.release(),
            static_cast<blink::V8CacheOptions>(m_workerStartData.v8CacheOptions));

    m_mainScriptLoader.clear();

    m_workerGlobalScopeProxy = ServiceWorkerGlobalScopeProxy::create(*this, *document, *m_workerContextClient);
    m_loaderProxy = WorkerLoaderProxy::create(this);
    m_workerThread = ServiceWorkerThread::create(m_loaderProxy, *m_workerGlobalScopeProxy, startupData.release());
    m_workerThread->start();
    m_workerInspectorProxy->workerThreadCreated(document, m_workerThread.get(), scriptURL);
}

} // namespace blink
