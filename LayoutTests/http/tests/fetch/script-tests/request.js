if (self.importScripts) {
  importScripts('../resources/fetch-test-helpers.js');
}

var URL = 'https://www.example.com/test.html';

test(function() {
    var headers = new Headers;
    headers.set('User-Agent', 'Mozilla/5.0');
    headers.set('Accept', 'text/html');
    headers.set('X-Fetch-Test', 'request test field');

    var request = new Request(URL, {method: 'GET', headers: headers});

    assert_equals(request.url, URL, 'Request.url should match');
    assert_equals(request.method, 'GET', 'Request.method should match');
    assert_equals(request.referrer, 'about:client',
                  'Request.referrer should be about:client');
    assert_true(request.headers instanceof Headers,
                'Request.headers should be Headers');

    // 'User-Agent' is a forbidden header.
    assert_equals(size(request.headers), 2,
                  'Request.headers size should match');
    // Note: detailed behavioral tests for Headers are in another test,
    // http/tests/fetch/*/headers.html.

    request.url = 'http://localhost/';
    assert_equals(request.url, 'https://www.example.com/test.html',
                  'Request.url should be readonly');

    // Unmatched lead surrogate.
    request = new Request('http://localhost/\uD800');
    assert_equals(request.url,
                  'http://localhost/' + encodeURIComponent('\uFFFD'),
                  'Request.url should have unmatched surrogates replaced.');

    request.method = 'POST';
    assert_equals(request.method, 'GET', 'Request.method should be readonly');
  }, 'Request basic test');

test(function() {
    [new Request(URL),
     // All mode/credentials below are invalid and thus ignored.
     new Request(URL, {mode: null}),
     new Request(URL, {mode: undefined}),
     new Request(URL, {mode: 'sameorigin'}),
     new Request(URL, {mode: 'same origin'}),
     new Request(URL, {mode: 'same-origin\0'}),
     new Request(URL, {mode: ' same-origin'}),
     new Request(URL, {mode: 'same--origin'}),
     new Request(URL, {mode: 'SAME-ORIGIN'}),
     new Request(URL, {mode: 'nocors'}),
     new Request(URL, {mode: 'no cors'}),
     new Request(URL, {mode: 'no-cors\0'}),
     new Request(URL, {mode: ' no-cors'}),
     new Request(URL, {mode: 'no--cors'}),
     new Request(URL, {mode: 'NO-CORS'}),
     new Request(URL, {mode: 'cors\0'}),
     new Request(URL, {mode: ' cors'}),
     new Request(URL, {mode: 'co rs'}),
     new Request(URL, {mode: 'CORS'}),
     new Request(URL, {mode: '\0'.repeat(100000)}),
     new Request(URL, {mode: 'x'.repeat(100000)}),
     new Request(URL, {credentials: null}),
     new Request(URL, {credentials: undefined}),
     new Request(URL, {credentials: 'omit\0'}),
     new Request(URL, {credentials: ' omit'}),
     new Request(URL, {credentials: 'om it'}),
     new Request(URL, {credentials: 'OMIT'}),
     new Request(URL, {credentials: 'sameorigin'}),
     new Request(URL, {credentials: 'same origin'}),
     new Request(URL, {credentials: 'same-origin\0'}),
     new Request(URL, {credentials: ' same-origin'}),
     new Request(URL, {credentials: 'same--origin'}),
     new Request(URL, {credentials: 'SAME-ORIGIN'}),
     new Request(URL, {credentials: 'include\0'}),
     new Request(URL, {credentials: ' include'}),
     new Request(URL, {credentials: 'inc lude'}),
     new Request(URL, {credentials: 'INCLUDE'}),
     new Request(URL, {credentials: '\0'.repeat(100000)}),
     new Request(URL, {credentials: 'x'.repeat(100000)})]
      .concat(INVALID_TOKENS.map(
          function(name) { return new Request(URL, {mode: name}); }))
      .concat(INVALID_TOKENS.map(
          function(name) { return new Request(URL, {credentials: name}); }))
      .forEach(function(request) {
          assert_equals(request.url, URL,
                        'Request.url should match');
          assert_equals(request.method, 'GET',
                        'Default Request.method should be GET');
          assert_equals(request.mode, 'cors',
                        'Default Request.mode should be cors');
          assert_equals(request.credentials, 'omit',
                        'Default Request.credentials should be omit');
        });
  }, 'Request default value test');

test(function() {
    var request = new Request(URL);
    request.headers.append('X-Fetch-Foo', 'foo1');
    request.headers.append('X-Fetch-Foo', 'foo2');
    request.headers.append('X-Fetch-Bar', 'bar');
    var request2 = new Request(request);
    assert_equals(request2.url, URL, 'Request.url should match');
    assert_equals(request2.method, 'GET', 'Request.method should match');
    assert_equals(request2.mode, 'cors', 'Request.mode should match');
    assert_equals(request2.credentials, 'omit',
                  'Request.credentials should match');
    assert_equals(request2.headers.getAll('X-Fetch-Foo')[0], 'foo1',
                  'Request.headers should match');
    assert_equals(request2.headers.getAll('X-Fetch-Foo')[1], 'foo2',
                  'Request.headers should match');
    assert_equals(request2.headers.getAll('X-Fetch-Bar')[0], 'bar',
                  'Request.headers should match');
    var request3 = new Request(URL,
                               {headers: [['X-Fetch-Foo', 'foo1'],
                                          ['X-Fetch-Foo', 'foo2'],
                                          ['X-Fetch-Bar', 'bar']]});
    assert_equals(request3.headers.getAll('X-Fetch-Foo')[0], 'foo1',
                  'Request.headers should match');
    assert_equals(request3.headers.getAll('X-Fetch-Foo')[1], 'foo2',
                  'Request.headers should match');
    assert_equals(request3.headers.getAll('X-Fetch-Bar')[0], 'bar',
                  'Request.headers should match');
  }, 'Request header test');

test(function() {
    var request1 = {};
    var request2 = {};
    var METHODS = ['GET', 'HEAD', 'POST', 'PUT', 'DELETE', 'OPTIONS',
                   undefined];
    var MODES = ['same-origin', 'no-cors', 'cors', '', undefined];
    function isSimpleMethod(method) {
      return ['GET', 'HEAD', 'POST', undefined].indexOf(method) != -1;
    };
    function effectiveMethod(method1, method2) {
      return method2 ? method2 : (method1 ? method1 : 'GET');
    };
    function effectiveMode(mode1, mode2) {
      return mode2 ? mode2 : (mode1 ? mode1 : 'cors');
    };
    METHODS.forEach(function(method1) {
        MODES.forEach(function(mode1) {
            var init1 = {};
            if (method1 != undefined) { init1['method'] = method1; }
            if (mode1 != undefined) { init1['mode'] = mode1; }
            if (!isSimpleMethod(method1) && mode1 == 'no-cors') {
              assert_throws(
                {name: 'TypeError'},
                function() { request1 = new Request(URL, init1); },
                'new no-cors Request with non simple method (' + method1 +
                ') should throw');
              return;
            }
            request1 = new Request(URL, init1);
            assert_equals(request1.method, method1 ? method1 : 'GET',
                          'Request.method should match');
            assert_equals(request1.mode, mode1 ? mode1 : 'cors',
                          'Request.mode should match');
            request1 = new Request(request1);
            assert_equals(request1.method, method1 ? method1 : 'GET',
                          'Request.method should match');
            assert_equals(request1.mode, mode1 ? mode1 : 'cors',
                          'Request.mode should match');
            METHODS.forEach(function(method2) {
                MODES.forEach(function(mode2) {
                    // We need to construct a new request1 because as soon as it
                    // is used in a constructor it will be flagged as 'used',
                    // and we can no longer construct objects with it.
                    request1 = new Request(URL, init1);
                    var init2 = {};
                    if (method2 != undefined) { init2['method'] = method2; }
                    if (mode2 != undefined) { init2['mode'] = mode2; }
                    if (!isSimpleMethod(effectiveMethod(method1, method2)) &&
                        effectiveMode(mode1, mode2) == 'no-cors') {
                      assert_throws(
                        {name: 'TypeError'},
                        function() { request2 = new Request(request1, init2); },
                        'new no-cors Request with non simple method should ' +
                        'throw');
                      return;
                    }
                    request2 = new Request(request1, init2);
                    assert_equals(request2.method,
                                  method2 ? method2 : request1.method,
                                  'Request.method should be overridden');
                    assert_equals(request2.mode,
                                  mode2 ? mode2 : request1.mode,
                                  'Request.mode should be overridden');
                  });
              });
          });
      });
  }, 'Request header test');

test(function() {
    var request1 = {};
    var request2 = {};
    var CREDENTIALS = ['omit', 'same-origin', 'include', '', undefined];
    CREDENTIALS.forEach(function(credentials1) {
        var init1 = {};
        if (credentials1 != undefined) { init1['credentials'] = credentials1; }
        request1 = new Request(URL, init1);
        assert_equals(request1.credentials, credentials1 || 'omit',
                      'Request.credentials should match');
        request1 = new Request(request1);
        assert_equals(request1.credentials, credentials1 || 'omit',
                      'Request.credentials should match');
        CREDENTIALS.forEach(function(credentials2) {
            request1 = new Request(URL, init1);
            var init2 = {};
            if (credentials2 != undefined) {
              init2['credentials'] = credentials2;
            }
            request2 = new Request(request1, init2);
            assert_equals(request2.credentials,
                          credentials2 ? credentials2 : request1.credentials,
                          'Request.credentials should be overridden');
          });
      });
  }, 'Request credentials test');

test(function() {
    ['same-origin', 'cors', 'no-cors'].forEach(function(mode) {
        FORBIDDEN_METHODS.forEach(function(method) {
            assert_throws(
              {name: 'TypeError'},
              function() {
                var request = new Request(URL, {mode: mode, method: method});
              },
              'new Request with a forbidden method (' + method + ') should ' +
              'throw');
          });
        INVALID_METHOD_NAMES.forEach(function(name) {
            assert_throws(
              {name: 'TypeError'},
              function() {
                var request = new Request(URL, {mode: mode, method: name});
              },
              'new Request with an invalid method (' + name + ') should throw');
          });
      });
  }, 'Request method name throw test');

test(function() {
    var url = 'http://example.com';
    TO_BE_NORMALIZED_METHOD_NAMES.forEach(
      function(method) {
        assert_equals(new Request(url, {method: method.toUpperCase()}).method,
                      method.toUpperCase(),
                      'method must match: ' + method);
        assert_equals(new Request(url, {method: method}).method,
                      method.toUpperCase(),
                      'method should be normalized to uppercase: ' + method);
      });

    OTHER_VALID_METHOD_NAMES.forEach(
      function(method) {
        assert_equals(new Request(url, {method: method}).method, method,
                      'method should not be changed when normalized: ' +
                      method);
        method = method.toLowerCase();
        assert_equals(new Request(url, {method: method}).method, method,
                      'method should not be changed when normalized: ' +
                      method);
      });
  }, 'Request: valid method names and normalize test');

test(function() {
    var req = new Request(URL);
    assert_false(req.bodyUsed,
                 'Request should not be flagged as used if it has not been ' +
                 'consumed.');
    var req2 = new Request(req);
    assert_true(req.bodyUsed,
                'Request should be flagged as used if it is used as a ' +
                'construction argument of another Request.');
    assert_false(req2.bodyUsed,
                 'Request should not be flagged as used if it has not been ' +
                 'consumed.');
    assert_throws(new TypeError(), function() { new Request(req); },
                  'Request cannot be constructed with a request that has ' +
                  'been flagged as used.');
    assert_throws(new TypeError(), function() { req.clone(); },
                  'Request.clone: If used flag is set, throw a TypeError.');
  },
  'Request construction behavior regarding "used" body flag and exceptions.');


// Spec: https://fetch.spec.whatwg.org/#dom-request
// Step 21:
// If request's method is `GET` or `HEAD`, throw a TypeError.
promise_test(function() {
    var headers = new Headers;
    headers.set('Content-Language', 'ja');
    ['GET', 'HEAD'].forEach(function(method) {
        assert_throws(
          {name: 'TypeError'},
          function() {
            new Request(URL,
                        {method: method,
                         body: new Blob(['Test Blob'], {type: 'test/type'})
                        });
          },
          'Request of GET/HEAD method cannot have RequestInit body.');
      });
  }, 'Request of GET/HEAD method cannot have RequestInit body.');

promise_test(function() {
    var headers = new Headers;
    headers.set('Content-Language', 'ja');
    var req = new Request(URL, {
        method: 'POST',
        headers: headers,
        body: new Blob(['Test Blob'], {type: 'test/type'})
      });
    var req2 = req.clone();
    // Change headers and of original request.
    req.headers.set('Content-Language', 'en');
    assert_equals(req2.headers.get('Content-Language'), 'ja',
                  'Headers of cloned request should not change when ' +
                  'original request headers are changed.');

    return req.text().then(function(text) {
        assert_equals(text, 'Test Blob', 'Body of request should match.');
        return req2.text();
      }).then(function(text) {
        assert_equals(text, 'Test Blob', 'Cloned request body should match.');
        return Promise.all([req.text(), req2.text()]);
      }).then(function(texts) {
        assert_equals(texts[0], '', 'The body is consumed.');
        assert_equals(texts[1], '', 'The body is consumed.');
        return req.clone().text();
      }).then(function(text) {
        assert_equals(text, '', 'The body was consumed before cloned.');
      });
  }, 'Test clone behavior with loading content from Request.');

async_test(function(t) {
    var request =
      new Request(URL,
                  {
                    method: 'POST',
                    body: new Blob(['Test Blob'], {type: 'test/type'})
                  });
    assert_equals(
      getContentType(request.headers), 'test/type',
      'ContentType header of Request created with Blob body must be set.');
    assert_false(request.bodyUsed,
                 'bodyUsed must be true before calling text()');
    request.text()
      .then(function(result) {
          assert_equals(result, 'Test Blob',
                        'Creating a Request with Blob body should success.');

          request = new Request(URL, {method: 'POST', body: 'Test String'});
          assert_equals(
            getContentType(request.headers), 'text/plain;charset=UTF-8',
            'ContentType header of Request created with string must be set.');
          return request.text();
        })
      .then(function(result) {
          assert_equals(result, 'Test String',
                        'Creating a Request with string body should success.');

          var text = 'Test ArrayBuffer';
          var array = new Uint8Array(text.length);
          for (var i = 0; i < text.length; ++i)
            array[i] = text.charCodeAt(i);
          request = new Request(URL, {method: 'POST', body: array.buffer});
          return request.text();
        })
      .then(function(result) {
          assert_equals(
            result, 'Test ArrayBuffer',
            'Creating a Request with ArrayBuffer body should success.');

          var text = 'Test ArrayBufferView';
          var array = new Uint8Array(text.length);
          for (var i = 0; i < text.length; ++i)
            array[i] = text.charCodeAt(i);
          request = new Request(URL, {method: 'POST', body: array});
          return request.text();
        })
      .then(function(result) {
          assert_equals(
            result, 'Test ArrayBufferView',
            'Creating a Request with ArrayBuffer body should success.');

          var formData = new FormData();
          formData.append('sample string', '1234567890');
          formData.append('sample blob', new Blob(['blob content']));
          formData.append('sample file',
                          new File(['file content'], 'file.dat'));
          request = new Request(URL, {method: 'POST', body: formData});
          return request.text();
        })
      .then(function(result) {
          var reg = new RegExp('multipart\/form-data; boundary=(.*)');
          var regResult = reg.exec(getContentType(request.headers));
          var boundary = regResult[1];
          var expected_body =
            '--' + boundary + '\r\n' +
            'Content-Disposition: form-data; name="sample string"\r\n' +
            '\r\n' +
            '1234567890\r\n' +
            '--' + boundary + '\r\n' +
            'Content-Disposition: form-data; name="sample blob"; ' +
            'filename="blob"\r\n' +
            'Content-Type: application/octet-stream\r\n' +
            '\r\n' +
            'blob content\r\n' +
            '--' + boundary + '\r\n' +
            'Content-Disposition: form-data; name="sample file"; ' +
            'filename="file.dat"\r\n' +
            'Content-Type: application/octet-stream\r\n' +
            '\r\n' +
            'file content\r\n' +
            '--' + boundary + '--\r\n';
          assert_equals(
            result, expected_body,
            'Creating a Request with FormData body should success.');
        })
      .then(function() {
          t.done();
        })
      .catch(unreached_rejection(t));
    assert_true(request.bodyUsed,
                'bodyUsed must be true after calling text()');
  }, 'Request body test');

test(function() {
    // https://fetch.spec.whatwg.org/#dom-request
    // Step 20:
    // Fill r's Headers object with headers. Rethrow any exceptions.
    INVALID_HEADER_NAMES.forEach(function(name) {
        assert_throws(
          {name: 'TypeError'},
          function() {
            var obj = {};
            obj[name] = 'a';
            new Request('http://localhost/', {headers: obj});
          },
          'new Request with headers with an invalid name (' + name +
          ') should throw');
        assert_throws(
          {name: 'TypeError'},
          function() {
            new Request('http://localhost/', {headers: [[name, 'a']]});
          },
          'new Request with headers with an invalid name (' + name +
          ') should throw');
      });

    INVALID_HEADER_VALUES.forEach(function(value) {
        assert_throws(
          {name: 'TypeError'},
          function() {
            new Request('http://localhost/',
                         {headers: {'X-Fetch-Test': value}});
          },
          'new Request with headers with an invalid value should throw');
        assert_throws(
          {name: 'TypeError'},
          function() {
            new Request('http://localhost/',
                         {headers: [['X-Fetch-Test', value]]});
          },
          'new Request with headers with an invalid value should throw');
      });
  }, 'Request throw error test');

// Tests for MIME types.
promise_test(function(t) {
    var req = new Request('http://localhost/',
                          {method: 'POST', body: new Blob([''])});
    return req.blob()
      .then(function(blob) {
          assert_equals(blob.type, '');
          assert_equals(req.headers.get('Content-Type'), null);
        });
  }, 'MIME type for Blob');

promise_test(function(t) {
    var req = new Request('http://localhost/',
                          {method: 'POST',
                           body: new Blob([''], {type: 'Text/Plain'})});
    return req.blob()
      .then(function(blob) {
          assert_equals(blob.type, 'text/plain');
          assert_equals(req.headers.get('Content-Type'), 'text/plain');
        });
  }, 'MIME type for Blob with non-empty type');

promise_test(function(t) {
    var req = new Request('http://localhost/',
                          {method: 'POST', body: new FormData()});
    return req.blob()
      .then(function(blob) {
          assert_equals(blob.type.indexOf('multipart/form-data; boundary='),
                        0);
          assert_equals(req.headers.get('Content-Type')
                          .indexOf('multipart/form-data; boundary='),
                        0);
        });
  }, 'MIME type for FormData');

promise_test(function(t) {
    var req = new Request('http://localhost/',
                          {method: 'POST', body: ''});
    return req.blob()
      .then(function(blob) {
          assert_equals(blob.type, 'text/plain;charset=utf-8');
          assert_equals(req.headers.get('Content-Type'),
                        'text/plain;charset=UTF-8');
        });
  }, 'MIME type for USVString');

promise_test(function(t) {
    var req = new Request('http://localhost/',
                          {method: 'POST',
                           body: new Blob([''], {type: 'Text/Plain'}),
                           headers: [['Content-Type', 'Text/Html']]});
    var clone = req.clone();
    return Promise.all([req.blob(), clone.blob()])
      .then(function(blobs) {
          assert_equals(blobs[0].type, 'text/html');
          assert_equals(blobs[1].type, 'text/html');
          assert_equals(req.headers.get('Content-Type'), 'Text/Html');
          assert_equals(clone.headers.get('Content-Type'), 'Text/Html');
        });
  }, 'Extract a MIME type with clone');

promise_test(function(t) {
    var req = new Request('http://localhost/',
                          {method: 'POST',
                           body: new Blob([''], {type: 'Text/Plain'})});
    req.headers.set('Content-Type', 'Text/Html');
    return req.blob()
      .then(function(blob) {
          assert_equals(blob.type, 'text/plain');
          assert_equals(req.headers.get('Content-Type'), 'Text/Html');
        });
  },
  'MIME type unchanged if headers are modified after Request() constructor');

promise_test(function(t) {
    var req = new Request('http://localhost/',
                          {method: 'POST',
                           body: new Blob([''], {type: 'Text/Plain'}),
                           headers: [['Content-Type', 'Text/Html']]});
    return req.blob()
      .then(function(blob) {
          assert_equals(blob.type, 'text/html');
          assert_equals(req.headers.get('Content-Type'), 'Text/Html');
        });
  }, 'Extract a MIME type (1)');

promise_test(function(t) {
    var req = new Request('http://localhost/', {method: 'POST', body: 'hello'});
    return req.text().then(function(text) {
        assert_equals(text, 'hello');
        var req2 = new Request(req);
        assert_true(req.bodyUsed);
        assert_false(req2.bodyUsed);
        return req2.text();
      }).then(function(text) {
        assert_equals(text, '');
      });
  }, 'Consume and pass');

done();
