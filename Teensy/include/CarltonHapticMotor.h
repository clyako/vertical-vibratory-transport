#ifndef CarltonHapticMotor_h
#define CarltonHapticMotor_h
#include <Arduino.h>

#define DAC_PIN A21                     // DAC pin on the Teensy 3.6; writes a voltage which linearly corresponds to a current
#define MAX_VOLTAGE_PIN A15             // pin connected to a potentiometer that changes the variable '_max_dac_value'
#define TIME_BETWEEN_DECREMENTS_PIN A16 // pin connected to a potentiometer that changes the frequency of the sawtooth / square wave written to the motor
#define VOLTAGE_OFFSET_READ_PIN A17     // pin connected to a potentiometer that sets the voltage offset of the waveform
#define VOLTAGE_OFFSET_WRITE_PIN A19    // pin that writes the voltage offset to the difference amplifier
#define ANALOG_RESOLUTION 10            // analog read / write resolution

class CarltonHapticMotor
{

public:
    CarltonHapticMotor();
    float sawtooth(unsigned long time_on_high, float time_between_decrements = -1);
    void zero();

private:
    float _max_pwm_pin_write_voltage = 3.3;   // [V] max voltage output from DAC
    float _max_diff_amplifier_voltage = 5.27; // [V] maximum voltage before power amp (output of difference amp)
    float _diff_amp_gain = 15000.0 / 4700.0;  // difference amplifier gain (using 15k and 4.7k ohm resistors)
    int _analog_res;                          // set the resolution
    int _num_counts;                          // number of bits of analog reads based on '_analog_res'
    int _max_dac_value;                       // controls the peak-to-peak voltage of the sawtooth waveform written to the motor, as well as the maximum pulse intensity
    int _motor_zero_value;                    // half of the maximum DAC write value to get zero current flowing through the motor

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
};
#endif