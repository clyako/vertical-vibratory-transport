#include "CarltonHapticMotor.h"

CarltonHapticMotor::CarltonHapticMotor(uint8_t pwm_pin, int pwm_res, uint8_t max_voltage_pin, uint8_t time_between_decrements_pin, uint8_t voltage_offset_read_pin, uint8_t voltage_offset_write_pin, uint8_t square_offset_low_read_pin)
{
    _pwm_pin = pwm_pin;
    _pwm_res = pwm_res;
    _max_pwm_value = pow(2, _pwm_res) - 1;
    _num_counts = pow(2, _pwm_res);
    _motor_zero_value = _max_pwm_value / 2;
    _max_voltage_pin = max_voltage_pin;
    _time_between_decrements_pin = time_between_decrements_pin;
    _voltage_offset_read_pin = voltage_offset_read_pin;
    _voltage_offset_write_pin = voltage_offset_write_pin;
    _square_offset_low_read_pin = square_offset_low_read_pin;

    analogWriteResolution(_pwm_res);
    pinMode(_pwm_pin, OUTPUT);
    pinMode(_voltage_offset_read_pin, INPUT);
    pinMode(_voltage_offset_write_pin, OUTPUT);
    zero();
}

void CarltonHapticMotor::offset_square()
{
    _update_offset_square_waveform();

    if ((float)_offset_square_high_timer >= _offset_square_time_on_high && _offset_square_write_high)
    {
        _offset_square_write_high = false;
        _offset_square_low_timer = 0;
        // write the value to the low value
        analogWrite(_pwm_pin, _offset_square_low_value);
    }
    else if ((float)_offset_square_low_timer >= _offset_square_time_on_low && !_offset_square_write_high)
    {
        _offset_square_write_high = true;
        _offset_square_high_timer = 0;
        // write the value to the high value
        analogWrite(_pwm_pin, _offset_square_high_value);
    }
}

void CarltonHapticMotor::_update_offset_square_waveform()
{
    // frequency
    float frequency_potentiometer = analogRead(_time_between_decrements_pin);
    float new_waveform_frequency = (1 - frequency_potentiometer / _num_counts) * (_offset_square_max_frequency - _offset_square_min_frequency) + _offset_square_min_frequency;

    // duty cycle
    float duty_cycle_raw = analogRead(_voltage_offset_read_pin);
    float new_duty_cycle = duty_cycle_raw / _num_counts;

    // set new times
    _offset_square_time_on_high = new_duty_cycle / new_waveform_frequency * 1000000;
    _offset_square_time_on_low = (1 - new_duty_cycle) / new_waveform_frequency * 1000000;

    // set offset to midpoint
    analogWrite(_voltage_offset_write_pin, (float)_num_counts / 2);
    float offset_voltage = _max_pwm_pin_write_voltage / 2;

    float positive_acceleration_voltage = analogRead(_max_voltage_pin) / (float)_num_counts * _max_diff_amplifier_voltage;
    float negative_acceleration_voltage = analogRead(_square_offset_low_read_pin) / (float)_num_counts * _max_diff_amplifier_voltage;
    _offset_square_high_value = (offset_voltage - positive_acceleration_voltage / _diff_amp_gain) * _num_counts / _max_pwm_pin_write_voltage;
    _offset_square_low_value = (offset_voltage + negative_acceleration_voltage / _diff_amp_gain) * _num_counts / _max_pwm_pin_write_voltage;
}

float CarltonHapticMotor::sawtooth(unsigned long time_on_high, float time_between_decrements)
{
    if (time_between_decrements != -1)
    {
        _time_between_decrements = time_between_decrements;
    }
    else
    {
        _update_sawtooth_frequency();
    }
    _update_max_pwm_value();
    _update_voltage_offset();

    float pwm_write_value = 0;
    if ((float)_decrement_voltage_timer >= _time_between_decrements && _decrement_counter > 0)
    {
        _decrement_counter = _decrement_counter - _decrement_amount;
        pwm_write_value = _decrement_counter;
        analogWrite(_pwm_pin, _decrement_counter);
        _decrement_voltage_timer = 0;
    }
    else if (_decrement_counter <= 0)
    {
        if (_write_high == 0)
        {
            _write_high = 1;
            _high_voltage_timer = 0;
            analogWrite(_pwm_pin, _max_pwm_value);
        }
        if (_high_voltage_timer >= time_on_high)
        {
            _write_high = 0;
            _decrement_counter = (float)_max_pwm_value;
        }
        pwm_write_value = _max_pwm_value;
    }
    return pwm_write_value;
}

void CarltonHapticMotor::_update_sawtooth_frequency()
{
    float frequency_potentiometer = analogRead(_time_between_decrements_pin);
    float new_waveform_frequency = (1 - frequency_potentiometer / _num_counts) * (_sawtooth_max_frequency - _sawtooth_min_frequency) + _sawtooth_min_frequency;
    float microseconds_per_cycle = 1000000 / new_waveform_frequency;
    float decrements_per_cycle = microseconds_per_cycle / _time_between_decrements;
    _decrement_amount = _max_pwm_value / decrements_per_cycle;
}

void CarltonHapticMotor::_update_voltage_offset()
{
    int new_voltage_offset_value = analogRead(_voltage_offset_read_pin);
    analogWrite(_voltage_offset_write_pin, new_voltage_offset_value);
}

void CarltonHapticMotor::_update_max_pwm_value()
{
    int new_max_pwm_value = analogRead(_max_voltage_pin);
    _max_pwm_value = new_max_pwm_value;
}

void CarltonHapticMotor::pulse_motor(AccelerometerICM20649 imu, float intensity, unsigned long on_time)
{
    float pulse_intensity = intensity * _max_pwm_value;
    _pulse_timer = 0;
    while (_pulse_timer < on_time)
    {
        analogWrite(_pwm_pin, pulse_intensity);
        sensors_event_t acceleration_event = imu.read_accelerometer();
        Serial.print(((float)acceleration_event.timestamp) / ((float)1000));
        Serial.print(",");
        print_acceleration(acceleration_event);
    }
    zero();
}

void CarltonHapticMotor::print_acceleration(sensors_event_t acceleration_event)
{
    float acceleration_x = acceleration_event.acceleration.x;
    Serial.println(acceleration_x / SENSORS_GRAVITY_EARTH);
}

void CarltonHapticMotor::zero()
{
    analogWrite(_pwm_pin, _motor_zero_value);
    analogWrite(_voltage_offset_write_pin, _motor_zero_value);
}
