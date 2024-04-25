#include <Arduino.h>
#include <Adafruit_ICM20649.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Bounce.h>
#include "AccelerometerICM20649.h"
#include "CarltonHapticMotor.h"

// #define PULSE
// #define MANUAL_PING
#define SAWTOOTH
// #define OFFSET_SQUARE

#if defined(PULSE) || defined(MANUAL_PING)
#define IMU
#endif

void manual_ping();
void pulse_data_collection(float intensity, float on_time);
void check_switch();
void asymmetric_waveform_interrupt_service_routine();
void offset_square_interrupt_service_routine();

/*
 * Initializing the IMU
 */
#ifdef IMU
int baud_rate_ = 115200;                                                 // baud rate
uint8_t cs_pin_ = 10;                                                    // chip select pin
uint8_t fchoice_ = 0x01;                                                 // enable or disable DLPF
uint8_t dlpf_bw_ = 0x07;                                                 // sets the bandwidth for the DLPF (if enabled)
uint16_t accel_rate_divisor_ = 0;                                        // if fchoice is set to zero (disabled), this determines the ODR = 1.125 kHz / (1 + accel_rate_divisor)
icm20649_accel_range_t accelerometer_range_ = ICM20649_ACCEL_RANGE_30_G; // +/- 30G measurement range
AccelerometerICM20649 imu = AccelerometerICM20649(baud_rate_, cs_pin_, fchoice_, dlpf_bw_, accel_rate_divisor_, accelerometer_range_);
#endif

/*
 * Initializing the Carlton Haptic Motor
 */
uint8_t pwm_pin = A21;
uint8_t pwm_res = 10; // bits
uint8_t max_voltage_pin = A15;
uint8_t time_between_decrements_pin = A16;
uint8_t voltage_offset_read_pin = A17;
uint8_t square_offset_low_read_pin = A18;
uint8_t voltage_offset_write_pin = A19;
CarltonHapticMotor motor = CarltonHapticMotor(pwm_pin, pwm_res, max_voltage_pin, time_between_decrements_pin, voltage_offset_read_pin, voltage_offset_write_pin, square_offset_low_read_pin);

/*
 * Initializing the switch debounce to control data logging
 */
const int rocker_pin = A14;
int debounce_time = 5; // ms
Bounce rocker_switch = Bounce(rocker_pin, debounce_time);

/*
 * additional variables to control printing flow
 */
bool print_values;
bool printing = false;
IntervalTimer sawtooth_timer;
IntervalTimer square_offset_timer;

void setup()
{
#ifdef IMU
  imu.init(); // enables Serial, SPI, and writes specified IMU settings to appropriate registers
#else
  Serial.begin(115200);
#endif
  pinMode(rocker_pin, INPUT_PULLUP);
  delay(1000); // need this delay to get a correct read of the pin state
  print_values = !digitalRead(rocker_pin);
}

void loop()
{
#ifdef PULSE
  float intensity = 0.9;
  unsigned long on_time = 20000;             // ms
  pulse_data_collection(intensity, on_time); // to get a acceleration vs. time plots (need to run the python program as well)
#endif

#ifdef MANUAL_PING
  manual_ping();
#endif

#if defined(SAWTOOTH) || defined(OFFSET_SQUARE)
  check_switch();
  if (print_values)
  {
#ifdef SAWTOOTH
    unsigned long time_on_high = 1;
    motor.sawtooth(time_on_high);
#endif
#ifdef OFFSET_SQUARE
    motor.offset_square();
#endif
  }
  else
  {
    motor.zero();
  }
#endif
}

/*
  Allows the user to manually displace the moving platform, and then capture the acceleration data. Use the rocker switch
  to start the data collection, and then once the platform has stopped moving flip it back to stop data collection.
*/
void manual_ping()
{
  check_switch();
  if (print_values)
  {
    if (!printing)
    {
      printing = true;
    }
#ifdef IMU
    sensors_event_t acceleration_event = imu.read_accelerometer();
    Serial.print(acceleration_event.timestamp / 1000);
    Serial.print(",");
    motor.print_acceleration(acceleration_event);
#endif
  }
  else if (!print_values && printing)
  {
    // this tells the python data logging script ('data_logger.py') when to stop collecting data
    Serial.println(".");
    printing = false;
  }
}

/*
  Sends a pulse of specified strength (0.0 ≤ intensity ≤ 1.0) for a certain number of milliseconds (on_time). The pulse
  is sent once the user flips the rocker switch. IMU readings will be printed once (and while) the pulse is being sent,
  and will stop printing once the user flips the rocker switch back.
*/
void pulse_data_collection(float intensity, float on_time)
{
  check_switch();
  if (print_values)
  {
    if (!printing)
    {
#ifdef IMU
      motor.pulse_motor(imu, intensity, on_time);
      printing = true;
#endif
    }
  }
  else if (!print_values && printing)
  {
    // this tells the python data logging script ('data_logger.py') when to stop collecting data
    Serial.println(".");
    printing = false;
  }
}

/*
  This function to check the status of the rocker switch. The rocker switch is used to control when data collection
  starts and stops for the functions 'pulse_data_collection' and 'manual_ping'. The rocker switch also determines when the
  sawtooth or offset square wave is sent to the motor.
*/
void check_switch()
{
  if (rocker_switch.update())
  {
    if (rocker_switch.fallingEdge())
    {
      print_values = true;
    }
    else if (rocker_switch.risingEdge())
    {
      print_values = false;
    }
  }
}
