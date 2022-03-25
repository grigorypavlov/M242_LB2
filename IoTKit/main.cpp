/**
 * Collects sensor information and sends it to a REST API.
 * Sensor overview: https://github.com/iotkitv3/i2c
 */
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include "mbed.h"
#include <string>
#include "OLEDDisplay.h"
#include "http_request.h"
#include "MbedJSONValue.h"
#if MBED_CONF_IOTKIT_HTS221_SENSOR == true
#include "HTS221Sensor.h"
#endif
#if MBED_CONF_IOTKIT_BMP180_SENSOR == true
#include "BMP180Sensor.h"
#endif

static DevI2C devI2c(MBED_CONF_IOTKIT_I2C_SDA, MBED_CONF_IOTKIT_I2C_SCL);
#if MBED_CONF_IOTKIT_HTS221_SENSOR == true
static HTS221Sensor hum_temp(&devI2c);
#endif
#if MBED_CONF_IOTKIT_BMP180_SENSOR == true
static BMP180Wrapper hum_temp(&devI2c);
#endif

OLEDDisplay oled(MBED_CONF_IOTKIT_OLED_RST, MBED_CONF_IOTKIT_OLED_SDA, MBED_CONF_IOTKIT_OLED_SCL);

#define API_URI "http://192.168.104.29:8080/api/v1/hello_world"

using std::printf;

int32_t main(void)
{
    oled.clear();
    oled.printf("Starting...\n");

    // Initialize sensors.
    hum_temp.init(NULL);
    hum_temp.enable(); 

    // Connect to WiFi network over the default networking interface.
    // See `mbed_app.json` for the WiFi credentials.
    WiFiInterface* network = WiFiInterface::get_default_instance();

    if (!network) {
        printf("ERROR: No WiFiInterface found.\n");
        return EXIT_FAILURE;
    }

    printf("Connecting to %s...", MBED_CONF_APP_WIFI_SSID);
    int32_t ret = network->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);

    if (ret != 0) {
        printf("\nConnection error: %d\n", ret);
        return EXIT_FAILURE;
    }

    printf("SUCCESS\n");
    printf("MAC: %s\n", network->get_mac_address());

    SocketAddress a;
    network->get_ip_address(&a);
    printf("IP: %s\n", a.get_ip_address());

    float temperature, humidity;

    uint8_t id;
    hum_temp.read_id(&id);
    printf("Humidity & Temperature Sensor = 0x%X\r\n", id);

    for (;;)
    {
        hum_temp.get_temperature(&temperature);
        hum_temp.get_humidity(&humidity);

        oled.cursor(1, 0);
        oled.printf("temp: %3.2f\nhum:  %3.2f\n", temperature, humidity);

        // By default the body is automatically parsed and stored in a buffer, this is memory heavy.
        // To receive chunked response, pass in a callback as last parameter to the constructor.
        HttpRequest* post_req = new HttpRequest(network, HTTP_POST, API_URI);
        post_req->set_header("Content-Type", "application/json");

        // Build request body.
        const char format[] = "{\"sensor\": \"hum_temp\",\"data\":\"[%f, %f]\"}";
        char body[80];
        std::sprintf(body, format, humidity, temperature);

        HttpResponse* get_res = post_req->send(body, strlen(body));

        if (get_res)
        { // OK
            printf("HttpRequest status code: %d\n", get_res->get_status_code());
        }
        else
        { // Error
            printf("HttpRequest failed (error code %d)\n", post_req->get_error());
            delete post_req;
            thread_sleep_for(15000); // milliseconds
            continue;
        }

        delete post_req;
        thread_sleep_for(5000); // milliseconds
    }

    return EXIT_SUCCESS;
}
