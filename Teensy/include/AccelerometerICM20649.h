#ifndef AccelerometerICM20649_h
#define AccelerometerICM20649_h
#include <Arduino.h>
#include <Adafruit_ICM20649.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class AccelerometerICM20649
{

public:
    AccelerometerICM20649(int baud_rate,
                          uint8_t cs_pin,
                          uint8_t fchoice,
                          uint8_t dlpf_bw,
                          uint16_t accel_rate_divisor,
                          icm20649_accel_range_t accelerometer_range);
    void init();
    sensors_event_t read_accelerometer();
    float read_acceleration_1D(int dim);
    void initialize_serial();
    void initialize_imu();
    void check_accelerometer_range();
    void set_accelerometer_range();
    void set_accelerometer_data_rate();
    void get_dlpf_bandwidth();
    void check_dlpf_config();
    void configure_dlpf();
    float calculate_output_data_rate();
    void check_accelerometer_output_rate_divisor();
    void set_accelerometer_output_rate();
    void perform_settings_checks();

private:
    int _baud_rate;
    uint8_t _cs_pin;
    uint8_t _fchoice;
    uint8_t _dlpf_bw;
    uint16_t _accel_rate_divisor;
    icm20649_accel_range_t _accelerometer_range;

    Adafruit_ICM20649 _imu;
    sensors_event_t _accel;
};
#endif