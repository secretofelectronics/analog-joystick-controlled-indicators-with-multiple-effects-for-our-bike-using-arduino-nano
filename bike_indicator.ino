// Define pin numbers for LEDs driven by NPN transistors
const int leftIndicatorPin = 5;
const int rightIndicatorPin = 6;

// Define joystick and mode switch pins
const int joyXPin = A0;
const int joySwitchPin = 2;
const int modeSwitchPin = 4; // Additional switch to toggle between modes

// Variables to store joystick and switch states
int joyXValue;
int joySwitchState;
int modeSwitchState;
int lastJoySwitchState = HIGH;
int lastModeSwitchState = HIGH;

bool hazardMode = false;       // Variable to toggle between normal and hazard mode
int indicatorMode = 0;         // Variable to track the current mode (0=solid, 1=slow blink, 2=fast blink, 3=fade)

// Timing variables for blinking and fading
unsigned long previousMillisSlow = 0;
unsigned long previousMillisFast = 0;
int fadeValue = 0;                   // For fading effect
int fadeDirection = 5;               // Fading increment/decrement

// Setup function
void setup() {
  pinMode(leftIndicatorPin, OUTPUT);
  pinMode(rightIndicatorPin, OUTPUT);
  pinMode(joyXPin, INPUT);
  pinMode(joySwitchPin, INPUT_PULLUP); // Joystick switch uses internal pull-up resistor
  pinMode(modeSwitchPin, INPUT_PULLUP); // Mode switch uses internal pull-up resistor
}

// Function to turn on NPN-driven LED (set pin HIGH to turn on)
void turnOnNPN(int pin) {
  digitalWrite(pin, HIGH);
}

// Function to turn off NPN-driven LED (set pin LOW to turn off)
void turnOffNPN(int pin) {
  digitalWrite(pin, LOW);
}

// Loop function
void loop() {
  // Read joystick values
  joyXValue = analogRead(joyXPin);
  joySwitchState = digitalRead(joySwitchPin);
  modeSwitchState = digitalRead(modeSwitchPin);

  // Check for joystick switch press to toggle hazard mode
  if (joySwitchState == LOW && lastJoySwitchState == HIGH) {
    hazardMode = !hazardMode;
    delay(200); // Debounce delay
  }
  lastJoySwitchState = joySwitchState;

  // Check for mode switch press to cycle through modes
  if (modeSwitchState == LOW && lastModeSwitchState == HIGH) {
    indicatorMode = (indicatorMode + 1) % 4; // Cycle through 4 modes (0, 1, 2, 3)
    delay(200); // Debounce delay
  }
  lastModeSwitchState = modeSwitchState;

  // Get the current time for blink timing
  unsigned long currentMillis = millis();

  // Hazard mode: apply selected mode to both indicators
  if (hazardMode) {
    applyMode(leftIndicatorPin, rightIndicatorPin, currentMillis);
  }
  else {
    // Normal mode: control left or right indicator based on joystick direction
    if (joyXValue < 400) {  // Left direction
      applyMode(leftIndicatorPin, -1, currentMillis);
      turnOffNPN(rightIndicatorPin);
    }
    else if (joyXValue > 900) { // Right direction
      applyMode(rightIndicatorPin, -1, currentMillis);
      turnOffNPN(leftIndicatorPin);
    }
    else { // Center position, no indication
      turnOffNPN(leftIndicatorPin);
      turnOffNPN(rightIndicatorPin);
    }
  }
}

// Function to apply the selected mode to the indicators
void applyMode(int pin1, int pin2, unsigned long currentMillis) {
  switch (indicatorMode) {
    case 0: // Solid
      turnOnNPN(pin1);
      if (pin2 != -1) turnOnNPN(pin2); // Turn on both in hazard mode
      break;

    case 1: // Slow Blinking
      if (currentMillis - previousMillisSlow >= 1000) { // Slow blink interval
        previousMillisSlow = currentMillis;
        digitalWrite(pin1, !digitalRead(pin1)); // Toggle pin
        if (pin2 != -1) digitalWrite(pin2, !digitalRead(pin2));
      }
      break;

    case 2: // Fast Blinking
      if (currentMillis - previousMillisFast >= 200) { // Fast blink interval
        previousMillisFast = currentMillis;
        digitalWrite(pin1, !digitalRead(pin1)); // Toggle pin
        if (pin2 != -1) digitalWrite(pin2, !digitalRead(pin2));
      }
      break;

    case 3: // Fading
      analogWrite(pin1, fadeValue); // Set PWM for fading
      if (pin2 != -1) analogWrite(pin2, fadeValue);

      fadeValue += fadeDirection;
      if (fadeValue <= 0 || fadeValue >= 255) {
        fadeDirection = -fadeDirection; // Reverse fade direction at ends
      }
      delay(15); // Adjust this delay for smooth fading
      break;
  }
}
