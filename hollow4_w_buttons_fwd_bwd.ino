// Add two buttons that will roll the clock forward/backward
// User variables
const int forwardButton = 4; // TODO: Define a pin for forwardButton
const int backwardButton = 2; // TODO: Define a pin for backwardButton
int port[4] = {5, 6, 7, 8}; // TODO: Define 4 pins for Stepper Motor

// Sketch variables
// Please tune the following value if the clock gains or loses.
// Theoretically, standard of this value is 60000.
#define MILLIS_PER_MIN 60000 // milliseconds per a minute

// Motor and clock parameters
// 4096 * 90 / 12 = 30720
#define STEPS_PER_ROTATION 30720 // steps for a full turn of minute rotor

// wait for a single step of stepper
int delaytime = 2;

// sequence of stepper motor control
int seq[8][4] = {
  {  LOW, HIGH, HIGH,  LOW},
  {  LOW,  LOW, HIGH,  LOW},
  {  LOW,  LOW, HIGH, HIGH},
  {  LOW,  LOW,  LOW, HIGH},
  { HIGH,  LOW,  LOW, HIGH},
  { HIGH,  LOW,  LOW,  LOW},
  { HIGH, HIGH,  LOW,  LOW},
  {  LOW, HIGH,  LOW,  LOW}
};

boolean forwardButtonState = false;
boolean forward = false;
boolean backwardButtonState = false;
boolean backward = false;

long prev_min = 0, prev_pos = 0;
long min;
long pos;

#include <Stepper.h>
const int stepsPerRevolution = 4096;
Stepper myStepper(stepsPerRevolution, port[0], port[1], port[2], port[3]);

// Set up
void setup() {
  Serial.begin(9600);
  myStepper.setSpeed(100);
  pinMode(port[0], OUTPUT);
  pinMode(port[1], OUTPUT);
  pinMode(port[2], OUTPUT);
  pinMode(port[3], OUTPUT);
  pinMode(forwardButton, INPUT);
  pinMode(backwardButton, INPUT);
  rotate(-20); // for approach run
  rotate(20); // approach run without heavy load
  rotate(STEPS_PER_ROTATION / 60);
}

// Main Loop
void loop() {
  forward = check_forwardButton();
  backward = check_backwardButton();
  boolean rollingTime = true;

  if (forward) {
    // If the "forward" button is pressed, then roll the clock forward in time
    while (rollingTime) {
      // Keep rolling the clock forward in time until another button is pressed
      rollForward();
      rollingTime = !anyButton();
    }
    forward = false;
    delay(1000);
    Serial.println("Exit Rolling Forward");
  }
  else if (backward) {
    // If the "bacward" button is pressed, then roll the clock backward in time
    while (rollingTime) {
      // Keep rolling the clock backward in time until another button is pressed
      rollBackward();
      rollingTime = !anyButton();
    }
    backward = false;
    delay(1000);
    Serial.println("Exit Rolling Backward");
  }
  else {
    // Otherwise, just keep time
    keepTime();
  }
}

void keepTime() {  
  // Moves the minute-hand every minute
  min = millis() / MILLIS_PER_MIN;
  if(prev_min == min) {
    return;
  }
  Serial.print("Current min: "); Serial.println(min);
  prev_min = min;
  pos = (STEPS_PER_ROTATION * min) / 60;
  rotate(-20); // for approach run
  rotate(20); // approach run without heavy load
  
  if(pos - prev_pos > 0) {
    rotate(pos - prev_pos);
  }
  prev_pos = pos;
}

void rollForward() {
  // Moves the clock forward in time continuously
  Serial.println("Rolling Forward");
  rotate(STEPS_PER_ROTATION / 60);
}

void rollBackward() {
  // Moves the clock backward in time continuously
  Serial.println("Rolling Backward");
  rotate(-STEPS_PER_ROTATION / 60);
}

void rotate(int step) {
  // Spins the Stepper Motor continuously
  static int phase = 0;
  int i, j;
  int delta = (step > 0) ? 1 : 7;
  int dt = 20;

  step = (step > 0) ? step : -step;
  for(j = 0; j < step; j++) {
    phase = (phase + delta) % 8;
    for(i = 0; i < 4; i++) {
      digitalWrite(port[i], seq[phase][i]);
    }
    delay(dt);
    if(dt > delaytime) dt--;
  }
  // power cut
  for(i = 0; i < 4; i++) {
    digitalWrite(port[i], LOW);
  }
}

boolean anyButton() {
  // Check if any button has been pressed
  Serial.println("Any Button");
  return check_forwardButton() || check_backwardButton();
}

boolean check_forwardButton() {
  // Check if the "forward" button has been pressed
  if (debounce_forwardButton(forwardButtonState) == true && forwardButtonState == false) {
    forwardButtonState = true;
    return true;
  }
  else if (debounce_forwardButton(forwardButtonState) == false && forwardButtonState == true) {
    forwardButtonState = false;
    return false;
  }
  else {
    forwardButtonState = false;
    return false;
  }
}

boolean check_backwardButton() {
  // Check if the "backward" button has been pressed
  if (debounce_backwardButton(backwardButtonState) == true && backwardButtonState == false) {
    backwardButtonState = true;
    return true;
  }
  else if (debounce_backwardButton(backwardButtonState) == false && backwardButtonState == true) {
    backwardButtonState = false;
   return false;
  }
  else {
    backwardButtonState = false;
    return false;
  }
}

boolean debounce_forwardButton(boolean buttonState) {
  // Helps to more accurately check the state of a button
  boolean buttonStateNow = digitalRead(forwardButton);
  if (buttonState != buttonStateNow) {
    delay(10);
    buttonStateNow = digitalRead(forwardButton);
  }

  return buttonStateNow;
}

boolean debounce_backwardButton(boolean buttonState) {
  // Helps to more accurately check the state of a button
  boolean buttonStateNow = digitalRead(backwardButton);
  if (buttonState != buttonStateNow) {
    delay(10);
    buttonStateNow = digitalRead(backwardButton);
  }

  return buttonStateNow;
}
