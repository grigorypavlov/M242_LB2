package api // import "github.com/grigorypavlov/M242_LB2/Server/api"

import "errors"

type ErrorResponse struct {
	Error       error
	StatusCode  int
	Description string
}

// Sentinel errors to check against.
var (
	ErrInvalidRequest = errors.New("invalid_request")
	ErrServerError    = errors.New("server_error")
)

// Descriptions mapped to errors.
var Descriptions = map[error]string{
	ErrInvalidRequest: "The request is missing a required parameter, includes an invalid parameter value, includes a parameter more than once, or is otherwise malformed.",
	ErrServerError:    "The server encountered an unexpected condition that prevented it from fulfilling the request.",
}

// StatusCodes mapped to errors.
var StatusCodes = map[error]int{
	ErrInvalidRequest: 400,
	ErrServerError:    500,
}
