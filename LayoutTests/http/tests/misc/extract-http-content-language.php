<?php
  header("Content-Language: zh-CN");
?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<script src="../../js-test-resources/js-test.js"></script>
</head>
<body>
<p>Test for <a href="https://bugs.webkit.org/show_bug.cgi?id=97929">bug 97929</a>:
Extract HTTP Content-Language header.</p>
<div id="console"></div>
<div id="x"></div>
<div id="y" lang="ar"></div>
<script>
  debug('==> Value extracted from HTTP "Content-Language" header...');
  shouldBe('window.getComputedStyle(document.getElementById("x")).webkitLocale', '"\'zh-CN\'"')
  debug('==> Value set by div "lang" tag...');
  shouldBe('window.getComputedStyle(document.getElementById("y")).webkitLocale', '"\'ar\'"')
  debug('==> All done...');
</script>
</body>
</html>
