# Arduino Frequency Detector README

## Project Description:

This project involves the development of an Arduino-based frequency detector for a simple embedded system. The system is programmed within a host-target environment.

## Project Content:

The project consists of an Arduino script designed to detect frequency and duty cycle within specified limits. It includes definitions for minimum and maximum frequency and duty cycle, tolerance values, PIN configurations, and a Finite State Machine (FSM) implementation to recognize the configured frequency.

## Usage:

1. **Setup:**

   - Connect Arduino Due to the host environment.
   - Configure the Arduino Due by providing frequency (in mHz) and duty cycle (in %) through the serial port.
   - The system initializes and configures the input and output PINs accordingly.

2. **Loop:**

   - The Arduino continuously loops to recognize the configured frequency and duty cycle.
   - It implements a Finite State Machine (FSM) to detect frequency based on the input signal.
   - FSM states include UNCOUPLED, COUPLING, and COUPLED, each representing different stages of frequency detection.

3. **Functions:**
   - `printFrequencyRange`: Prints the allowed frequency range.
   - `printDutyCycleRange`: Prints the allowed duty cycle range.
   - `printConfig`: Prints the received configuration (frequency and duty cycle).
   - `configure`: Calculates minimum and maximum period values and configures the input and output PINs.

## Important Notes:

- Ensure proper configuration of the Arduino Due by providing valid frequency and duty cycle values within specified limits.
- The FSM implementation optimizes frequency detection and minimizes jitter.
- Customize the loop function as per project requirements to enhance functionality.

Feel free to modify the script to suit specific project needs or expand its capabilities.

For detailed instructions on setup and usage, refer to the project documentation.

---

**Author:** **Authors:** [Alessandro Bonciani](https://github.com/alebonch), [Elia Innocenti](https://github.com/eliainnocenti), [Francesco Taccetti](https://github.com/Tacce)
**Contact:** [alessandro.bonciani@edu.unifi.it](mailto:alessandro.bonciani@edu.unifi.it), [elia.innocenti@edu.unifi.it](mailto:elia.innocenti@edu.unifi.it), [francesco.taccetti@edu.unifi.it](mailto:francesco.taccetti@edu.unifi.it)
