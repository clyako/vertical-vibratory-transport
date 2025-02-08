#include "CarltonHapticMotor.h"

CarltonHapticMotor::CarltonHapticMotor(uint8_t dac_pin, int analog_res, uint8_t max_voltage_pin, uint8_t time_between_decrements_pin, uint8_t voltage_offset_read_pin, uint8_t voltage_offset_write_pin, uint8_t square_offset_low_read_pin)
{
    _dac_pin = dac_pin;
    _analog_res = analog_res;
    _max_dac_value = pow(2, _analog_res) - 1;
    _num_counts = pow(2, _analog_res);
    _motor_zero_value = _max_dac_value / 2;
    _max_voltage_pin = max_voltage_pin;
    _time_between_decrements_pin = time_between_decrements_pin;
    _voltage_offset_read_pin = voltage_offset_read_pin;
    _voltage_offset_write_pin = voltage_offset_write_pin;
    _square_offset_low_read_pin = square_offset_low_read_pin;

    analogWriteResolution(_analog_res);
    pinMode(_dac_pin, OUTPUT);
    pinMode(_voltage_offset_read_pin, INPUT);
    pinMode(_voltage_offset_write_pin, OUTPUT);
    zero();
}

/*
  This function allows the user to write a large current for a short time, followed by a low current
  for a longer time. Potentiometers are used as input to change the frequency, duty cycle, and current
  values (see '_update_offset_square_waveform()').
*/
void CarltonHapticMotor::offset_square()
{
    _update_offset_square_waveform();

    if ((float)_offset_square_high_timer >= _offset_square_time_on_high && _offset_square_write_high)
    {
        _offset_square_write_high = false;
        _offset_square_low_timer = 0;
        // write the value to the low value
        analogWrite(_dac_pin, _offset_square_low_value);
    }
    else if ((float)_offset_square_low_timer >= _offset_square_time_on_low && !_offset_square_write_high)
    {
        _offset_square_write_high = true;
        _offset_square_high_timer = 0;
        // write the value to the high value
        analogWrite(_dac_pin, _offset_square_high_value);
    }
}

/*
  Updates the offset square waveform parameters (frequency, duty cycle, and voltage / acceleration) based
  on the potentiometer values.
*/
void CarltonHapticMotor::_update_offset_square_waveform()
{
    // frequency
    float frequency_potentiometer = analogRead(_time_between_decrements_pin);
    float new_waveform_frequency = (1 - frequency_potentiometer / _num_counts) * (_offset_square_max_frequency - _offset_square_min_frequency) + _offset_square_min_frequency;

    // duty cycle
    float duty_cycle_raw = analogRead(_voltage_offset_read_pin);
    float new_duty_cycle = duty_cycle_raw / _num_counts;

    // set new times for the high and low accelerations in µs
    _offset_square_time_on_high = new_duty_cycle / new_waveform_frequency * 1000000;
    _offset_square_time_on_low = (1 - new_duty_cycle) / new_waveform_frequency * 1000000;

    // set offset to midpoint; this is an input to the difference amplifier to shift the waveform
    analogWrite(_voltage_offset_write_pin, (float)_num_counts / 2);
    float offset_voltage = _max_pwm_pin_write_voltage / 2;

    // calculating the high acceleration and low acceleration phases (acceleration is proportional to
    // current which is proportional to voltage)
    float positive_acceleration_voltage = analogRead(_max_voltage_pin) / (float)_num_counts * _max_diff_amplifier_voltage;
    float negative_acceleration_voltage = analogRead(_square_offset_low_read_pin) / (float)_num_counts * _max_diff_amplifier_voltage;
    _offset_square_high_value = (offset_voltage - positive_acceleration_voltage / _diff_amp_gain) * _num_counts / _max_pwm_pin_write_voltage;
    _offset_square_low_value = (offset_voltage + negative_acceleration_voltage / _diff_amp_gain) * _num_counts / _max_pwm_pin_write_voltage;
}

/*
  Instead of a square wave, the user can send a user-controllable sawtooth current profile. This is what
  was used in the paper and video demos. Potentiometers are used to change the height of the sawtooth
  ('_max_dac_value'), the frequency ('_decrement_amount'), and the offset of the waveform ('new_voltage_offset_value').
  This function works by first writing '_max_dac_value' and then decrementing an amount ('_decrement_amount') based
  on the desired frequency and the value '_time_between_decrements'. This resets once '_decrement_counter' becomes
  less than zero.
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
        analogWrite(_dac_pin, _decrement_counter);
        _decrement_voltage_timer = 0;
    }
    else if (_decrement_counter <= 0)
    {
        // controls how long the DAC writes '_max_dac_value' before decrementing again
        if (_write_high == 0)
        {
            _write_high = 1;
            _high_voltage_timer = 0;
            analogWrite(_dac_pin, _max_dac_value);
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
    float frequency_potentiometer = analogRead(_time_between_decrements_pin);
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
    int new_voltage_offset_value = analogRead(_voltage_offset_read_pin);
    analogWrite(_voltage_offset_write_pin, new_voltage_offset_value);
}

/*
  Controls the amplitude of the sawtooth waveform.
*/
void CarltonHapticMotor::_update_max_dac_value()
{
    int new_max_dac_value = analogRead(_max_voltage_pin);
    _max_dac_value = new_max_dac_value;
}

/*
  Sends a pulse to the motor and logs the subsequent acceleration data.
*/
void CarltonHapticMotor::pulse_motor(AccelerometerICM20649 imu, float intensity, unsigned long on_time)
{
    float pulse_intensity = intensity * _max_dac_value;
    _pulse_timer = 0;
    while (_pulse_timer < on_time)
    {
        analogWrite(_dac_pin, pulse_intensity);
        sensors_event_t acceleration_event = imu.read_accelerometer();
        Serial.print(((float)acceleration_event.timestamp) / ((float)1000));
        Serial.print(",");
        print_acceleration(acceleration_event);
    }
    zero();
}

/*
  Prints the measured acceleration in the direction of movement.
*/
void CarltonHapticMotor::print_acceleration(sensors_event_t acceleration_event)
{
    float acceleration_x = acceleration_event.acceleration.x;
    Serial.println(acceleration_x / SENSORS_GRAVITY_EARTH);
}

/*
  Sets the offset voltage and dac write value to the midpoint of the write values to stop the motor.
*/
void CarltonHapticMotor::zero()
{
    analogWrite(_dac_pin, _motor_zero_value);
    analogWrite(_voltage_offset_write_pin, _motor_zero_value);
}
