package api

import (
	"net/http"
)

func Default() *Server {
	return &Server{}
}

type Server struct{}

func (s *Server) PostData(w http.ResponseWriter, r *http.Request) error {
	if r.Method != http.MethodPost {
		w.WriteHeader(http.StatusMethodNotAllowed)
		return nil
	}

	_, err := w.Write([]byte("Hello, World."))
	return err
}
