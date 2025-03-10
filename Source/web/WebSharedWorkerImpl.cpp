/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
#include "web/WebSharedWorkerImpl.h"

#include "core/dom/CrossThreadTask.h"
#include "core/dom/Document.h"
#include "core/events/MessageEvent.h"
#include "core/html/HTMLFormElement.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/WorkerDebuggerAgent.h"
#include "core/inspector/WorkerInspectorController.h"
#include "core/loader/FrameLoadRequest.h"
#include "core/loader/FrameLoader.h"
#include "core/page/Page.h"
#include "core/workers/SharedWorkerGlobalScope.h"
#include "core/workers/SharedWorkerThread.h"
#include "core/workers/WorkerClients.h"
#include "core/workers/WorkerGlobalScope.h"
#include "core/workers/WorkerInspectorProxy.h"
#include "core/workers/WorkerLoaderProxy.h"
#include "core/workers/WorkerScriptLoader.h"
#include "core/workers/WorkerThreadStartupData.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/heap/Handle.h"
#include "platform/network/ContentSecurityPolicyParsers.h"
#include "platform/network/ResourceResponse.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/WebFileError.h"
#include "public/platform/WebMessagePortChannel.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"
#include "public/platform/WebURLRequest.h"
#include "public/web/WebDevToolsAgent.h"
#include "public/web/WebFrame.h"
#include "public/web/WebServiceWorkerNetworkProvider.h"
#include "public/web/WebSettings.h"
#include "public/web/WebView.h"
#include "public/web/WebWorkerContentSettingsClientProxy.h"
#include "web/LocalFileSystemClient.h"
#include "web/WebDataSourceImpl.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WorkerContentSettingsClient.h"
#include "wtf/Functional.h"
#include "wtf/MainThread.h"

namespace blink {

// A thin wrapper for one-off script loading.
class WebSharedWorkerImpl::Loader : public WorkerScriptLoaderClient {
public:
    static PassOwnPtr<Loader> create()
    {
        return adoptPtr(new Loader());
    }

    virtual ~Loader()
    {
        m_scriptLoader->setClient(0);
    }

    void load(ExecutionContext* loadingContext, const KURL& scriptURL, PassOwnPtr<Closure> receiveResponseCallback, PassOwnPtr<Closure> finishCallback)
    {
        ASSERT(loadingContext);
        m_receiveResponseCallback = receiveResponseCallback;
        m_finishCallback = finishCallback;
        m_scriptLoader->setRequestContext(WebURLRequest::RequestContextSharedWorker);
        m_scriptLoader->loadAsynchronously(
            *loadingContext, scriptURL, DenyCrossOriginRequests, this);
    }

    void didReceiveResponse(unsigned long identifier, const ResourceResponse& response) override
    {
        m_identifier = identifier;
        m_appCacheID = response.appCacheID();
        (*m_receiveResponseCallback)();
    }

    virtual void notifyFinished() override
    {
        (*m_finishCallback)();
    }

    void cancel()
    {
        m_scriptLoader->cancel();
    }

    bool failed() const { return m_scriptLoader->failed(); }
    const KURL& url() const { return m_scriptLoader->responseURL(); }
    String script() const { return m_scriptLoader->script(); }
    unsigned long identifier() const { return m_identifier; }
    long long appCacheID() const { return m_appCacheID; }

private:
    Loader() : m_scriptLoader(WorkerScriptLoader::create()), m_identifier(0), m_appCacheID(0)
    {
    }

    RefPtr<WorkerScriptLoader> m_scriptLoader;
    unsigned long m_identifier;
    long long m_appCacheID;
    OwnPtr<Closure> m_receiveResponseCallback;
    OwnPtr<Closure> m_finishCallback;
};

// This function is called on the main thread to force to initialize some static
// values used in WebKit before any worker thread is started. This is because in
// our worker processs, we do not run any WebKit code in main thread and thus
// when multiple workers try to start at the same time, we might hit crash due
// to contention for initializing static values.
static void initializeWebKitStaticValues()
{
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        // Note that we have to pass a URL with valid protocol in order to follow
        // the path to do static value initializations.
        RefPtr<SecurityOrigin> origin =
            SecurityOrigin::create(KURL(ParsedURLString, "http://localhost"));
        origin.release();
    }
}

WebSharedWorkerImpl::WebSharedWorkerImpl(WebSharedWorkerClient* client)
    : m_webView(0)
    , m_mainFrame(0)
    , m_askedToTerminate(false)
    , m_workerInspectorProxy(WorkerInspectorProxy::create())
    , m_client(WeakReference<WebSharedWorkerClient>::create(client))
    , m_clientWeakPtr(WeakPtr<WebSharedWorkerClient>(m_client))
    , m_pauseWorkerContextOnStart(false)
    , m_isPausedOnStart(false)
{
    initializeWebKitStaticValues();
}

WebSharedWorkerImpl::~WebSharedWorkerImpl()
{
    ASSERT(m_webView);
    // Detach the client before closing the view to avoid getting called back.
    toWebLocalFrameImpl(m_mainFrame)->setClient(0);

    m_webView->close();
    m_mainFrame->close();
    if (m_loaderProxy)
        m_loaderProxy->detachProvider(this);
}

void WebSharedWorkerImpl::stopWorkerThread()
{
    if (m_askedToTerminate)
        return;
    m_askedToTerminate = true;
    if (m_mainScriptLoader) {
        m_mainScriptLoader->cancel();
        m_mainScriptLoader.clear();
        if (client())
            client()->workerScriptLoadFailed();
        delete this;
        return;
    }
    if (m_workerThread)
        m_workerThread->stop();
    m_workerInspectorProxy->workerThreadTerminated();
}

void WebSharedWorkerImpl::initializeLoader()
{
    // Create 'shadow page'. This page is never displayed, it is used to proxy the
    // loading requests from the worker context to the rest of WebKit and Chromium
    // infrastructure.
    ASSERT(!m_webView);
    m_webView = WebView::create(0);
    // FIXME: http://crbug.com/363843. This needs to find a better way to
    // not create graphics layers.
    m_webView->settings()->setAcceleratedCompositingEnabled(false);
    // FIXME: Settings information should be passed to the Worker process from Browser process when the worker
    // is created (similar to RenderThread::OnCreateNewView).
    m_mainFrame = WebLocalFrame::create(this);
    m_webView->setMainFrame(m_mainFrame);
    m_webView->setDevToolsAgentClient(this);

    // If we were asked to pause worker context on start and wait for debugger then it is the good time to do that.
    client()->workerReadyForInspection();
    if (m_pauseWorkerContextOnStart) {
        m_isPausedOnStart = true;
        return;
    }
    loadShadowPage();
}

WebApplicationCacheHost* WebSharedWorkerImpl::createApplicationCacheHost(WebLocalFrame*, WebApplicationCacheHostClient* appcacheHostClient)
{
    if (client())
        return client()->createApplicationCacheHost(appcacheHostClient);
    return 0;
}

void WebSharedWorkerImpl::loadShadowPage()
{
    WebLocalFrameImpl* webFrame = toWebLocalFrameImpl(m_webView->mainFrame());

    // Construct substitute data source for the 'shadow page'. We only need it
    // to have same origin as the worker so the loading checks work correctly.
    CString content("");
    RefPtr<SharedBuffer> buffer(SharedBuffer::create(content.data(), content.length()));
    webFrame->frame()->loader().load(FrameLoadRequest(0, ResourceRequest(m_url), SubstituteData(buffer, "text/html", "UTF-8", KURL())));
}

void WebSharedWorkerImpl::willSendRequest(
    WebLocalFrame* frame, unsigned, WebURLRequest& request,
    const WebURLResponse& redirectResponse)
{
    if (m_networkProvider)
        m_networkProvider->willSendRequest(frame->dataSource(), request);
}

void WebSharedWorkerImpl::didFinishDocumentLoad(WebLocalFrame* frame)
{
    ASSERT(!m_loadingDocument);
    ASSERT(!m_mainScriptLoader);
    m_networkProvider = adoptPtr(client()->createServiceWorkerNetworkProvider(frame->dataSource()));
    m_mainScriptLoader = Loader::create();
    m_loadingDocument = toWebLocalFrameImpl(frame)->frame()->document();
    m_mainScriptLoader->load(
        m_loadingDocument.get(),
        m_url,
        bind(&WebSharedWorkerImpl::didReceiveScriptLoaderResponse, this),
        bind(&WebSharedWorkerImpl::onScriptLoaderFinished, this));
}

bool WebSharedWorkerImpl::isControlledByServiceWorker(WebDataSource& dataSource)
{
    return m_networkProvider && m_networkProvider->isControlledByServiceWorker(dataSource);
}

int64_t WebSharedWorkerImpl::serviceWorkerID(WebDataSource& dataSource)
{
    if (!m_networkProvider)
        return -1;
    return m_networkProvider->serviceWorkerID(dataSource);
}

void WebSharedWorkerImpl::sendProtocolMessage(int callId, const WebString& message, const WebString& state)
{
    client()->sendDevToolsMessage(callId, message, state);
}

void WebSharedWorkerImpl::resumeStartup()
{
    bool isPausedOnStart = m_isPausedOnStart;
    m_isPausedOnStart = false;
    if (isPausedOnStart)
        loadShadowPage();
}

// WorkerReportingProxy --------------------------------------------------------

void WebSharedWorkerImpl::reportException(const String& errorMessage, int lineNumber, int columnNumber, const String& sourceURL, int exceptionId)
{
    // Not suppported in SharedWorker.
}

void WebSharedWorkerImpl::reportConsoleMessage(PassRefPtrWillBeRawPtr<ConsoleMessage>)
{
    // Not supported in SharedWorker.
}

void WebSharedWorkerImpl::postMessageToPageInspector(const String& message)
{
    toWebLocalFrameImpl(m_mainFrame)->frame()->document()->postInspectorTask(FROM_HERE, createCrossThreadTask(&WebSharedWorkerImpl::postMessageToPageInspectorOnMainThread, this, message));
}

void WebSharedWorkerImpl::postMessageToPageInspectorOnMainThread(const String& message)
{
    WorkerInspectorProxy::PageInspector* pageInspector = m_workerInspectorProxy->pageInspector();
    if (!pageInspector)
        return;
    pageInspector->dispatchMessageFromWorker(message);

}

void WebSharedWorkerImpl::workerGlobalScopeClosed()
{
    Platform::current()->mainThread()->postTask(FROM_HERE, bind(&WebSharedWorkerImpl::workerGlobalScopeClosedOnMainThread, this));
}

void WebSharedWorkerImpl::workerGlobalScopeClosedOnMainThread()
{
    if (client())
        client()->workerContextClosed();

    stopWorkerThread();
}

void WebSharedWorkerImpl::workerGlobalScopeStarted(WorkerGlobalScope*)
{
}

void WebSharedWorkerImpl::workerThreadTerminated()
{
    Platform::current()->mainThread()->postTask(FROM_HERE, bind(&WebSharedWorkerImpl::workerThreadTerminatedOnMainThread, this));
}

void WebSharedWorkerImpl::workerThreadTerminatedOnMainThread()
{
    if (client())
        client()->workerContextDestroyed();
    // The lifetime of this proxy is controlled by the worker context.
    delete this;
}

// WorkerLoaderProxyProvider -----------------------------------------------------------

void WebSharedWorkerImpl::postTaskToLoader(PassOwnPtr<ExecutionContextTask> task)
{
    toWebLocalFrameImpl(m_mainFrame)->frame()->document()->postTask(FROM_HERE, task);
}

bool WebSharedWorkerImpl::postTaskToWorkerGlobalScope(PassOwnPtr<ExecutionContextTask> task)
{
    m_workerThread->postTask(FROM_HERE, task);
    return true;
}

void WebSharedWorkerImpl::connect(WebMessagePortChannel* webChannel)
{
    workerThread()->postTask(
        FROM_HERE, createCrossThreadTask(&connectTask, adoptPtr(webChannel)));
}

void WebSharedWorkerImpl::connectTask(ExecutionContext* context, PassOwnPtr<WebMessagePortChannel> channel)
{
    // Wrap the passed-in channel in a MessagePort, and send it off via a connect event.
    RefPtrWillBeRawPtr<MessagePort> port = MessagePort::create(*context);
    port->entangle(channel);
    WorkerGlobalScope* workerGlobalScope = toWorkerGlobalScope(context);
    ASSERT_WITH_SECURITY_IMPLICATION(workerGlobalScope->isSharedWorkerGlobalScope());
    workerGlobalScope->dispatchEvent(createConnectEvent(port.release()));
}

void WebSharedWorkerImpl::startWorkerContext(const WebURL& url, const WebString& name, const WebString& contentSecurityPolicy, WebContentSecurityPolicyType policyType)
{
    m_url = url;
    m_name = name;
    m_contentSecurityPolicy = contentSecurityPolicy;
    m_policyType = policyType;
    initializeLoader();
}

void WebSharedWorkerImpl::didReceiveScriptLoaderResponse()
{
    InspectorInstrumentation::didReceiveScriptResponse(m_loadingDocument.get(), m_mainScriptLoader->identifier());
    if (client())
        client()->selectAppCacheID(m_mainScriptLoader->appCacheID());
}

void WebSharedWorkerImpl::onScriptLoaderFinished()
{
    ASSERT(m_loadingDocument);
    ASSERT(m_mainScriptLoader);
    if (m_askedToTerminate)
        return;
    if (m_mainScriptLoader->failed()) {
        m_mainScriptLoader->cancel();
        if (client())
            client()->workerScriptLoadFailed();

        // The SharedWorker was unable to load the initial script, so
        // shut it down right here.
        delete this;
        return;
    }

    Document* document = toWebLocalFrameImpl(m_mainFrame)->frame()->document();
    WorkerThreadStartMode startMode = DontPauseWorkerGlobalScopeOnStart;
    if (InspectorInstrumentation::shouldPauseDedicatedWorkerOnStart(document))
        startMode = PauseWorkerGlobalScopeOnStart;

    // FIXME: this document's origin is pristine and without any extra privileges. (crbug.com/254993)
    SecurityOrigin* starterOrigin = document->securityOrigin();

    OwnPtrWillBeRawPtr<WorkerClients> workerClients = WorkerClients::create();
    provideLocalFileSystemToWorker(workerClients.get(), LocalFileSystemClient::create());
    WebSecurityOrigin webSecurityOrigin(m_loadingDocument->securityOrigin());
    provideContentSettingsClientToWorker(workerClients.get(), adoptPtr(client()->createWorkerContentSettingsClientProxy(webSecurityOrigin)));
    OwnPtr<WorkerThreadStartupData> startupData = WorkerThreadStartupData::create(m_url, m_loadingDocument->userAgent(m_url), m_mainScriptLoader->script(), nullptr, startMode, m_contentSecurityPolicy, static_cast<ContentSecurityPolicyHeaderType>(m_policyType), starterOrigin, workerClients.release());
    m_loaderProxy = WorkerLoaderProxy::create(this);
    setWorkerThread(SharedWorkerThread::create(m_name, m_loaderProxy, *this, startupData.release()));
    InspectorInstrumentation::scriptImported(m_loadingDocument.get(), m_mainScriptLoader->identifier(), m_mainScriptLoader->script());
    m_mainScriptLoader.clear();

    workerThread()->start();
    m_workerInspectorProxy->workerThreadCreated(m_loadingDocument.get(), workerThread(), m_url);
    if (client())
        client()->workerScriptLoaded();
}

void WebSharedWorkerImpl::terminateWorkerContext()
{
    stopWorkerThread();
}

void WebSharedWorkerImpl::clientDestroyed()
{
    m_client.clear();
}

void WebSharedWorkerImpl::pauseWorkerContextOnStart()
{
    m_pauseWorkerContextOnStart = true;
}

void WebSharedWorkerImpl::attachDevTools(const WebString& hostId)
{
    WebDevToolsAgent* devtoolsAgent = m_webView->devToolsAgent();
    if (devtoolsAgent)
        devtoolsAgent->attach(hostId);
}

void WebSharedWorkerImpl::reattachDevTools(const WebString& hostId, const WebString& savedState)
{
    WebDevToolsAgent* devtoolsAgent = m_webView->devToolsAgent();
    if (devtoolsAgent)
        devtoolsAgent->reattach(hostId, savedState);
    resumeStartup();
}

void WebSharedWorkerImpl::detachDevTools()
{
    WebDevToolsAgent* devtoolsAgent = m_webView->devToolsAgent();
    if (devtoolsAgent)
        devtoolsAgent->detach();
}

void WebSharedWorkerImpl::dispatchDevToolsMessage(const WebString& message)
{
    if (m_askedToTerminate)
        return;
    WebDevToolsAgent* devtoolsAgent = m_webView->devToolsAgent();
    if (devtoolsAgent)
        devtoolsAgent->dispatchOnInspectorBackend(message);
}

WebSharedWorker* WebSharedWorker::create(WebSharedWorkerClient* client)
{
    return new WebSharedWorkerImpl(client);
}

} // namespace blink
