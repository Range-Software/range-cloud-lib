## Version 1.0.0

- `RHttpMessage`: request headers support added (`requestHeaders` field,
  `getRequestHeaders()`, `setRequestHeaders()`)
- `RCloudToolAction`: new `Query` action type with `requestQuery()` and
  `processQueryResponse()`
- `RFileTools` moved to `range-base-lib`

### Bug fixes

- Fixed crash in `RHttpClient` on null pointer
