package main

import (
	"fmt"
	"log"
	"net/http"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/grigorypavlov/M242_LB2/Server/sse"
)

// Notifier channel sents broadcasts into the broker.
var Notifier chan []byte

func main() {
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

	broker := sse.NewServer()
	Notifier = broker.Notifier

	/*go func() {
		var input string
		for {
			fmt.Scanf("%s", &input)

			broker.Notifier <- []byte(input)
		}
	}()*/

	log.Fatal("HTTP server error: ", http.ListenAndServe(":8080", loggingMiddleware(broker)))
}

func loggingMiddleware(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		log.Printf("Request received: %v\n", r)
		next.ServeHTTP(w, r)
	})
}

func handleMessage(client mqtt.Client, msg mqtt.Message) {
	fmt.Printf("Client: %v Received Message: %v\n", client, msg)
	Notifier <- msg.Payload()
}
