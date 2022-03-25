# Web Service

![Class Diagram](class_diagram.svg)

`Server` provides an endpoint for the IoTKit to send data to
(`PostUpdateReport`), and an endpoint for the client web interface to read data
from (`GetDataUpdate`).

The IoTKit sends `Request`s to the server, where `Sensor` describes the sensor
the data was measured with, and `Data` is the measured data json-encoded.

`ErrorResponse` is returned to the caller in case the request was malformed, or
another error occured.

## Usage

In `main.go`, use the `Server` like this, to configure all necessary endpoints:

```go
func main() {
	db, _ := buntdb.Open(":memory:")
	srv := api.New(db)

	mux := http.NewServeMux()
	mux.Handle(
		"/api/v1/hello_world",
		http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			err := srv.PostUpdateReport(w, r)
			if err != nil {
				log.Printf("Error handling request: %v, %v\n", r, err)
			}
		}))

	mux.Handle(
		"/api/v1/data",
		http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			err := srv.GetDataUpdate(w, r)
			if err != nil {
				log.Printf("Error handling request: %v, %v\n", r, err)
			}
		}))

	fmt.Println("Listening on :8080 ...")
	log.Fatal(http.ListenAndServe(":8080", mux))
}
```

Request handlers can be chained together, for example, to log requests:

```go
func loggingMiddleware(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		log.Printf("Request received: %v\n", r)
		next.ServeHTTP(w, r)
	})
}

mux.Handle(
    "/api/v1/hello_world",
    loggingMiddleware(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
        err := srv.PostUpdateReport(w, r)
        if err != nil {
            log.Printf("Error handling request: %v, %v\n", r, err)
        }
    })))
```
