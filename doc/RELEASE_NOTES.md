## Version 1.0.3

### Improvements

- `RCloudAIQueryRequest` / `RCloudAIQueryResponse`: new data types carrying an AI
  query (application, model, query text) and its response
- `RCloudAction`: new `ai-query` action
- `RCloudToolAction`: new `AIQuery` action type with `requestAIQuery()` and
  `processAIQueryResponse()`

### Removals

- Removed the incomplete `query` action, superseded by `ai-query`. It was never
  handled server-side and always resolved as an unknown action. This drops
  `RCloudAction::Action::Query`, `RCloudToolAction::Query` with `requestQuery()`
  and `processQueryResponse()`, and `RCloudClient::requestQuery()` with the
  `queryResultAvailable()` signal

### Bug fixes

- Fixed double-delete and lost-wakeup races in HTTP server handlers

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
