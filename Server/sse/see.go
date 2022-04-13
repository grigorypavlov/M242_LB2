package sse

import (
	"fmt"
	"log"
	"net/http"
)

// NewServer instantiates a new broker and sets it to listening.
func NewServer() (broker *Broker) {
	broker = &Broker{
		Notifier:       make(chan []byte, 1),
		newClients:     make(chan chan []byte),
		closingClients: make(chan chan []byte),
		clients:        make(map[chan []byte]struct{}),
	}

	go broker.listen()

	return
}

// A Broker holds open client connections, listens for incoming events on its
// Notifier channel and broadcasts event data to all registered connections.
type Broker struct {
	Notifier       chan []byte
	newClients     chan chan []byte
	closingClients chan chan []byte
	clients        map[chan []byte]struct{}
}

func (b *Broker) listen() {
	for {
		select {
		case s := <-b.newClients:
			// New client connected. Register its message channel.
			b.clients[s] = struct{}{}
			log.Printf("Client added. %d registered clients.", len(b.clients))
		case s := <-b.closingClients:
			// Client disconnected. Stop sending it messages.
			delete(b.clients, s)
			log.Printf("Removed client. %d registered clients.", len(b.clients))
		case event := <-b.Notifier:
			// Message received, send it to all connected clients.
			for clientMsgChan := range b.clients {
				clientMsgChan <- event
			}
		}
	}
}

func (b *Broker) ServeHTTP(w http.ResponseWriter, req *http.Request) {
	// Make sure writer supports flushing.
	flusher, ok := w.(http.Flusher)

	if !ok {
		http.Error(w, "streaming unspported", http.StatusInternalServerError)
		return
	}

	// Set headers related to event streaming.
	w.Header().Set("Access-Control-Allow-Origin", "*")
	w.Header().Set("Access-Control-Allow-Headers", "Content-Type")
	w.Header().Set("Content-Type", "text/event-stream")
	w.Header().Set("Cache-Control", "no-cache")
	w.Header().Set("Connection", "keep-alive")

	// Every connection registers its own message channel with the broker's
	// registry.
	msgChan := make(chan []byte)

	// Signal broker that there is a new connection.
	b.newClients <- msgChan

	// Remove client from registry when handler exists.
	defer func() {
		b.closingClients <- msgChan
	}()

	notify := w.(http.CloseNotifier).CloseNotify()

	go func() {
		<-notify
		b.closingClients <- msgChan
	}()

	// Wait for message broadcasts and sent any broadcasts received to the client.
	for {
		fmt.Fprintf(w, "data: %s\n\n", <-msgChan)
		flusher.Flush()
	}
}
