package main

import (
	"log"
	"net/http"

	"framagit.org/edu-cvl/M242-LB/API/api"
)

func main() {
	srv := api.Default()

	mux := http.NewServeMux()
	mux.Handle(
		"/api/v1/hello_world",
		loggingMiddleware(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			err := srv.PostData(w, r)
			if err != nil {
				log.Printf("Error handling request: %v, %v\n", r, err)
			}
		})))

	log.Fatal(http.ListenAndServe(":8080", mux))
}

func loggingMiddleware(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		log.Printf("Request received: %v\n", r)
		next.ServeHTTP(w, r)
	})
}
