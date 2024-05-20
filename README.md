# Arduino Frequency Detector

## Project Description:

This project involves the development of an Arduino-based frequency detector for a simple embedded system, aimed at creating an industrial system divided into multiple subsystems. Each group works independently to develop a subsystem, which will be integrated and tested with those of other groups. The project is programmed within a host-target environment, using two platforms: ClearSy SK0 and Arduino Due.

## Objectives:

1. **Create a Simple Embedded Application**:
   - Develop an application using host-target programming on Arduino Due.
   
2. **Study Safety Solutions**:
   - Explore specific safety solutions integrated into one of the platforms.
   
3. **Evaluate Platform Usability**:
   - Assess the comparative difficulty of using ClearSy SK0 and Arduino Due.

## Project Content:

The project consists of an Arduino script designed to detect frequency and duty cycle within specified limits. It includes definitions for minimum and maximum frequency and duty cycle, tolerance values, PIN configurations, and a Finite State Machine (FSM) implementation to recognize the configured frequency.

## Pulse Width Modulation (PWM):

PWM is a repeatable (periodic) square wave signal with two edges:
- **Rising edge**
- **Falling edge**

### PWM Description Methods:
1. **Time-Based Description**:
   - Period (T) and Pulse Width (PW or T_ON)
   
2. **Frequency-Based Description**:
   - Frequency (f = 1/T) and Duty Cycle (d = PW/T)

### Edge Detection:
- **Rising Edge**: Current state is high, previous state was low.
- **Falling Edge**: Current state is low, previous state was high.

### PWM Recognition:
1. Time between two rising (or falling) edges equals period T.
2. Time between a rising edge and a falling edge equals pulse width PW.

### Real-World Considerations:
- Clocks generating and recognizing waveforms are not perfect.
- Frequency may vary due to environmental factors, e.g., temperature.
- Tolerances must be considered.

## FSM for PWM Detection:

1. **UNCOUPLED**:
   - Wait for valid T and PW.
   - Transition to COUPLING upon validation.
   
2. **COUPLING**:
   - If T or PW are invalid, return to UNCOUPLED.
   - Otherwise, wait for another valid T and PW to transition to COUPLED and set the output high (ON).

3. **COUPLED**:
   - Stay in this state as long as T and PW are valid.
   - Return to UNCOUPLED if T or PW become invalid and set the output low (OFF).

## 2oo3 Voter System:

- **Majority Voting System**:
  - 3 inputs, 1 output.
  - Output is high when at least two inputs are active, low otherwise.
  - Suitable for systems with high safety requirements, ideal for ClearSy implementation.

## Usage:

1. **Setup**:
   - Connect Arduino Due to the host environment.
   - Configure the Arduino Due by providing frequency (in mHz) and duty cycle (in %) through the serial port.
   - The system initializes and configures the input and output PINs accordingly.

2. **Loop**:
   - The Arduino continuously loops to recognize the configured frequency and duty cycle.
   - Implements a Finite State Machine (FSM) to detect frequency based on the input signal.
   - FSM states include UNCOUPLED, COUPLING, and COUPLED.

3. **Functions**:
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

**Authors:** [Alessandro Bonciani](https://github.com/alebonch), [Elia Innocenti](https://github.com/eliainnocenti), [Francesco Taccetti](https://github.com/Tacce)

**Contacts:** [alessandro.bonciani@edu.unifi.it](mailto:alessandro.bonciani@edu.unifi.it), [elia.innocenti@edu.unifi.it](mailto:elia.innocenti@edu.unifi.it), [francesco.taccetti@edu.unifi.it](mailto:francesco.taccetti@edu.unifi.it)
