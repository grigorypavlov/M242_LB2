# Sensors

## I2C

I-Squared-C, Inter-Integrated Circuit, is a serial databus. It is a
master-slave bus, a data transfer is always initialized by the master (MCU on
the board).

## Get the microprocessor's localtime

First, the time needs to be set. One possibility is to read from the terminal,
if one is connected to the microprocessor:

```cpp
// Read time from terminal.
struct tm t;
printf("Enter current date and time:\n");
printf("YYYY MM DD HH MM SS[Enter]\n");
std:scanf("%d %d %d %d %d %d", &t.tm_year, &t.tm_mon, &t.tm_mday,
    &t.tm_hour, &t.tm_min, &t.tm_sec);

// Values need to be adjusted.
t.tm_year = t.tm_year - 1900;
t.tm_mon = t.tm_mon - 1;

// Set the time.
set_time(mktime(&t));
```

Read the time:

```cpp
// Get and format the time.
time_t unix_seconds = time(NULL);
char time_buffer[32];
strftime(time_buffer, 32, "%H:%M\n", localtime(&unix_seconds));
printf("Current local time: %s", time_buffer);

// Display the time on the OLED screen.
oled.cursor(1, 0);
oled.printf(time_buffer);
```

## LSM6DSL

Accellerator and gyroscope, we use this sensor to measure the tilt of the
microprocessor.

```cpp
#include "LSM6DSLSensor.h"

static LSM6DSLSensor tilt(&devI2c, LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW);

// Initialize sensor.
tilt.init(NULL);
tilt.enable_g();
tilt.enable_x();
tilt.enable_6d_orientation(); 

// Get the sensor's ID.
tilt.read_id(&id);
printf("LSM6DSL accelerometer & gyroscope = 0x%X\r\n", id);

// Variables to store sensor data in.
uint8_t xl, xh, yl, yh, zl, zh;
xl = xh = yl = yh = zl = zh = 0;

for (;;) {
    // Reead sensor data.
    tilt.get_6d_orientation_xl(&xl);
    tilt.get_6d_orientation_xh(&xh);
    tilt.get_6d_orientation_xl(&yl);
    tilt.get_6d_orientation_yh(&yh);
    tilt.get_6d_orientation_zl(&zl);
    tilt.get_6d_orientation_zh(&zh);
}
```

## HTS221

Measure temperature and humidity.

Initialize the sensor:

```cpp
#if MBED_CONF_IOTKIT_HTS221_SENSOR == true // Sensor supported?
static DevI2C devI2c(MBED_CONF_IOTKIT_I2C_SDA, MBED_CONF_IOTKIT_I2C_SCL);
static HTS221Sensor hum_temp(&devI2c);
#endif

hum_temp.init(NULL);
hum_temp.enable();
```

Read sensor data:

```cpp
float temperature, humidity;
hum_temp.get_temperature(&temperature);
hum_temp.get_humidity(&humidity);

// Data is now stored in the vars temperature and humidity.
```

## BMP180

Can be used to measure air pressure.

```cpp
#include <BMP180Wrapper.h>
static BMP180Wrapper air_pressure(&devI2c);

// Initialize sensor.
air_pressure.init(NULL);
air_pressure.enable();

// Get sensor ID.
air_pressure.read_id(&id);
printf("BMP180 air pressure = 0x%X\r\n", id);

// Variable to store sensor data in.
float pressure;

for (;;) {
    // Read sensor data.
    // NOTE: The method says humidity, but actually reads air pressure in kPa.
    air_pressure.get_humidity(&pressure);
}
```

## APDS-9960

Motion/Gesture sensor.

```cpp
#include "glibr.h"
static glibr gsensor(D14, D15);

// Initialize and enable sensor.
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

// Wait for motion.
if (gsensor.isGestureAvailable() && gsensor.readGesture() != DIR_NONE)
{
    // Motion detected, do something...
}
```

There is a small red LED on the IoTKit, that will turn off whenever a gesture
is detected.

## LIS3MDL

3-axis magnet sensor.

Initialize the sensor:

```cpp
static LIS3MDL magnetometer(&devI2c);
magnetometer.init(NULL);
```

Read sensor data:

```cpp
int32_t axes[3];
magnetometer.get_m_axes(axes);

// Calculatea direction
int diff = axes[0] + axes[1];
diff = (diff < 0) ? diff * -1 : diff;
if  ( diff > 350 && diff < 370 )
    oled.printf( "north %6ld", diff );
else if ( diff > 450 && diff < 470 )
    oled.printf( "west  %6ld", diff );
else if ( diff > 50 && diff < 70 )
    oled.printf( "east  %6ld", diff );
else if ( diff > 130 && diff < 150 )
    oled.printf( "south %6ld", diff );
else
    oled.printf( "      %6ld", diff );
```

## LSM6DSL

3D gyroscope and accellerator sensor.

In this example used to count steps.

Initializ the sensor:

```cpp
static LSM6DSLSensor acc_gyro(&devI2c, LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW);
acc_gyro.init(NULL);
acc_gyro.enable_x();
acc_gyro.enable_pedometer();
```

Read sensor data:

In this example we count steps.

```cpp
LSM6DSL_Event_Status_t gryo_status;
uint16_t step_count;

acc_gyro.get_event_status(&gyro_status);

if (gyro_status.StepStatus) {
    step_count++;
}
```

Tab detection:

```cpp
acc_gyro.init(NULL);
acc_gyro.enable_x();
acc_gyro.enable_g();
acc_gyro.enable_single_tap_detection();

// ...

LSM6DSL_Event_Status_t gyro_status;
uint16_t tab_count;

acc_gyro.get_event_status(&gyro_status);

if (gyro_status.TapStatus) {
    tap_count++
}
```
