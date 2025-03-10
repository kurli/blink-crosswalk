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

#ifndef ServiceWorkerGlobalScopeProxy_h
#define ServiceWorkerGlobalScopeProxy_h

#include "core/workers/WorkerReportingProxy.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebString.h"
#include "public/web/WebServiceWorkerContextProxy.h"
#include "wtf/Forward.h"
#include "wtf/OwnPtr.h"

namespace blink {

class ConsoleMessage;
class Document;
class ServiceWorkerGlobalScope;
class WebEmbeddedWorkerImpl;
class WebServiceWorkerContextClient;
class WebServiceWorkerRequest;

// This class is created and destructed on the main thread, but live most
// of its time as a resident of the worker thread.
// All methods other than its ctor/dtor are called on the worker thread.
//
// This implements WebServiceWorkerContextProxy, which connects ServiceWorker's
// WorkerGlobalScope and embedder/chrome, and implements ServiceWorker-specific
// events/upcall methods that are to be called by embedder/chromium,
// e.g. onfetch.
//
// An instance of this class is supposed to outlive until
// workerThreadTerminated() is called by its corresponding
// WorkerGlobalScope.
class ServiceWorkerGlobalScopeProxy final
    : public WebServiceWorkerContextProxy
    , public WorkerReportingProxy {
    WTF_MAKE_NONCOPYABLE(ServiceWorkerGlobalScopeProxy);
public:
    static PassOwnPtr<ServiceWorkerGlobalScopeProxy> create(WebEmbeddedWorkerImpl&, Document&, WebServiceWorkerContextClient&);
    virtual ~ServiceWorkerGlobalScopeProxy();

    // WebServiceWorkerContextProxy overrides:
    virtual void setRegistration(WebServiceWorkerRegistration*) override;
    virtual void dispatchActivateEvent(int) override;
    virtual void dispatchCrossOriginConnectEvent(int, const WebCrossOriginServiceWorkerClient&) override;
    virtual void dispatchCrossOriginMessageEvent(const WebCrossOriginServiceWorkerClient&, const WebString& message, const WebMessagePortChannelArray&) override;
    virtual void dispatchFetchEvent(int, const WebServiceWorkerRequest&) override;
    virtual void dispatchGeofencingEvent(int, WebGeofencingEventType, const WebString& regionID, const WebCircularGeofencingRegion&) override;
    virtual void dispatchInstallEvent(int) override;
    virtual void dispatchMessageEvent(const WebString& message, const WebMessagePortChannelArray&) override;
    virtual void dispatchNotificationClickEvent(int, const WebString& notificationID, const WebNotificationData&) override;
    virtual void dispatchPushEvent(int, const WebString& data) override;
    virtual void dispatchSyncEvent(int) override;

    // WorkerReportingProxy overrides:
    virtual void reportException(const String& errorMessage, int lineNumber, int columnNumber, const String& sourceURL, int exceptionId) override;
    virtual void reportConsoleMessage(PassRefPtrWillBeRawPtr<ConsoleMessage>) override;
    virtual void postMessageToPageInspector(const String&) override;
    virtual void postWorkerConsoleAgentEnabled() override { }
    virtual void didEvaluateWorkerScript(bool success) override;
    virtual void workerGlobalScopeStarted(WorkerGlobalScope*) override;
    virtual void workerGlobalScopeClosed() override;
    virtual void willDestroyWorkerGlobalScope() override;
    virtual void workerThreadTerminated() override;

private:
    ServiceWorkerGlobalScopeProxy(WebEmbeddedWorkerImpl&, Document&, WebServiceWorkerContextClient&);

    WebEmbeddedWorkerImpl& m_embeddedWorker;
    Document& m_document;

    WebServiceWorkerContextClient& m_client;

    ServiceWorkerGlobalScope* m_workerGlobalScope;
};

} // namespace blink

#endif // ServiceWorkerGlobalScopeProxy_h
