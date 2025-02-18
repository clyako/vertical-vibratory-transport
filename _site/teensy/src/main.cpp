/*
 * NOTE: barebones code to get the device from the paper running. Please note that this code has been updated and cleaned up WITHOUT retesting on
 * the device, as the device is no longer in service. If you need help implementing this code, please contact clyako@stanford.edu.
 *
 * HIGH LEVEL: this code controls the current that flows through the motor. The circuit ensures that a specified voltage corresponds to a
 * specific current (assuming a sufficient power supply that can handle the inductive spikes). The user can interact with several potentiometers
 * that control the amplitude, frequency, and voltage offset of a sawtooth current waveform. We determined experimentally that a sawtooth waveform
 * worked to achieve vertical transport of various grasped objects, but there are probably other waveforms that will work (and are more optimal).
 * However, the complex dynamics (impacts, nonlinear magnetic spring) made it difficult to determine the optimal experimental waveform.
 */

#include <Arduino.h>
#include <Bounce.h>
#include "CarltonHapticMotor.h"

void check_switch();

/*
 * Initializing the Carlton Haptic Motor
 */
CarltonHapticMotor motor = CarltonHapticMotor();

/*
 * Initializing the switch debounce to control if the device is on or off
 */
const int rocker_pin = A14;
int debounce_time = 5; // ms
Bounce rocker_switch = Bounce(rocker_pin, debounce_time);

/*
 * additional variables to control printing flow
 */
bool run_device;

void setup()
{
  Serial.begin(115200);
  pinMode(rocker_pin, INPUT_PULLUP);
  delay(1000);
  run_device = !digitalRead(rocker_pin);
}

void loop()
{
  check_switch();
  if (run_device)
  {
    unsigned long time_on_high = 1;
    motor.sawtooth(time_on_high);
  }
  else
  {
    motor.zero();
  }
}

/*
  This function checks the status of the rocker switch. The rocker switch determines when the
  sawtooth waveform is sent to the motor.
*/
void check_switch()
{
  if (rocker_switch.update())
  {
    if (rocker_switch.fallingEdge())
    {
      run_device = true;
    }
    else if (rocker_switch.risingEdge())
    {
      run_device = false;
    }
  }
}
