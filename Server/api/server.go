package api // import "github.com/grigorypavlov/M242_LB2/Server/api"

import (
	"encoding/json"
	"errors"
	"log"
	"net/http"

	"github.com/tidwall/buntdb"
)

// New creates an instance of Server.
func New(db *buntdb.DB) *Server {
	return &Server{
		db: db,
		errorHandler: func(err error) {
			log.Printf("Error encountered: %s", err)
		},
	}
}

type Server struct {
	db           *buntdb.DB
	errorHandler func(err error)
}

// PostUpdateReport accepts a json encoded UpdateRequest containing sensor
// data.
func (s *Server) PostUpdateReport(w http.ResponseWriter, r *http.Request) (err error) {
	if r.Method != http.MethodPost {
		w.WriteHeader(http.StatusMethodNotAllowed)
		return nil
	}

	req := &UpdateRequest{}
	err = json.NewDecoder(r.Body).Decode(req)
	if err != nil {
		s.errorHandler(err)
		return errorResponse(w, ErrInvalidRequest)
	}

	err = s.db.Update(func(tx *buntdb.Tx) error {
		_, _, err := tx.Set(req.Sensor, req.Data, nil)
		return err
	})
	if err != nil {
		s.errorHandler(err)
		return errorResponse(w, err)
	}

	w.WriteHeader(http.StatusOK)
	return err
}

// GetDataUpdate accepts a query parameter sensor and returns the most recent
// data from this sensor.
func (s *Server) GetDataUpdate(w http.ResponseWriter, r *http.Request) (err error) {
	if r.Method != http.MethodGet {
		w.WriteHeader(http.StatusMethodNotAllowed)
		return nil
	}

	sensor := r.FormValue("sensor")
	if sensor == "" {
		w.WriteHeader(http.StatusBadRequest)
		return nil
	}

	var data string
	err = s.db.View(func(tx *buntdb.Tx) error {
		data, err = tx.Get(sensor)
		return err
	})
	if err != nil {
		if errors.Is(err, buntdb.ErrNotFound) {
			w.Header().Set("Content-Type", "application/json")
			w.Header().Set("Access-Control-Allow-Origin", "*")
			w.Write([]byte("{\"error\": \"no sensor data\"}"))
			return nil
		}
		s.errorHandler(err)
		return errorResponse(w, err)
	}

	w.Header().Set("Content-Type", "application/json")
	w.Header().Set("Access-Control-Allow-Origin", "*")
	_, err = w.Write([]byte(data))
	if err != nil {
		s.errorHandler(err)
		return errorResponse(w, err)
	}
	return err
}

func jsonResponse(w http.ResponseWriter, data map[string]interface{}, statusCode int) error {
	w.Header().Set("Content-Type", "application/json")

	w.WriteHeader(statusCode)
	return json.NewEncoder(w).Encode(data)
}

func errorResponse(w http.ResponseWriter, err error) error {
	d, c := GetErrorData(err)
	return jsonResponse(w, d, c)
}

func GetErrorData(err error) (map[string]interface{}, int) {
	var res ErrorResponse

	if v, ok := Descriptions[err]; ok {
		res.Error = err
		res.Description = v
		res.StatusCode = StatusCodes[err]
	} else {
		res.Error = ErrServerError
		res.Description = Descriptions[ErrServerError]
		res.StatusCode = StatusCodes[ErrServerError]
	}

	data := map[string]interface{}{
		"error":             res.Error.Error(),
		"error_code":        res.StatusCode,
		"error_description": res.Description,
	}
	return data, res.StatusCode
}
