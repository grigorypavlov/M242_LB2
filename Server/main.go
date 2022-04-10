package main

import (
	"fmt"
	"log"
	"net/http"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/grigorypavlov/M242_LB2/Server/api"
	"github.com/tidwall/buntdb"
)

func main() {
	db, err := buntdb.Open(":memory:")
	if err != nil {
		log.Fatalf("Failed opening database: %s", err)
	}
	srv := api.New(db)

	broker := "192.168.1.137"
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
	}

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
