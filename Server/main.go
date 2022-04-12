package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"log"
	"math/rand"
	"net/http"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/grigorypavlov/M242_LB2/Server/api"
	"github.com/tidwall/buntdb"
)

type Client struct {
	name   string
	events chan *DashBoard
}

type DashBoard struct {
	User uint
}

func sseHandler(w http.ResponseWriter, r *http.Request) {
	client := &Client{name: r.RemoteAddr, events: make(chan *DashBoard, 10)}
	go updateSSE(client)

	w.Header().Set("Access-Control-Allow-Origin", "*")
	w.Header().Set("Access-Control-Allow-Headers", "Content-Type")
	w.Header().Set("Content-Type", "text/event-stream")
	w.Header().Set("Cache-Control", "no-cache")
	w.Header().Set("Connection", "keep-alive")

	timeout := time.After(1 * time.Second)
	select {
	case ev := <-client.events:
		var buf bytes.Buffer
		enc := json.NewEncoder(&buf)
		enc.Encode(ev)
		fmt.Fprintf(w, "data: %v\n\n", buf.String())
		fmt.Printf("data: %v\n", buf.String())
	case <-timeout:
		fmt.Fprintf(w, ": nothing to send\n\n")
	}

	if f, ok := w.(http.Flusher); ok {
		f.Flush()
	}
}

func updateSSE(client *Client) {
	for {
		db := &DashBoard{
			User: uint(rand.Uint32()),
		}
		client.events <- db
	}
}

func main() {
	db, err := buntdb.Open(":memory:")
	if err != nil {
		log.Fatalf("Failed opening database: %s", err)
	}
	srv := api.New(db)

	/*broker := "192.168.1.137"
	port := 1883
	opts := mqtt.NewClientOptions()
	opts.AddBroker(fmt.Sprintf("tcp://%s:%d", broker, port))
	opts.SetClientID("GoServer")
	opts.SetUsername("mostest")
	opts.SetPassword("supersecretpassword")

	opts.OnConnect = func(c mqtt.Client) {
		fmt.Println("Broker Connected")
	}

	opts.OnConnectionLost = func(c mqtt.Client, err error) {
		fmt.Printf("Connection lost: %v", err)
	}

	client := mqtt.NewClient(opts)
	if token := client.Connect(); token.Wait() && token.Error() != nil {
		panic(token.Error())
	}

	if token := client.Subscribe("LB3/Temperature", 1, handleMessage); token.Wait() && token.Error() != nil {
		log.Fatalf("Failed Subscribing to Topic: %v", token.Error())
		panic(token.Error())
	}

	if token := client.Publish("LB3/Temperature", 1, true, `{"temp": 28.5, "hum": 30.8}`); token.Wait() && token.Error() != nil {
		log.Fatalln("Failed to Publish to Topic")
	}*/

	mux := http.NewServeMux()
	/*mux.Handle(
	"/api/v1/hello_world",
	loggingMiddleware(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		err := srv.PostUpdateReport(w, r)
		if err != nil {
			log.Printf("Error handling request: %v, %v\n", r, err)
		}
	})))*/

	mux.Handle(
		"/api/v1/data",
		loggingMiddleware(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			err := srv.GetDataUpdate(w, r)
			if err != nil {
				log.Printf("Error handling request: %v, %v\n", r, err)
			}
		})))

	mux.Handle("/sse", loggingMiddleware(http.HandlerFunc(sseHandler)))

	fmt.Println("Listening on :8080 ...")
	log.Fatal(http.ListenAndServe(":8080", mux))
}

func loggingMiddleware(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		log.Printf("Request received: %v\n", r)
		next.ServeHTTP(w, r)
	})
}

func handleMessage(client mqtt.Client, msg mqtt.Message) {
	fmt.Printf("Client: %v Received Message: %v\n", client, msg)
}
