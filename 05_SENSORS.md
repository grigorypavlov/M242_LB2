# Sensors

What works?

A tick means we were able to get the sensor working correctly.

- [x] HTS221
- [ ] LIS3MDL (Reads incorrect data)
- [ ] LSM6DSL (Reads incorrect data)

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
