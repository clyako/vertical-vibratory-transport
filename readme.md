# Vertical Vibratory Transport Using Impacts

## Overview
This repository contains the Teensy code used to run the gripper from our paper _Vertical Vibratory Transport Using Impacts_. It also contains the mechanical and electrical design files as well as the MATLAB code used to verify our dynamical model. Links to the paper (Arxiv) and the accompanying video presentation can be found at the links below.

**Links:**
- [IEEE paper](https://ieeexplore.ieee.org/document/10610769) and [Arxiv paper](https://arxiv.org/abs/2502.05693)
- [Full video presentation](https://youtu.be/Mb02fUOyaTE) and [gripper demo only](https://youtu.be/99dn1M9muk4)

The goal of this project was to use a vibrating _surface_ to transport a grasped _part_ vertically against gravity. Manipulation via vibration is not a new concept, in fact, many of the papers cited in our work go back decades. However, most of this transport occurred in the plane or at a slight angle to the plane (think [vibratory part feeders](https://www.youtube.com/watch?v=E0WLpJ0FyaU)). They essentially work using an alternating sticking and slipping waveform, where the part is transported forward while sticking to the moving surface, and then the moving surface slips behind the part before repeating the process. While this concept can be applied in our case, the dynamics of vertical vibratory transport make the process much more challenging compared to its horizontal (or near horizontal) counterpart. Gravity tends to skew the friction cone (it is symmetric in the horizontal case), limiting the peak accelerations during sticking and necessitating more surface acceleration to instigate slip. The effect is slower average part velocities and the need for more powerful actuators that can quickly achieve large accelerations. To meet this need we used impact induced accelerations. We verified our dynamical model by feeding the tracked surface position into a Simulink model of our theorized dynamics and confirmed our hypothesized design guidelines by building a prototype gripper to demonstrate the practicality of the working principle.

## Disclaimer
The Teensy code that is uploaded here is a cleaned up version of what was actually run on the gripper for the paper, and has not been tested on the actual gripper since it is no longer in service. However, if you do run this code please reach out to me at connor.yako@stanford.edu and I am happy to help you get things up and running. If you do run the device, PLEASE WEAR HEARING PROTECTION! The device can be incredibly loud, especially at higher currents. Additionally, running the device at too high of currents for too long can melt the plastic housing of the motors, locking the magnetic ram in place.

---

## Table of Contents
- [Teensy Code](#teensy-code)
- [MATLAB Code](#matlab-code)
- [Design Files](#design-files)
- [Contributors](#contributors)

---

## Teensy Code
The `Teensy` folder contains the C++ code for the Teensy 3.6 microcontroller, however, a Teensy 4.1 with an appropriate external DAC can also be used. This code is responsible for:
- Driving the [Carlton haptic actuator](https://titanhaptics.com/wp-content/uploads/2023/01/TacHammer-Carlton-Datasheet.pdf)
- Updating the drive signal based on user input via several potentiometers

The user controls the amplitude, frequency, and vertical offset of a [sawtooth waveform](https://en.wikipedia.org/wiki/Sawtooth_wave#/media/File:Waveforms.svg), which directly corresponds to the current flowing through the actuators (assuming a power supply that can handle the inductive spikes). In theory, to achieve the alternating sticking and slipping waveform an asymmetric square wave should be sent. However, because of the complex dynamics involving impacts and a nonlinear magnetic suspension it is not this straightforward. We experimentally determined that a sawtooth waveform seemed to produce transport, but there are likely more optimal waveforms to use.

The code was written in Visual Studio with [Platformio](https://platformio.org/).

### Usage
Be sure to update the pins that you use for the potentiometers, DAC (if using Teensy3.6), and the switch that controls whether current is sent to the motor or not. Pins for the potentiometers and DAC can be found at the top of `CarltonHapticMotor.h` (see the `#define` statements). The pin for the switch is called `rocker_pin` and can be found in `main.cpp`.

---

## MATLAB Code
The `MATLAB` folder contains both MATLAB and Simulink files for:
- Simulating part motion in response to the experimentally measured surface motion (and comparing to experimentally measured part motion)
- Simulating part motion in response to the theoretical optimal surface motion (and comparing it to experimentally measured part motion
- Fitting parameters (normal force and coefficient of kinetic friction) to experimentally measured data using particle swarm optimization

Experimental data across 10 trials can be found in the `data` folder. Surface and part positions were tracked using the free software [Tracker](https://physlets.org/tracker/), and the `.csv` files contain the measured position, and calculated velocity and acceleration data for each trial.

The dynamical model can be seen in `stick_slip.m`, which is called by each Simulink file. Please see the files `parameter_estimation.m` and `experimental_vs_optimal_comparison.m` for details on which sections to run to see various outputs.

---

## Design Files
The circuit schematic and `.step` files for the CAD can be found in the `design_files` folder.

### Circuit
A benchtop power supply that could supply at least ±18V, a power op amp (OPA548), a standard op amp (LM348), a 4 Ohm power resistor, several rotary potentiometers, and a rocker switch were used, in addition to a variety of capacitors and 5% tolerance resistors. Be sure to also add a heatsink to the power op amp. In can be useful to scope the voltage of the power resistor to confirm that the desired current waveform is flowing through the motor.

### CAD
The gripper design is simply a quick prototype of a parallel jaw gripper. It is made from laser cut birch plywood and some 3D printed PLA parts. The dowel pins, screws, and various holes may have to be adjusted based on what you have access to / on hand. When printing the flexures, be sure to use a small layer height (we used 0.1 mm). You can also play around with their geometry to get different behavior of the moving surfaces. The gripper is meant to be operated by hand, which one hand holding the gripper and the other turning the dial / gear to adjust the normal force.

---

## Contributors
- **Connor Yako**
- **Jérôme Nowak**
- **Shenli Yuan**
- **Kenneth Salisbury**

