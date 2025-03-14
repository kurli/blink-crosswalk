if (self.importScripts) {
  importScripts('../resources/fetch-test-helpers.js');
  importScripts('/serviceworker/resources/fetch-access-control-util.js');
}

var TEST_TARGETS = [
  // Redirect: same origin -> same origin
  [REDIRECT_URL + encodeURIComponent(BASE_URL) +
   '&mode=no-cors&method=GET&headers=CUSTOM',
   [fetchResolved, hasContentLength, hasServerHeader, hasBody, typeBasic],
   [methodIsGET, noCustomHeader, authCheck1]],

  // Redirect: same origin -> other origin
  [REDIRECT_URL + encodeURIComponent(OTHER_BASE_URL) +
   '&mode=no-cors&method=GET&headers=CUSTOM',
   [fetchResolved, noContentLength, noServerHeader, noBody, typeOpaque],
   onlyOnServiceWorkerProxiedTest([methodIsGET, noCustomHeader, authCheck2])],

  // Status code tests for mode="no-cors"
  // The 301 redirect response changes POST method to GET method.
  [REDIRECT_URL + encodeURIComponent(OTHER_BASE_URL) +
   '&mode=no-cors&method=POST&Status=301',
   [fetchResolved, noContentLength, noServerHeader, noBody, typeOpaque],
   onlyOnServiceWorkerProxiedTest([methodIsGET, authCheck2])],
  // The 302 redirect response changes POST method to GET method.
  [REDIRECT_URL + encodeURIComponent(OTHER_BASE_URL) +
   '&mode=no-cors&method=POST',
   [fetchResolved, noContentLength, noServerHeader, noBody, typeOpaque],
   onlyOnServiceWorkerProxiedTest([methodIsGET, authCheck2])],
  // GET method must be used for 303 redirect.
  [REDIRECT_URL + encodeURIComponent(OTHER_BASE_URL) +
   '&mode=no-cors&method=POST&Status=303',
   [fetchResolved, noContentLength, noServerHeader, noBody, typeOpaque],
   onlyOnServiceWorkerProxiedTest([methodIsGET, authCheck2])],
  // The 307 redirect response doesn't change the method.
  [REDIRECT_URL + encodeURIComponent(OTHER_BASE_URL) +
   '&mode=no-cors&method=POST&Status=307',
   [fetchResolved, noContentLength, noServerHeader, noBody, typeOpaque],
   onlyOnServiceWorkerProxiedTest([methodIsPOST, authCheck2])],
  // The 308 redirect response doesn't change the method.
  // FIXME: disabled due to https://crbug.com/451938
  // [REDIRECT_URL + encodeURIComponent(OTHER_BASE_URL) +
  // '&mode=no-cors&method=POST&Status=308',
  // [fetchResolved, noContentLength, noServerHeader, noBody, typeOpaque],
  // onlyOnServiceWorkerProxiedTest([methodIsPOST, authCheck2])],

  // Redirect: other origin -> same origin
  [OTHER_REDIRECT_URL + encodeURIComponent(BASE_URL) +
   '&mode=no-cors&method=GET',
   [fetchResolved, noContentLength, noServerHeader, noBody, typeOpaque],
   onlyOnServiceWorkerProxiedTest([methodIsGET, authCheck1])],
  [OTHER_REDIRECT_URL + encodeURIComponent(BASE_URL) +
   '&mode=no-cors&method=GET&headers=CUSTOM',
   [fetchResolved, noContentLength, noServerHeader, noBody, typeOpaque],
   onlyOnServiceWorkerProxiedTest([methodIsGET, noCustomHeader, authCheck1])],

  // Status code tests for mode="no-cors"
  // The 301 redirect response MAY change the request method from POST to GET.
  [OTHER_REDIRECT_URL + encodeURIComponent(BASE_URL) +
   '&mode=no-cors&method=POST&Status=301',
   [fetchResolved, noContentLength, noServerHeader, noBody, typeOpaque],
   onlyOnServiceWorkerProxiedTest([methodIsGET, authCheck1])],
  // The 302 redirect response MAY change the request method from POST to GET.
  [OTHER_REDIRECT_URL + encodeURIComponent(BASE_URL) +
   '&mode=no-cors&method=POST',
   [fetchResolved, noContentLength, noServerHeader, noBody, typeOpaque],
   onlyOnServiceWorkerProxiedTest([methodIsGET, authCheck1])],
  // GET method must be used for 303 redirect.
  [OTHER_REDIRECT_URL + encodeURIComponent(BASE_URL) +
   '&mode=no-cors&method=POST&Status=303',
   [fetchResolved, noContentLength, noServerHeader, noBody, typeOpaque],
   onlyOnServiceWorkerProxiedTest([methodIsGET, authCheck1])],
  // The 307 redirect response MUST NOT change the method.
  [OTHER_REDIRECT_URL + encodeURIComponent(BASE_URL) +
   '&mode=no-cors&method=POST&Status=307',
   [fetchResolved, noContentLength, noServerHeader, noBody, typeOpaque],
   onlyOnServiceWorkerProxiedTest([methodIsPOST, authCheck1])],
  // The 308 redirect response MUST NOT change the method.
  // FIXME: disabled due to https://crbug.com/451938
  // [OTHER_REDIRECT_URL + encodeURIComponent(BASE_URL) +
  //  '&mode=no-cors&method=POST&Status=308',
  //  [fetchResolved, noContentLength, noServerHeader, noBody, typeOpaque],
  //  onlyOnServiceWorkerProxiedTest([methodIsPOST, authCheck1])],

  // Redirect: other origin -> same origin
  [OTHER_REDIRECT_URL + encodeURIComponent(OTHER_BASE_URL) +
   '&mode=no-cors&method=GET',
   [fetchResolved, noContentLength, noServerHeader, noBody, typeOpaque],
   onlyOnServiceWorkerProxiedTest([methodIsGET, authCheck2])],
];

if (self.importScripts) {
  executeTests(TEST_TARGETS);
  done();
}
