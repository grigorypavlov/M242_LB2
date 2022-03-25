package api // import "github.com/grigorypavlov/M242_LB2/Server/api"

type UpdateRequest struct {
	Sensor string `json:"sensor"`
	Data   string `json:"data"`
}
