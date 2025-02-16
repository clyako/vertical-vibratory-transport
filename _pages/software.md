---
title: Software
permalink: /software/
---

# Software

The software controlling the vertical vibratory transport system is built around a Teensy microcontroller, leveraging high-speed PWM and real-time feedback for precise motor control.

## Control Architecture
- **PID control:** The system uses a PID loop to maintain surface acceleration within target parameters.
- **Feedback sensors:** Position and current sensors provide real-time feedback, allowing for adaptive control.
- **Frequency modulation:** To optimize transport velocity, the control software dynamically adjusts vibration frequency based on the system’s current operating conditions.

## Code Structure
- **Main loop:** Handles sensor input, PID calculations, and motor control output.
- **Interrupt routines:** Manage timing-critical tasks, such as encoder readings and PWM updates.
- **Configuration files:** Allow users to easily adjust system parameters, including PID gains, vibration frequency, and acceleration limits.

You can find the complete Teensy code in the repository's `/src` folder.

[Back to Overview](index.md)
