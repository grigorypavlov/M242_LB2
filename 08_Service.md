# Web Service

![Class Diagram](class_diagram.svg)

`Server` provides an endpoint for the IoTKit to send data to
(`PostUpdateReport`), and an endpoint for the client web interface to read data
from (`GetDataUpdate`).

The IoTKit sends `Request`s to the server, where `Sensor` describes the sensor
the data was measured with, and `Data` is the measured data json-encoded.

`ErrorResponse` is returned to the caller in case the request was malformed, or
another error occured.
