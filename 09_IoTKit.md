# IoTKit Documentation

The program is constructed very simple, using only a `main.cpp` file and
running in an infinite loop.

```cpp
// Includes ...

// Constant definitions ...

// Main entry point.
int32_t main(void) {
    // Sensor setup
    hum_temp.init(NULL);
    hum_temp.enable(); 
    // ...

    // Networking setup
    WiFiInterface* network = WiFiInterface::get_default_instance();

    if (!network) {
        printf("ERROR: No WiFiInterface found.\n");
        return EXIT_FAILURE;
    }

    // Infinite loop, read sensor data, send data to server, sleep, repeat.
    for (;;)
    {
        // Read sensor data
        hum_temp.get_temperature(&temperature);
        hum_temp.get_humidity(&humidity);
        // ...

        // Send POST requests
        HttpRequest* post_req = new HttpRequest(network, HTTP_POST, API_URI);
        post_req->set_header("Content-Type", "application/json");
        // ...

        // Clean up resources
        delete post_req;

        // Sleep for 5 seconds
        thread_sleep_for(5000); // milliseconds
    }
}
```
