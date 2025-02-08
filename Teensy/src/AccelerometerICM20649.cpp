#include "AccelerometerICM20649.h"

AccelerometerICM20649::AccelerometerICM20649(int baud_rate,
                                             uint8_t cs_pin,
                                             uint8_t fchoice,
                                             uint8_t dlpf_bw,
                                             uint16_t accel_rate_divisor,
                                             icm20649_accel_range_t accelerometer_range)
{
    _baud_rate = baud_rate;
    _cs_pin = cs_pin;
    _fchoice = fchoice;
    _dlpf_bw = dlpf_bw;
    _accel_rate_divisor = accel_rate_divisor;
    _accelerometer_range = accelerometer_range;
}

sensors_event_t AccelerometerICM20649::read_accelerometer()
{
    _imu.getAccelEvent(&_accel);
    return _accel;
}

float AccelerometerICM20649::read_acceleration_1D(int dim)
{
    _imu.getAccelEvent(&_accel);
    float acceleration = 0;
    if (dim == 1)
    {
        acceleration = _accel.acceleration.x;
    }
    else if (dim == 2)
    {
        acceleration = _accel.acceleration.y;
    }
    else if (dim == 3)
    {
        acceleration = _accel.acceleration.z;
    }
    return acceleration;
}

void AccelerometerICM20649::initialize_serial()
{
    Serial.begin(_baud_rate);
    while (!Serial)
    {
        delay(10);
    }
}

void AccelerometerICM20649::initialize_imu()
{
    if (!_imu.begin_SPI(_cs_pin))
    {
        Serial.println("Failed to find ICM20649");
        while (1)
        {
            delay(10);
        }
    }
}

void AccelerometerICM20649::check_accelerometer_range()
{
    Serial.print("Accelerometer range set to: ");
    switch (_imu.getAccelRange())
    {
    case ICM20649_ACCEL_RANGE_4_G:
        Serial.println("+-4G");
        break;
    case ICM20649_ACCEL_RANGE_8_G:
        Serial.println("+-8G");
        break;
    case ICM20649_ACCEL_RANGE_16_G:
        Serial.println("+-16G");
        break;
    case ICM20649_ACCEL_RANGE_30_G:
        Serial.println("+-30G");
        break;
    }
}

void AccelerometerICM20649::set_accelerometer_range()
{
    _imu.setAccelRange(_accelerometer_range);
}

void AccelerometerICM20649::get_dlpf_bandwidth()
{
    uint8_t dlpf_bw_config = _imu.getBandwidthDLPF();
    Serial.print("The DLPF bandwidth is: ");
    switch (dlpf_bw_config)
    {
    case 0:
        Serial.println("246.0 Hz");
        break;
    case 1:
        Serial.println("246.0 Hz");
        break;
    case 2:
        Serial.println("111.4 Hz");
        break;
    case 3:
        Serial.println("50.4 Hz");
        break;
    case 4:
        Serial.println("23.9 Hz");
        break;
    case 5:
        Serial.println("11.5 Hz");
        break;
    case 6:
        Serial.println("5.7 Hz");
        break;
    case 7:
        Serial.println("473 Hz");
        break;
    }
}

void AccelerometerICM20649::check_dlpf_config()
{
    uint8_t dlpf_state = _imu.getStateDLPF();
    if (dlpf_state == 1)
    {
        Serial.println("DLPF is enabled");
        get_dlpf_bandwidth();
    }
    else
    {
        Serial.println("DLPF is disabled");
        Serial.println("The DLPF bandwidth is: 1209 Hz");
    }
}

void AccelerometerICM20649::configure_dlpf()
{
    _imu.setAccelDLPF(_fchoice);
    _imu.setAccelBandwidthDLPF(_dlpf_bw);
}

float AccelerometerICM20649::calculate_output_data_rate()
{
    unsigned long t1 = 0;
    unsigned long t2 = 0;
    float prev_reading = 0;
    int counter = 0;
    int number_of_samples = 10000;
    float sampled_sum = 0;

    while (counter < number_of_samples)
    {
        _imu.getAccelEvent(&_accel);
        float current_reading = _accel.acceleration.x;
        if (current_reading != prev_reading)
        {
            t1 = micros();
            float data_rate = 1 / ((float)(t1 - t2) / 1000000);
            sampled_sum += data_rate;
            t2 = t1;
            prev_reading = current_reading;
            counter++;
        }
    }
    float average_data_rate = sampled_sum / number_of_samples;
    return average_data_rate;
}

void AccelerometerICM20649::check_accelerometer_output_rate_divisor()
{
    float calculated_output_rate = calculate_output_data_rate();
    if (_fchoice == 0)
    {
        Serial.print("The output rate should be: 4500 Hz");
        Serial.print(", but it is actually ");
        Serial.print(calculated_output_rate);
        Serial.println(" Hz");
    }
    else
    {
        Serial.print("The output rate should be: ");
        Serial.print(1125 / (_imu.getAccelRateDivisor() + 1));
        Serial.print(", but it is actually ");
        Serial.print(calculated_output_rate);
        Serial.println(" Hz");
    }
}

void AccelerometerICM20649::set_accelerometer_output_rate()
{
    _imu.setAccelRateDivisor(_accel_rate_divisor);
}

void AccelerometerICM20649::perform_settings_checks()
{
    check_accelerometer_range();
    check_dlpf_config();
    check_accelerometer_output_rate_divisor();
}

void AccelerometerICM20649::init()
{
    initialize_serial();
    initialize_imu();
    set_accelerometer_range();
    configure_dlpf();
    set_accelerometer_output_rate();
}