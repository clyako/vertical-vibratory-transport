---
layout: single
title: Implementation
permalink: /docs/implementation/
---

## Electrical

The moving surfaces are driven by [Carlton Haptic Actuators]({{ site.baseurl }}/assets/files/Carlton-Datasheet.pdf), a type of voice coil actuator. Since the generated force is proportional to the current through the coil, it is preferable to control the motor’s current rather than the voltage across its terminals. To do this, we used the current-drive circuit described by McMahan and Kuchenbecker[^1].

![image-center]({{ site.baseurl }}/assets/images/kuchenbecker-current-voltage-drive.png){: .align-center}

### Schematic

The full circuit schematic is shown below and available as a PDF [here]({{ site.baseurl }}/assets/files/circuit-schematic.pdf).

![image-center]({{ site.baseurl }}/assets/images/schematic.png){: .align-center}
### Components

- **Teensy Microcontroller:** Core control unit managing the PID loop and sensor inputs.
- **Motor Drivers:** High-power drivers capable of handling the rapid acceleration cycles required for vertical transport.
- **Current Sensors:** Provide feedback for closed-loop current control, ensuring consistent performance.
- **Power Supply:** Delivers sufficient voltage and current to support high-frequency oscillations without voltage sag.

### Wiring Diagram

A detailed wiring diagram is available in the repository’s `/electrical` folder, showing all necessary connections between the Teensy, motor drivers, sensors, and power supply.

### Best Practices

- **Noise management:** Shielded cables and proper grounding are essential to minimize electrical noise, particularly at high PWM frequencies.
- **Heat dissipation:** Motor drivers and power components may require heat sinks or active cooling to avoid thermal shutdown.

[^1]: McMahan, W., & Kuchenbecker, K. J. (2014). Dynamic modeling and control of voice-coil actuators for high-fidelity display of haptic vibrations. *2014 IEEE Haptics Symposium (HAPTICS)*, 115-122. IEEE. [doi:10.1109/HAPTICS.2014.6775439](https://doi.org/10.1109/HAPTICS.2014.6775439)
