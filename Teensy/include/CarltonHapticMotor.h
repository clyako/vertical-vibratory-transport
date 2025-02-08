#ifndef CarltonHapticMotor_h
#define CarltonHapticMotor_h
#include <Arduino.h>
#include "AccelerometerICM20649.h"

class CarltonHapticMotor
{

public:
    CarltonHapticMotor(uint8_t dac_pin, int analog_res, uint8_t max_voltage_pin, uint8_t time_between_decrements_pin, uint8_t voltage_offset_read_pin, uint8_t voltage_offset_write_pin, uint8_t square_offset_low_read_pin);
    void offset_square();
    float sawtooth(unsigned long time_on_high, float time_between_decrements = -1);
    void pulse_motor(AccelerometerICM20649 imu, float intensity, unsigned long pulse_time = 0);
    void print_acceleration(sensors_event_t acceleration_event);
    void zero();

private:
    float _max_pwm_pin_write_voltage = 3.3;   // [V] max voltage output from DAC
    float _max_diff_amplifier_voltage = 5.27; // [V] maximum voltage before power amp (output of difference amp)
    float _diff_amp_gain = 15000.0 / 4700.0;  // difference amplifier gain (using 15k and 4.7k ohm resistors)
    int _analog_res;                          // set the resolution
    int _num_counts;                          // number of bits of analog reads based on '_analog_res'
    int _max_dac_value;                       // controls the peak-to-peak voltage of the sawtooth waveform written to the motor, as well as the maximum pulse intensity
    int _motor_zero_value;                    // half of the maximum DAC write value to get zero current flowing through the motor

    /* Pin definitions */
    uint8_t _dac_pin;                     // DAC pin on the Teensy 3.6
    uint8_t _max_voltage_pin;             // pin connected to a potentiometer that changes the variable '_max_dac_value'
    uint8_t _time_between_decrements_pin; // pin connected to a potentiometer that changes the frequency of the sawtooth / square wave written to the motor
    uint8_t _voltage_offset_read_pin;     // pin connected to a potentiometer that sets the voltage offset of the waveform
    uint8_t _voltage_offset_write_pin;    // pin that writes the voltage offset to the difference amplifier
    uint8_t _square_offset_low_read_pin;  // pin connected to a potentiometer that reads the negative acceleration value for the square wave

    /* Sawtooth Wave Parameters */
    // these 3 parameters control the frequency of the sawtooth wave
    float _time_between_decrements = 50;    // [µs]
    float _decrement_counter = 0;           // the number of counts to write (corresponds to the current to send to the motor)
    float _decrement_amount = 1;            // how much to decrement the counter every '_time_between_decrements' steps
    elapsedMicros _decrement_voltage_timer; // to consistently write
    // control how long the maximum DAC value is written for
    int _write_high = 0;
    elapsedMicros _high_voltage_timer;
    // range of frequencies that can be written to the motor by turning the corresponding potentiometer
    float _sawtooth_min_frequency = 1;   // [Hz]
    float _sawtooth_max_frequency = 100; // [Hz]

    void _update_voltage_offset();
    void _update_max_dac_value();
    void _update_sawtooth_frequency();

    /* Square Wave Parameters */
    // parameters that control the amount of current and how long each high and low acceleration phases are run
    float _offset_square_time_on_high;       // [µs]
    float _offset_square_time_on_low;        // [µs]
    float _offset_square_high_value;         // [V] - corresponds to current
    float _offset_square_low_value;          // [V] - corresponds to current
    elapsedMicros _offset_square_high_timer; //
    elapsedMicros _offset_square_low_timer;  //
    bool _offset_square_write_high = true;   //
    // range of frequencies that can be written to the motor by turning the corresponding potentiometer
    float _offset_square_min_frequency = 1;   //
    float _offset_square_max_frequency = 100; //
    void _update_offset_square_waveform();

    elapsedMillis _pulse_timer; // timer for controlling how long a pulse is sent for (compared to user passed variable 'on_time')
};
#endif