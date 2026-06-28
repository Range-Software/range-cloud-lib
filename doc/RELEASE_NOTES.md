## Version 1.0.2

### Improvements

- Added max body size

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
