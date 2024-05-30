// Embedded System Host-Target - Arduino Detector script

// us = MicroSeconds
// mHz = MicroHertz

// Minimum and maximum limits of frequency and duty cycle
#define MIN_FREQ                    (250)         /* Minimum frequency (in mHz) */
#define MAX_FREQ                    (100000)      /* Maximum frequency (in mHz) */
#define MIN_DUTY                    (10)          /* Minimum duty cycle (in %) */
#define MAX_DUTY                    (90)          /* Maximum duty cycle (in %) */

// Nanoseconds in one second (1 second = 10^9 nanoseconds)
#define NSEC_IN_SEC                 (1000000000)  /* Nanseconds in one second */

// Tolerances
#define TOLERANCE_FREQUENCY         (5)           /* Tolerance on frequency (per thousand) */
#define TOLERANCE_DUTY              (1)           /* Tolerance on duty cycle (percent) */

// Numerical constants
#define HUNDRED                     (100)         /* One hundred */
#define THOUSAND                    (1000)        /* One thousand */

// PINs used as input and output
#define INPUT_PIN                   (23)          /* PIN used as input */
#define OUTPUT_PIN                  (53)          /* PIN used as output */

// Definition of the FSM states as an enumerated set of values
enum fsm {
  UNCOUPLED = 0,            /* UNCOUPLED: I have not recognized any valid period yet */
  COUPLING,                 /*  COUPLING: I have recognized a single valid period, waiting for the second one */
  COUPLED                   /*   COUPLED: I have recognized at least two valid periods, the frequency is correctly recognized */
};

// State initialization
fsm currentState = UNCOUPLED;

// List of static variables (common to all functions)
static uint32_t frequency;  /* Frequency to be recognized (mHz) without tolerance */
static uint32_t dutyCycle;  /* Duty cycle to be recognized (%) without tolerance */
static uint32_t periodMin;  /* Minimum period (us) to be recognized T_min */
static uint32_t periodMax;  /* Maximum period (us) to be recognized T_max */
static uint32_t tOnMin;     /* Minimum pulse width (us) to be recognized TON_min */
static uint32_t tOnMax;     /* Maximum pulse width (us) to be recognized TON_max */

// List of static functions (accessible only from this .ino file)
static void printFrequencyRange(bool bInvalid);
static void printDutyCycleRange(bool bInvalid);
static void printConfig(void);
static void configure(void);

// Setup function: configure serial port, await parameters, save configuration, set up two I/O pins, close port
void setup() {
  String tmp;
  unsigned long value;
  int bValid;
  
  // Initialize serial and wait for port to open
  SerialUSB.begin(9600);
  SerialUSB.setTimeout(60000);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for native USB
  }

  SerialUSB.println("Configuring Frequency Detector...");
  printFrequencyRange(false);
  bValid = 0;
  do {
    tmp = SerialUSB.readStringUntil('\n');
    if (tmp != NULL) {
      value = strtoul(tmp.c_str(), NULL, 10);
      if (value >= MIN_FREQ && value <= MAX_FREQ) {
        frequency = (uint32_t)value;                              
        bValid = 1;
      }
      else {
        printFrequencyRange(true);
      }
    }
  } while (bValid == 0);
  printDutyCycleRange(false);
  bValid = 0;
  do {
    tmp = SerialUSB.readStringUntil('\n');
    if (tmp != NULL) {
      value = strtoul(tmp.c_str(), NULL, 10);
      if (value >= MIN_DUTY && value <= MAX_DUTY) {
        dutyCycle = (uint32_t)value;
        bValid = 1;
      }
      else {
        printDutyCycleRange(true);
      }
    }
  } while (bValid == 0);

  configure();
  printConfig();
  SerialUSB.end();   
}

// List of static variables used in loop
static uint32_t lastRisingEdgeTime = 0;
static uint32_t lastPeriodTime = 0;
static bool lastValidTon = false;
static bool previousSignalState = LOW;

// Loop function: implements the FSM to recognise the configured frequency 
void loop() {

  uint32_t currentTime = micros();
  bool signalState = digitalRead(INPUT_PIN);

  switch(currentState) {
          
    case UNCOUPLED:
      if (signalState != previousSignalState) {                                 // If the current state has changed compared to the previous one
        if (!signalState) {                                                     // If it's a falling edge
          uint32_t pulseWidth = currentTime - lastRisingEdgeTime;               
          if (pulseWidth >= tOnMin && pulseWidth <= tOnMax) {                   // Check if the TON is valid
            lastValidTon = true;
          } else {
            lastValidTon = false;
          }
        } else {                                                                // If it's a rising edge
          lastRisingEdgeTime = currentTime;                                     // Update the time of the last rising edge
          uint32_t period = currentTime - lastPeriodTime;                       // Update the period
          if (period >= periodMin && period <= periodMax && lastValidTon) {     // Check if the period is valid
            lastPeriodTime = currentTime;                                       // Update the last period time
            currentState = COUPLING;                                            // Go to the COUPLING state
          } else {
            lastValidTon = false;
          }
        }
      }
      break;

    case COUPLING:
    if (signalState == previousSignalState) {                                   // If the current state of the input has not changed compared to 
                                                                                // the previous one                                                   
      if ((signalState && (currentTime - lastRisingEdgeTime) > tOnMax) ||       // Check if the maximum value of TRON (if the current state is high) or 
          (!signalState && (currentTime - lastRisingEdgeTime) > periodMax)) {   // the maximum value of T (if it's low) has been exceeded
        lastValidTon = false;                                                   // Declare the last TON invalid
        currentState = UNCOUPLED;                                               // Go back to the UNCOUPLED state
      }
    } else {                                                                    // If the current state has changed
      if (!signalState) {                                                       // If it's a falling edge
        uint32_t pulseWidth = currentTime - lastRisingEdgeTime;
        if (!(pulseWidth >= tOnMin && pulseWidth <= tOnMax)) {                  // If the pulse width is not within TON_min and TON_max
          lastValidTon = false;                                                 // Declare the last TON invalid
          currentState = UNCOUPLED;                                             // Go back to the UNCOUPLED state
        }
      } else {                                                                  // If it's a rising edge
        uint32_t period = currentTime - lastRisingEdgeTime;
        if (period >= periodMin && period <= periodMax) {                       // If the period is within T_min and T_max
          currentState = COUPLED;                                               // Go to the COUPLED state
          digitalWrite(OUTPUT_PIN, HIGH);                                       // Turn on the output
        } else {
          lastValidTon = false;                                                 // Declare the last TON invalid
          currentState = UNCOUPLED;                                             // Go back to the UNCOUPLED state
        }
        lastRisingEdgeTime = currentTime;
      }
    }
    break;

    case COUPLED:
    if (signalState == previousSignalState) {                                   // If the current state of the input has not changed compared to 
                                                                                // the previous one
      if ((signalState && (currentTime - lastRisingEdgeTime) > tOnMax) ||       // Check if the maximum value of TON (if the current state is high) or 
          (!signalState && (currentTime - lastRisingEdgeTime) > periodMax)) {   // the maximum value of T (if it's low) has been exceeded
        lastValidTon = false;                                                   // Declare the last TON invalid
        currentState = UNCOUPLED;                                               // Go back to the UNCOUPLED state
        digitalWrite(OUTPUT_PIN, LOW);                                          // Turn off the output
      }
    } else {                                                                    // If the current state has changed
      if (!signalState) {                                                       // If it's a falling edge
        uint32_t pulseWidth = currentTime - lastRisingEdgeTime;
        if (!(pulseWidth >= tOnMin && pulseWidth <= tOnMax)) {                  // If the pulse width is not within TON_min and TON_max
          lastValidTon = false;                                                 // Declare the last TON invalid
          currentState = UNCOUPLED;                                             // Go back to the UNCOUPLED state
          digitalWrite(OUTPUT_PIN, LOW);                                        // Turn off the output
        }
      } else {                                                                  // If it's a rising edge
        uint32_t period = currentTime - lastRisingEdgeTime;
        lastRisingEdgeTime = currentTime;                                       //update last rising edge time because we had a rising edge
        if (!(period >= periodMin && period <= periodMax)) {                    // If the period is not within T_min and T_max
          lastValidTon = false;                                                 // Declare the last TON invalid
          currentState = UNCOUPLED;                                             // Go back to the UNCOUPLED state
          digitalWrite(OUTPUT_PIN, LOW);                                        // Turn off the output
        }
        lastRisingEdgeTime = currentTime;                                       // Update the time of the last rising edge
      }
    }
    break; 
          
  }
    
  previousSignalState = signalState; // Update the previous signal state
          
}

// Print the permissible range for the frequency
static void printFrequencyRange(bool bInvalid) {
  SerialUSB.print("  ");
  if (bInvalid != false) {
    SerialUSB.print("Invalid value. ");
  }
  SerialUSB.print("Provide frequency (mHz) [");
  SerialUSB.print(MIN_FREQ, DEC);
  SerialUSB.print("-");
  SerialUSB.print(MAX_FREQ, DEC);
  SerialUSB.println("]");
}

// Print the permissible duty cycle range 
static void printDutyCycleRange(bool bInvalid) {
  SerialUSB.print("  ");
  if (bInvalid != false) {
    SerialUSB.print("Invalid value. ");
  }
  SerialUSB.print("Provide duty cycle (%) [");
  SerialUSB.print(MIN_DUTY, DEC);
  SerialUSB.print("-");
  SerialUSB.print(MAX_DUTY, DEC);
  SerialUSB.println("]");
}

// Print received configuration (frequency and duty cycle)   
static void printConfig(void) {
  SerialUSB.println();
  SerialUSB.println("Frequency Detector configuration");
  SerialUSB.print("  Period: [");
  SerialUSB.print(periodMin, DEC);
  SerialUSB.print(" - ");
  SerialUSB.print(periodMax, DEC);
  SerialUSB.println("] us");
  SerialUSB.print("  T_ON: [");
  SerialUSB.print(tOnMin, DEC);
  SerialUSB.print(" - ");
  SerialUSB.print(tOnMax, DEC);
  SerialUSB.println("] us");
  SerialUSB.print("  Input PIN: D");
  SerialUSB.println(INPUT_PIN, DEC);
  SerialUSB.print("  Output PIN: D");
  SerialUSB.println(OUTPUT_PIN, DEC);
}

// Calculate min/max period and min/max pulse width in microseconds. Configure INPUT_PIN and OUTPUT_PIN as input/output.
static void configure(void) {

  periodMin = (uint32_t)((NSEC_IN_SEC / (frequency + (frequency * TOLERANCE_FREQUENCY / THOUSAND))) / THOUSAND);
  periodMax = (uint32_t)((NSEC_IN_SEC / (frequency - (frequency * TOLERANCE_FREQUENCY / THOUSAND))) / THOUSAND);
  
  tOnMin = (uint32_t)(periodMin * (dutyCycle - TOLERANCE_DUTY) / HUNDRED);
  tOnMax = (uint32_t)(periodMax * (dutyCycle + TOLERANCE_DUTY) / HUNDRED);
  
  pinMode(INPUT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  digitalWrite(OUTPUT_PIN, LOW); // Ensure the output pin is initially low
  
}
