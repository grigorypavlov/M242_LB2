# REST (RESTful API)

Short for *Re*presentational *S*tate *T*ransfer, builds on top of HTTP.

The HTTP header contains the (request) method and information about the
request/response body, the body contains the actual request/response message.

A client sends requests over HTTP(S) to a server:

```http
GET /api/v3/auth HTTP/1.1
Host: example.com
```

And the server may respond, for example, with JSON in the response body:

```http
HTTP/1.1 200 OK
Access-Control-Allow-Origin: *
Content-Type: application/json

{
    "data": 5,
    "more-data": "hello, world"
}
```

## Request Methods

Describe an action. The most important ones are:

Method | Description
------ | -----------
GET    | Get a resource from the server.
POST   | Send data to be processed on the server.
PUT    | Upload a resource to the server. (Or modify, if the resource already exists.)
DELETE | Delete a resource from the server.

GET and POST are very common, PUT and DELETE are mostly found in file transfer
protocols like "remoteStorage".

## HTTP Requests On the IoTKit

Include the required header files.

```cpp
#include "mbed.h"
#include "http_request.h"
#include "MbedJSONValue.h"

int main(void) {
    // ...
}
```

Connect to WiFi

```cpp
WiFiInterface* network = WiFiInterface::get_default_instance();
if (!network) {
    // Error
}

int ret = network->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
if (ret != 0) {
    // Error
}
```

The call to `get_default_instance` uses the credentials configured in the
`mbed_app.json` file.

```json
{
    "config": {
        "wifi-ssid": {
            "help": "WiFi SSID",
            "value": "\"WLAN_NAME\""
        },
        "wifi-password": {
            "help": "WiFi Password",
            "value": "\"PASSWORD\""
        }
    },
}
```

Make a GET request.

```cpp
HttpRequest* get_req = new HttpRequest(network, HTTP_GET, "http://api.example.com?var=val");
HttpResponse* get_res = get_req->send();

// Clean-up
delete get_req;
```

Make a POST request.

```cpp
HttpRequest* post_req = new HttpRequest(network, HTTP_POST, "http://api.example.com/endpoint");
post_req->set_header("Content-Type", "application/json");
const char body[] = "{\"key\":\"value\"}";
HttpResponse* post_res = post_req->send(body, strlen(body));

// Clean-up
delete post_res;
```

Parse a JSON response.

```cpp
MbedJSONValue parser;
parse(parser, get_res->get_body_as_string().c_str());

std::string val1;
std::string val2;

sunrise = parser["val1"].get<std::string>();
sunset  = parser["val2"].get<std::string>();
```

## Re-using Sockets

Sockets can be re-used to be more resource friendly.

```cpp
// Allows us to re-use the same socket for each request (more resource friendly).
TCPSocket* socket = new TCPSocket();
nsapi_error_t open_result = socket->open(network);
if (open_result != 0)
{ // Error
    printf("Opening Socket Failed: %d\n", open_result);
    return EXIT_FAILURE;
}

nsapi_error_t connect_result = socket->connect(API_BASE);
if (connect_result != 0)
{ // Error
    printf("Connecting to API Failed: %d\n", connect_result);
    return EXIT_FAILURE;
}
```

In network requests, we pass the socket as the first argument, instead of the
network.

```cpp
HttpRequest* post_req = new HttpRequest(socket, HTTP_POST, API_URI);
```

Once we don't need it anymore, we can close and clean it up.

```cpp
socket->close();
delete socket;
```
