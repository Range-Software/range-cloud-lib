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
