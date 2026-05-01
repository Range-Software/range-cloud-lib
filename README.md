# Range Cloud Library

Qt-based C++ library providing cloud connectivity, file synchronisation, user/group management, and HTTP networking for Range applications.

## What it provides

**Cloud client** — `RCloudClient` is the main entry point for all cloud operations: file transfer, user and group administration, remote process submission, and general queries. `RCloudSessionManager` manages named connectivity profiles.

**File management** — `RFileManager` synchronises local and remote files with a persistent cache (`RFileManagerCache`). `RSoftwareManager` handles software-update downloads.

**HTTP layer** — `RHttpClient` and `RHttpServer` provide full HTTP/HTTPS with SSL/TLS, proxy support, token-based authentication, and request routing. `RHttpMessage` encapsulates request and response data.

**Security & auth** — `RAuthToken`/`RAuthTokenValidator` handle session tokens. `RTlsTrustStore`, `RTlsKeyStore`, and `ROpenSslTool` cover certificate management, CSR generation, and OpenSSL operations.

**Data models** — `RFileInfo`, `RUserInfo`, `RGroupInfo`, `RCloudProcessInfo`, `RCloudActionInfo`, `RAccessRights` and related types carry metadata for all cloud entities.

## Class prefix

All public classes use the `R` prefix (e.g. `RCloudClient`, `RHttpClient`).

## Dependencies

- Qt 6.10+
- OpenSSL
- range-base-lib
- C++17
