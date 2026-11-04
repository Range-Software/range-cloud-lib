## Version 1.3.0

### Improvements

#### Native key and CSR generation on macOS

- `RSecurityKeyTool` generates private keys and certificate signing requests with
  the Apple Security framework instead of the `openssl` tool. RSA 4096 keys are
  created with `SecKeyCreateRandomKey` and stored as unencrypted PKCS#8 PEM files,
  the same format `openssl genpkey` writes, readable only by the owner. CSRs are
  signed with `SecKeyCreateSignature` (sha256WithRSAEncryption) and are byte for
  byte identical to those of `openssl req` for the same key and subject. Keys
  generated earlier by `openssl` (PKCS#8 or PKCS#1 PEM) can be used as well
- `RSecurityKeyTool` also writes the text form of a private key, identical to
  `openssl pkey -text -noout`, and a text summary of certificates built from
  `QSslCertificate`, which works with every Qt TLS backend
- `ROpenSslTool` passes key generation, CSR generation and the text output to
  `RSecurityKeyTool` when built with the `RANGE_NATIVE_KEY_TOOL` CMake option,
  which is on by default and only applies to macOS. The openssl configuration
  file is not needed in that case. `ROpenSslTool::isToolRequired()` tells whether
  the `openssl` tool is used
- `RPkcs`: DER and PEM encoding of RSA keys (PKCS#1, PKCS#8) and certificate
  signing requests (PKCS#10), independent of the cryptographic library
- Unit test `tst_security_key_tool` checks the generated files against Qt and the
  `openssl` tool

#### Host certificate verification

- `RHttpClientSettings::Verification` selects how the host certificate is verified:
  `FullVerification` (certificate chain and host name, the default),
  `CertificateVerification` (certificate chain only, host name is not checked) and
  `NoVerification`. It is set with `RHttpClientSettings::setVerification()` and
  stored with each Cloud session (`RCloudSessionInfo::setVerification()`) as
  `"verification": "full" | "certificate" | "none"` in the `host` object. Sessions
  saved without it use full verification
- `CertificateVerification` suits a server reached by its IP address whose
  certificate is issued by a private certificate authority set as the session's
  host certificate (`RTlsTrustStore`)
- `RHttpClient` verifies the peer (`QSslSocket::VerifyPeer`). SSL errors which the
  selected verification does not ignore abort the TLS handshake before any request
  data is sent, and are logged as errors. Ignored errors are logged as warnings

### Bug fixes

- Subject fields with characters outside ASCII, such as `Žilina`, were stored
  in the CSR in a mangled form (`Å½ilina`), as `openssl req` was not given
  `-utf8`. CSRs generated with `RSecurityKeyTool` store them correctly
- `RHttpClient` did not verify the host certificate (`QSslSocket::VerifyNone`).
  A certificate issued by an untrusted authority or for another host name was
  accepted, and the reported SSL errors were only logged. Such connections are now
  refused unless the verification is relaxed

---

## Version 1.2.0

### Improvements

- `RHttpServerSettings`: new `maxConnections` and `maxConnectionsPerHost` settings
  (`getMaxConnections()` / `setMaxConnections()`, `getMaxConnectionsPerHost()` /
  `setMaxConnectionsPerHost()`), defaults 1000 and 20
- `RHttpServer` enforces the connection limits through `QHttpServerConfiguration`
  and logs them on start
- `RHttpServerSettings::validate()` rejects `maxConnectionsPerHost` greater than
  `maxConnections`
- Rate limiting, maximum body size and connection limits are always applied; the
  fallback for Qt older than 6.10 was removed
- Translations to Czech, German, Spanish, French, Italian, Polish and Ukrainian
  added in addition to English and Slovak

---

## Version 1.1.0

### Improvements

- New AI query data types: `RAIQuery` (the user's `question` plus an optional
  referenced Cloud file `fileId` / `fileDescription`, free-form `context` and
  preferred response `language`), wrapped by `RCloudAIQueryRequest` (application,
  model, query) and answered by `RCloudAIQueryResponse` (`id`, `status`, response
  message)
- AI queries are processed asynchronously: the new `ai-query` action is acknowledged
  immediately and the answer is fetched with the new `ai-query-result` action,
  repeated until `status` turns from `pending` to `completed`
- New client/action surface for AI queries: `RCloudAction` gains the `ai-query` and
  `ai-query-result` actions; `RCloudToolAction` the `AIQuery` / `AIQueryResult` types
  (`requestAIQuery()` / `processAIQueryResponse()`, `requestAIQueryResult()` /
  `processAIQueryResult()`); `RCloudClient` the `requestAIQuery()` and
  `requestAIQueryResult()` requests, reporting through the `aiQueryResponseAvailable()`
  and `aiQueryResultAvailable()` signals
- `RHttpClient`: added support for the HTTP `DELETE` method
- `RFileManager`: new `findRemoteFile()` returning the file info of a synchronized
  remote file matching given local file name. File name and ownership are matched
  the same way as when local and remote file lists are compared. Already known
  remote files are searched, no file listing is requested, so a running
  synchronization is not disturbed

### Removals

- Removed the incomplete `query` action, superseded by `ai-query`. It was never
  handled server-side and always resolved as an unknown action. This drops
  `RCloudAction::Action::Query`, `RCloudToolAction::Query` with `requestQuery()`
  and `processQueryResponse()`, and `RCloudClient::requestQuery()` with the
  `queryResultAvailable()` signal

### Bug fixes

- Fixed double-delete and lost-wakeup races in HTTP server handlers
- `ai-query` was sent as HTTP GET, silently dropping the request body; it is now
  sent as POST

---

## Version 1.0.2

### Improvements

- Added max body size

### Bug fixes

- Missing object parrent results in memory leak

---

## Version 1.0.1

### Improvements

- Added unit tests based on QTest framework

---

## Version 1.0.0

### Improvements

- `RHttpMessage`: request headers support added (`requestHeaders` field,
  `getRequestHeaders()`, `setRequestHeaders()`)
- `RCloudToolAction`: new `Query` action type with `requestQuery()` and
  `processQueryResponse()`
- `RFileTools` moved to `range-base-lib`

### Bug fixes

- Fixed crash in `RHttpClient` on null pointer
