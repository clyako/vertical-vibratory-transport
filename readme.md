# Vertical Vibratory Transport Using Impacts

## Overview
This repository contains the code used to run the gripper from our paper 'Vertical Vibratory Transport Using Impacts'. 

**Links:**  
📄 [Project Paper](#)  
🎥 [Demonstration Video](#)

---

## Table of Contents
- [Teensy Code](#teensy-code)
- [MATLAB Code](#matlab-code)
- [Bill of Materials (BOM)](#bill-of-materials)
- [Installation & Usage](#installation--usage)
- [Contributors](#contributors)
- [License](#license)

---

## Teensy Code
The `Teensy` directory contains firmware written in C++ for the Teensy 4.1 microcontroller. This code is responsible for:
- Controlling the voice coil actuator
- Reading encoder and sensor data
- Running real-time control loops at 10 kHz

### Dependencies
- [Arduino IDE](https://www.arduino.cc/en/software) with Teensyduino
- [Bounce2 Library](https://github.com/thomasfredericks/Bounce2)
- Other relevant dependencies...

### Usage
Upload the code to the Teensy using the Arduino IDE or PlatformIO. Ensure all required libraries are installed.

---

## MATLAB Code
The `MATLAB` directory contains scripts for:
- Processing experimental data
- Generating plots
- Analyzing system performance

### Key Scripts
- `data_logger.m`: Logs and processes real-time data from the Teensy.
- `plot_response.m`: Visualizes system response.
- `analyze_performance.m`: Computes tracking error and other key metrics.

---

## Bill of Materials
A detailed list of components used in this project:

| Item | Description | Quantity | Link |
|------|------------|----------|------|
| Teensy 4.1 | Microcontroller | 1 | [Buy](https://www.pjrc.com/store/teensy41.html) |
| Voice Coil Actuator | Linear actuator | 1 | [Supplier](#) |
| Pololu G2 24v13 | Motor Driver | 1 | [Buy](https://www.pololu.com/product/2992) |
| Encoder | POSIC Incremental Encoder | 1 | [Supplier](#) |
| PLA Gears | 3D-printed gears | Varies | Custom |

For a full parts list, see the `BOM.csv` file in the repository.

---

## Installation & Usage
1. Clone this repository:
   ```sh
   git clone https://github.com/your_username/your_project.git
   cd your_project
   ```
2. Install required dependencies for Teensy and MATLAB.
3. Follow individual instructions in the `Teensy` and `MATLAB` sections above.
4. Run MATLAB scripts to process and visualize data.

---

## Contributors
- **[Your Name]** - Initial Development
- **Other Contributors** - Additional Work

Feel free to contribute by submitting pull requests!

---

## License
This project is licensed under the [MIT License](LICENSE).

