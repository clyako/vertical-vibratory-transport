#ifndef CarltonHapticMotor_h
#define CarltonHapticMotor_h
#include <Arduino.h>
#include "AccelerometerICM20649.h"

class CarltonHapticMotor
{

public:
    CarltonHapticMotor(uint8_t pwm_pin, int pwm_res, uint8_t max_voltage_pin, uint8_t time_between_decrements_pin, uint8_t voltage_offset_read_pin, uint8_t voltage_offset_write_pin, uint8_t square_offset_low_read_pin);
    void offset_square();
    float sawtooth(unsigned long time_on_high, float time_between_decrements = -1);
    void pulse_motor(AccelerometerICM20649 imu, float intensity, unsigned long pulse_time = 0);
    void print_acceleration(sensors_event_t acceleration_event);
    void zero();

private:
    uint8_t _pwm_pin;                         // I think this is actually the DAC pin on the 3.6
    float _max_pwm_pin_write_voltage = 3.3;   // max voltage output from DAC
    float _max_diff_amplifier_voltage = 5.27; // maximum voltage before power amp (output of difference amp)
    float _diff_amp_gain = 15000.0 / 4700.0;  // difference amplifier gain (using 15k and 4.7k ohm resistors)
    int _pwm_res;                             // set the resolution
    int _num_counts;                          // number of bits of analog reads based
    int _max_pwm_value;                       // controls the peak-to-peak voltage of the sawtooth waveform written to the motor, as well as the maximum pulse intensity
    uint8_t _max_voltage_pin;                 // pin connected to a potentiometer that changes the variable '_max_pwm_value'
    uint8_t _time_between_decrements_pin;     // pin connected to a potentiometer that changes the frequency of the sawtooth / square wave written to the motor
    uint8_t _voltage_offset_read_pin;         //
    uint8_t _voltage_offset_write_pin;        //
    uint8_t _square_offset_low_read_pin;      //
    int _motor_zero_value;                    //
    float _time_between_decrements = 50;      //
    float _decrement_counter = 0;             //
    float _decrement_amount = 1;              //
    int _write_high = 0;                      //
    int _min_frequency = 1;                   // Hz
    int _max_frequency = 50;                  // Hz
    elapsedMicros _decrement_voltage_timer;   //
    elapsedMicros _high_voltage_timer;        //
    elapsedMillis _pulse_timer;               //
    float _sawtooth_min_frequency = 1;        //
    float _sawtooth_max_frequency = 100;      //
    elapsedMicros _offset_square_high_timer;  //
    elapsedMicros _offset_square_low_timer;   //
    float _offset_square_time_on_high;        //
    float _offset_square_time_on_low;         //
    float _offset_square_high_value;          //
    float _offset_square_low_value;           //
    bool _offset_square_write_high = true;    //
    float _offset_square_write_value = 0;     //
    float _offset_square_min_frequency = 0;   //
    float _offset_square_max_frequency = 100; //
    void _update_voltage_offset();
    void _update_max_pwm_value();
    void _update_sawtooth_frequency();
    void _update_offset_square_waveform();
};
#endif