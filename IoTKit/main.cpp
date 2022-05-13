/**
 * Collects sensor information and publishes it to a MQTT Broker.
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
#include "MQTTNetwork.h"

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

#define BROKER_HOST "192.168.65.55"
#define BROKER_PORT 1883
#define TOPIC_ACTION "action"


using std::printf;

void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    printf("Message arrived: qos %d, retained: %d, dup: %d, packetid: %d\n", message.qos, message.retained, message.dup, message.id);
    printf("Message payload: %.*s\n", message.payloadlen, (char*)message.payload);
    
    char * content = (char*) message.payload;
    oled.printf(content);
}

int32_t main(void)
{
    oled.clear();
    oled.printf("Starting...\n");

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

    nsapi_error_t rc;

    MQTTNetwork mqttNetwork(network);
    MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);
    if ((rc = mqttNetwork.connect(BROKER_HOST, BROKER_PORT)) != 0)
    { // Error
        printf("TCP Connection Returned: %d\n", rc);
    }

    MQTTPacket_connectData conn_data = MQTTPacket_connectData_initializer;
    conn_data.MQTTVersion = 3;
    conn_data.clientID.cstring = (char *) "IoTKitV3";
    conn_data.username.cstring = (char *) "above";
    conn_data.password.cstring = (char *) "andbeyond";

    if ((rc = client.connect(conn_data)) != 0)
    { // Error
        printf("Connecting to Broker Failed: %d\n", rc);
        return EXIT_FAILURE;
    }

    if ((rc = client.subscribe(TOPIC_ACTION, MQTT::QOS2, messageArrived)) != 0)
    { // Error
        printf("Subscribing to Topic Failed: %d\n", rc);
    }

    float temperature, humidity;
    uint8_t xl, xh, yl, yh, zl, zh;
    xl = xh = yl = yh = zl = zh = 0;
    float pressure;

    char body[80];
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;

    std::sprintf(body, "Running");
    message.payload = (void*)body;
    message.payloadlen = strlen(body)+1;
    rc = client.publish("LB3/state", message);

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

            std:sprintf(body, "{\"motion\":\"none\"}");
            message.payload = (void*)body;
            message.payloadlen = strlen(body)+1;
            rc = client.publish("LB3/motion", message);
            
            std::sprintf(body, "%f", temperature);
            message.payload = (void*)body;
            message.payloadlen = strlen(body)+1;
            rc = client.publish("LB3/temperature", message);

            std::sprintf(body, "%f", humidity);
            message.payload = (void*)body;
            message.payloadlen = strlen(body)+1;
            rc = client.publish("LB3/humidity", message);

            std::sprintf(body, "{\"tilt\":[%d %d %d %d %d %d]}", xl, xh, yl, yh, zl, zh);
            message.payload = (void*)body;
            message.payloadlen = strlen(body)+1;
            rc = client.publish("LB3/tilt", message);

            std::sprintf(body, "%f", pressure);
            message.payload = (void*)body;
            message.payloadlen = strlen(body)+1;
            rc = client.publish("LB3/pressure", message);

            if (rc != 0)
            { // Error
                printf("Publish failed: %d\n", rc);
                thread_sleep_for(15000); // milliseconds
            }
            else
            { // OK
                thread_sleep_for(5000); // milliseconds
            }
        }
        else
        {
            std::sprintf(body, "{\"motion\":\"none\"}");
            message.payload = (void*)body;
            message.payloadlen = strlen(body)+1;
            rc = client.publish("LB3/motion", message);
        }

        thread_sleep_for(10); // milliseconds
    }

    return EXIT_SUCCESS;
}
