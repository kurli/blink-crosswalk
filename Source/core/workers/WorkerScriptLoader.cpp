/*
 * Copyright (C) 2009 Apple Inc. All Rights Reserved.
 * Copyright (C) 2009, 2011 Google Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
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
 *
 */

#include "config.h"
#include "core/workers/WorkerScriptLoader.h"

#include "core/dom/ExecutionContext.h"
#include "core/html/parser/TextResourceDecoder.h"
#include "core/loader/WorkerThreadableLoader.h"
#include "core/workers/WorkerGlobalScope.h"
#include "core/workers/WorkerScriptLoaderClient.h"
#include "platform/network/ResourceResponse.h"
#include "public/platform/WebURLRequest.h"

#include "wtf/OwnPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

WorkerScriptLoader::WorkerScriptLoader()
    : m_client(nullptr)
    , m_failed(false)
    , m_identifier(0)
    , m_finishing(false)
    , m_requestContext(blink::WebURLRequest::RequestContextWorker)
{
}

WorkerScriptLoader::~WorkerScriptLoader()
{
}

void WorkerScriptLoader::loadSynchronously(ExecutionContext& executionContext, const KURL& url, CrossOriginRequestPolicy crossOriginRequestPolicy)
{
    m_url = url;

    OwnPtr<ResourceRequest> request(createResourceRequest());
    if (!request)
        return;

    ASSERT_WITH_SECURITY_IMPLICATION(executionContext.isWorkerGlobalScope());

    ThreadableLoaderOptions options;
    options.crossOriginRequestPolicy = crossOriginRequestPolicy;
    // FIXME: Should we add EnforceScriptSrcDirective here?
    options.contentSecurityPolicyEnforcement = DoNotEnforceContentSecurityPolicy;

    ResourceLoaderOptions resourceLoaderOptions;
    resourceLoaderOptions.allowCredentials = AllowStoredCredentials;

    WorkerThreadableLoader::loadResourceSynchronously(toWorkerGlobalScope(executionContext), *request, *this, options, resourceLoaderOptions);
}

void WorkerScriptLoader::loadAsynchronously(ExecutionContext& executionContext, const KURL& url, CrossOriginRequestPolicy crossOriginRequestPolicy, WorkerScriptLoaderClient* client)
{
    ASSERT(client);
    m_client = client;
    m_url = url;

    OwnPtr<ResourceRequest> request(createResourceRequest());
    if (!request)
        return;

    ThreadableLoaderOptions options;
    options.crossOriginRequestPolicy = crossOriginRequestPolicy;

    ResourceLoaderOptions resourceLoaderOptions;
    resourceLoaderOptions.allowCredentials = AllowStoredCredentials;

    // During create, callbacks may happen which remove the last reference to this object.
    RefPtr<WorkerScriptLoader> protect(this);
    m_threadableLoader = ThreadableLoader::create(executionContext, this, *request, options, resourceLoaderOptions);
}

const KURL& WorkerScriptLoader::responseURL() const
{
    ASSERT(!failed());
    return m_responseURL;
}

PassOwnPtr<ResourceRequest> WorkerScriptLoader::createResourceRequest()
{
    OwnPtr<ResourceRequest> request = adoptPtr(new ResourceRequest(m_url));
    request->setHTTPMethod("GET");
    request->setRequestContext(m_requestContext);
    return request.release();
}

void WorkerScriptLoader::didReceiveResponse(unsigned long identifier, const ResourceResponse& response, PassOwnPtr<WebDataConsumerHandle> handle)
{
    ASSERT_UNUSED(handle, !handle);
    if (response.httpStatusCode() / 100 != 2 && response.httpStatusCode()) {
        m_failed = true;
        return;
    }
    m_responseURL = response.url();
    m_responseEncoding = response.textEncodingName();
    if (m_client)
        m_client->didReceiveResponse(identifier, response);
}

void WorkerScriptLoader::didReceiveData(const char* data, unsigned len)
{
    if (m_failed)
        return;

    if (!m_decoder) {
        if (!m_responseEncoding.isEmpty())
            m_decoder = TextResourceDecoder::create("text/javascript", m_responseEncoding);
        else
            m_decoder = TextResourceDecoder::create("text/javascript", "UTF-8");
    }

    if (!len)
        return;

    m_script.append(m_decoder->decode(data, len));
}

void WorkerScriptLoader::didReceiveCachedMetadata(const char* data, int size)
{
    m_cachedMetadata = adoptPtr(new Vector<char>(size));
    memcpy(m_cachedMetadata->data(), data, size);
}

void WorkerScriptLoader::didFinishLoading(unsigned long identifier, double)
{
    if (m_failed) {
        notifyError();
        return;
    }

    if (m_decoder)
        m_script.append(m_decoder->flush());

    m_identifier = identifier;
    notifyFinished();
}

void WorkerScriptLoader::didFail(const ResourceError&)
{
    notifyError();
}

void WorkerScriptLoader::didFailRedirectCheck()
{
    notifyError();
}

void WorkerScriptLoader::notifyError()
{
    m_failed = true;
    notifyFinished();
}

void WorkerScriptLoader::cancel()
{
    if (m_threadableLoader)
        m_threadableLoader->cancel();
}

String WorkerScriptLoader::script()
{
    return m_script.toString();
}

void WorkerScriptLoader::notifyFinished()
{
    if (!m_client || m_finishing)
        return;

    m_finishing = true;
    m_client->notifyFinished();
}

} // namespace blink
