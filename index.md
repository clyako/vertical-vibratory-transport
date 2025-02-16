---
title: Vertical Vibratory Transport Using Impacts
layout: default
---

# Vertical Vibratory Transport Using Impacts

![Gripper in Action](assets/gripper_loop_video.gif)

## Overview
This repository contains the Teensy code used to run the gripper from our paper _Vertical Vibratory Transport Using Impacts_. It also contains the mechanical and electrical design files as well as the MATLAB code used to verify our dynamical model. Links to the paper (Arxiv) and the accompanying video presentation can be found at the links below.

**Links:**
- [IEEE paper](https://ieeexplore.ieee.org/document/10610769) and [Arxiv paper](https://arxiv.org/abs/2502.05693)
- [Full video presentation](https://youtu.be/Mb02fUOyaTE) and [gripper demo only](https://youtu.be/99dn1M9muk4)

The goal of this project was to use a vibrating _surface_ to transport a grasped _part_ vertically against gravity. Manipulation via vibration is not a new concept, in fact, many of the papers cited in our work go back decades. However, most of this transport occurred in the plane or at a slight angle to the plane (think [vibratory part feeders](https://www.youtube.com/watch?v=E0WLpJ0FyaU)). They essentially work using an alternating sticking and slipping waveform, where the part is transported forward while sticking to the moving surface, and then the moving surface slips behind the part before repeating the process. While this concept can be applied in our case, the dynamics of vertical vibratory transport make the process much more challenging compared to its horizontal (or near horizontal) counterpart. Gravity tends to skew the friction cone (it is symmetric in the horizontal case), limiting the peak accelerations during sticking and necessitating more surface acceleration to instigate slip. The effect is slower average part velocities and the need for more powerful actuators that can quickly achieve large accelerations. To meet this need we used impact induced accelerations. We verified our dynamical model by feeding the tracked surface position into a Simulink model of our theorized dynamics and confirmed our hypothesized design guidelines by building a prototype gripper to demonstrate the practicality of the working principle.

---

## Table of Contents
- [Overview](index.md)
- [Working Principle](pages/working-principle.md)
- [Software](pages/software.md)
- [Electrical](pages/electrical.md)
- [Mechanical](pages/mechanical.md)

---

## Disclaimer
The Teensy code that is uploaded here is a cleaned up version of what was actually run on the gripper for the paper, and has not been tested on the actual gripper since it is no longer in service. However, if you do run this code please reach out to me at connor.yako@stanford.edu and I am happy to help you get things up and running. If you do run the device, PLEASE WEAR HEARING PROTECTION! The device can be incredibly loud, especially at higher currents. Additionally, running the device at too high of currents for too long can melt the plastic housing of the motors, locking the magnetic ram in place.
