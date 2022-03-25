# HTTPS

## IoTKit

Enable the IoTKit to use HTTPS.

```cpp
HttpsRequest* post_req = new HttpsRequest(network, SSL_CA_PEM, HTTP_POST, API_URI);
post_req->set_header("Content-Type", "application/json");
const char body[] = "{\"sensor\": \"hum_temp\", \"data\": [34.7, 29.8]}";

HttpResponse* post_res = post_req->send(body, strlen(body));
if (!post_res) {
    // Handle error
}

delete post_req;
```

The constant `SSL_CA_PEM` stores a list of trusted root CAs (Certificate
Authority, issues digital certificates for TLS).

```cpp
const char SSL_CA_PEM[] = "-----BEGIN CERTIFICATE-----\n"
    /* Certificates... */;
    "-----END CERTIFICATE-----\n"
```

## Server

To enable HTTPS on the server side, we need to change the line:

```go
log.Fatal(http.ListenAndServeTLS(":8080", "cert.pem", "key.pem", nil)
```

`cert.pem` and `key.pem` are the respective certificate and private key files.
