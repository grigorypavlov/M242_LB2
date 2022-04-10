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
#include "LSM6DSLSensor.h"
#include <BMP180Wrapper.h>
#include "glibr.h"
#include <MQTTClientMbedOs.h>

static DevI2C devI2c(MBED_CONF_IOTKIT_I2C_SDA, MBED_CONF_IOTKIT_I2C_SCL);
#if MBED_CONF_IOTKIT_HTS221_SENSOR == true
static HTS221Sensor hum_temp(&devI2c);
#endif
#if MBED_CONF_IOTKIT_BMP180_SENSOR == true
static BMP180Wrapper hum_temp(&devI2c);
#endif
static LSM6DSLSensor tilt(&devI2c, LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW);
static BMP180Wrapper air_pressure(&devI2c);
static glibr gsensor(D14, D15);

OLEDDisplay oled(MBED_CONF_IOTKIT_OLED_RST, MBED_CONF_IOTKIT_OLED_SDA, MBED_CONF_IOTKIT_OLED_SCL);

#define API_BASE "http://192.168.65.55:8080/"
#define API_URI "http://192.168.65.55:8080/api/v1/hello_world"

using std::printf;

int32_t main(void)
{
    oled.clear();
    oled.printf("Starting...\n");

    /*// Read time from terminal.
    struct tm t;
    printf("Enter current date and time:\n");
    printf("YYYY MM DD HH MM SS[Enter]\n");
    std:scanf("%d %d %d %d %d %d", &t.tm_year, &t.tm_mon, &t.tm_mday,
        &t.tm_hour, &t.tm_min, &t.tm_sec);

    // Values need to be adjusted.
    t.tm_year = t.tm_year - 1900;
    t.tm_mon = t.tm_mon - 1;

    // Set the time.
    set_time(mktime(&t));*/

    // Initialize sensors.
    uint8_t id;

    if (gsensor.ginit())
    { // Success
        printf("\nAPDS-9960 initialization completed\r\n");
    }
    else 
    { // Error
        printf("\nAPDS-9960 initialiaztion ERROR!\r\n");
        return EXIT_FAILURE;
    }

    if (gsensor.enableGestureSensor(true))
    { // Success
        printf("Gesture sensor is running.\r\n");
    }
    else
    { // Error
        printf("Gestore sensor ERROR!\r\n");
        return EXIT_FAILURE;
    }

    hum_temp.init(NULL);
    hum_temp.enable();

    hum_temp.read_id(&id);
    printf("Humidity & Temperature Sensor = 0x%X\r\n", id);
    
    tilt.init(NULL);
    tilt.enable_g();
    tilt.enable_x();
    tilt.enable_6d_orientation(); 

    tilt.read_id(&id);
    printf("LSM6DSL accelerometer & gyroscope = 0x%X\r\n", id);

    air_pressure.init(NULL);
    air_pressure.enable();

    air_pressure.read_id(&id);
    printf("BMP180 air pressure = 0x%X\r\n", id);

    // Connect to WiFi network over the default networking interface.
    // See `mbed_app.json` for the WiFi credentials.
    WiFiInterface* network = WiFiInterface::get_default_instance();

    if (!network) {
        printf("\nERROR: No WiFiInterface found.\n");
        return EXIT_FAILURE;
    }

    printf("\nConnecting to %s...", MBED_CONF_APP_WIFI_SSID);
    nsapi_error_t ret = network->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);

    if (ret != 0) {
        printf("\nConnection error: %d\n", ret);
        return EXIT_FAILURE;
    }

    printf("SUCCESS\n");
    printf("MAC: %s\n", network->get_mac_address());

    SocketAddress a;
    network->get_ip_address(&a);
    printf("IP: %s\n", a.get_ip_address());

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

    MQTTClient client(socket);
    MQTTPacket_connectData conn_data = MQTTPacket_connectData_initializer;
    conn_data.MQTTVersion = 3;
    conn_data.clientID.cstring = (char *) "IoTKitV3";
    conn_data.username.cstring = (char *) "above";
    conn_data.password.cstring = (char *) "andbeyond";
    nsapi_error_t mqtt_connect_result = client.connect(conn_data);
    if (mqtt_connect_result != 0)
    { // Error
        printf("Connecting to Broker Failed: %d\n", mqtt_connect_result);
    }

    float temperature, humidity;
    uint8_t xl, xh, yl, yh, zl, zh;
    xl = xh = yl = yh = zl = zh = 0;
    float pressure;

    for (;;)
    {
        oled.clear();
        if (gsensor.isGestureAvailable() && gsensor.readGesture() != DIR_NONE)
        {
            time_t unix_seconds = time(NULL);
            char time_buffer[32];
            strftime(time_buffer, 32, "%H:%M\n", localtime(&unix_seconds));
            printf("Current local time: %s", time_buffer);

            oled.cursor(1, 0);
            oled.printf(time_buffer);

            hum_temp.get_temperature(&temperature);
            hum_temp.get_humidity(&humidity);

            printf("\ntemp: %f, hum: %f\n", temperature, humidity);

            tilt.get_6d_orientation_xl(&xl);
            tilt.get_6d_orientation_xh(&xh);
            tilt.get_6d_orientation_xl(&yl);
            tilt.get_6d_orientation_yh(&yh);
            tilt.get_6d_orientation_zl(&zl);
            tilt.get_6d_orientation_zh(&zh);

            printf("\nxl %d, xh %d, yl %d, yh %d, zl %d, zh %d\n", xl, xh, yl, yh, zl, zh);

            air_pressure.get_humidity(&pressure);

            printf("Air pressure [kPa] %.2f%%\r\n", pressure);

            HttpRequest* post_req = new HttpRequest(socket, HTTP_POST, API_URI);
            post_req->set_header("Content-Type", "application/json");

            // Build request body.
            const char format[] = "[{\"sensor\":\"hum_temp\",\"data\":\"[%f, %f]\"},{\"sensor\":\"tilt\",\"data\":\"[%d, %d, %d, %d, %d, %d]\"},{\"sensor\":\"pressure\",\"data\":\"%f\"}]";
            char body[80];
            std::sprintf(body, format, humidity, temperature, xl, xh, yl, yh, zl, zh, pressure);

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
            continue;
        }

        thread_sleep_for(10); // milliseconds
    }

    socket->close();
    delete socket;
    return EXIT_SUCCESS;
}
