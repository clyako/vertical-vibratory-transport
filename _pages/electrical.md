---
title: Electrical
permalink: /electrical/
---

The electrical system for the vertical vibratory transport mechanism integrates high-current motor drivers, feedback sensors, and a reliable power distribution network.

## Components
- **Teensy Microcontroller:** Core control unit managing the PID loop and sensor inputs.
- **Motor Drivers:** High-power drivers capable of handling the rapid acceleration cycles required for vertical transport.
- **Current Sensors:** Provide feedback for closed-loop current control, ensuring consistent performance.
- **Power Supply:** Delivers sufficient voltage and current to support high-frequency oscillations without voltage sag.

## Wiring Diagram
A detailed wiring diagram is available in the repository’s `/electrical` folder, showing all necessary connections between the Teensy, motor drivers, sensors, and power supply.

## Best Practices
- **Noise management:** Shielded cables and proper grounding are essential to minimize electrical noise, particularly at high PWM frequencies.
- **Heat dissipation:** Motor drivers and power components may require heat sinks or active cooling to avoid thermal shutdown.

[Back to Overview](index.md)
