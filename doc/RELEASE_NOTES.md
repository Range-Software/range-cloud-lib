## Version 1.0.3

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
