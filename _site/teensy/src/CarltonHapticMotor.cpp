#include "CarltonHapticMotor.h"

CarltonHapticMotor::CarltonHapticMotor()
{
    _analog_res = ANALOG_RESOLUTION;
    _max_dac_value = pow(2, _analog_res) - 1;
    _num_counts = pow(2, _analog_res);
    _motor_zero_value = _max_dac_value / 2;

    analogWriteResolution(_analog_res);
    analogReadResolution(_analog_res);
    pinMode(DAC_PIN, OUTPUT);
    pinMode(VOLTAGE_OFFSET_READ_PIN, INPUT);
    pinMode(VOLTAGE_OFFSET_WRITE_PIN, OUTPUT);
    zero();
}

/*
  The user can send a user-controllable sawtooth current profile. This is what was used in the paper and video demos.
  Potentiometers are used to change the height of the sawtooth ('_max_dac_value'), the frequency ('_decrement_amount'),
  and the offset of the waveform ('new_voltage_offset_value'). This function works by first writing '_max_dac_value' and
  then decrementing an amount ('_decrement_amount') based on the desired frequency and the value '_time_between_decrements'.
  This resets once '_decrement_counter' becomes less than zero (and the 'time_on_high' value is surpassed).
*/
float CarltonHapticMotor::sawtooth(unsigned long time_on_high, float time_between_decrements)
{
    // controls if frequency is fixed (set by user) or controlled via a potentiometer
    if (time_between_decrements != -1)
    {
        _time_between_decrements = time_between_decrements;
    }
    else
    {
        _update_sawtooth_frequency();
    }
    _update_max_dac_value();
    _update_voltage_offset();

    float dac_write_value = 0;
    if ((float)_decrement_voltage_timer >= _time_between_decrements && _decrement_counter > 0)
    {
        _decrement_counter = _decrement_counter - _decrement_amount;
        dac_write_value = _decrement_counter;
        analogWrite(DAC_PIN, _decrement_counter);
        _decrement_voltage_timer = 0;
    }
    else if (_decrement_counter <= 0)
    {
        // controls how long the DAC writes '_max_dac_value' before decrementing again
        if (_write_high == 0)
        {
            _write_high = 1;
            _high_voltage_timer = 0;
            analogWrite(DAC_PIN, _max_dac_value);
        }
        if (_high_voltage_timer >= time_on_high)
        {
            _write_high = 0;
            _decrement_counter = (float)_max_dac_value;
        }
        dac_write_value = _max_dac_value;
    }
    return dac_write_value;
}

/*
  Updates the variable '_decrement_amount' which corresponds to the frequency of the sawtooth waveform.
*/
void CarltonHapticMotor::_update_sawtooth_frequency()
{
    float frequency_potentiometer = analogRead(TIME_BETWEEN_DECREMENTS_PIN);
    float new_waveform_frequency = (1 - frequency_potentiometer / _num_counts) * (_sawtooth_max_frequency - _sawtooth_min_frequency) + _sawtooth_min_frequency;
    float microseconds_per_cycle = 1000000 / new_waveform_frequency;
    float decrements_per_cycle = microseconds_per_cycle / _time_between_decrements;
    _decrement_amount = _max_dac_value / decrements_per_cycle;
}

/*
  A direct offset applied to the difference amplifier to shift the sawtooth waveform up and down.
*/
void CarltonHapticMotor::_update_voltage_offset()
{
    int new_voltage_offset_value = analogRead(VOLTAGE_OFFSET_READ_PIN);
    analogWrite(VOLTAGE_OFFSET_WRITE_PIN, new_voltage_offset_value);
}

/*
  Controls the amplitude of the sawtooth waveform.
*/
void CarltonHapticMotor::_update_max_dac_value()
{
    int new_max_dac_value = analogRead(MAX_VOLTAGE_PIN);
    _max_dac_value = new_max_dac_value;
}

/*
  Sets the offset voltage and dac write value to the midpoint of the write values to stop the motor.
*/
void CarltonHapticMotor::zero()
{
    analogWrite(DAC_PIN, _motor_zero_value);
    analogWrite(VOLTAGE_OFFSET_WRITE_PIN, _motor_zero_value);
}
