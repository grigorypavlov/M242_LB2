# Cross Origin Resource Sharing

Things like `iframe`s make it possible for one website to access another
website. To protect this feature from being misused for evil purposes, CORS was
invented.

## Problem

<sample>
Cross-Origin Request Blocked: The Same Origin Policy disallows reading the
remote resource at http://192.168.104.29:8080/api/v1/data?sensor=hum_temp.
(Reason: CORS request did not succeed). Status code: (null).
</sample>

CORS won't work when testing from `file://`.

## Solution

Run a simple http server:

```sh
python3 -m http.server
```

On the backend, allow all cross origin requests:

```go
// Set CORS header
w.Header().Set("Access-Control-Allow-Origin", "*")
```

Or specify allowed domains:

```go
// Set CORS header
w.Header().Set("Access-Control-Allow-Origin", "https://example.com")
```
