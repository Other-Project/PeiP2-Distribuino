#include <Stepper.h>
const int stepsPerRevolution = 2048;
const int rpm = 12;
Stepper stepper1 = Stepper(stepsPerRevolution, 31, 35, 33, 37);

void setup() {
  stepper1.setSpeed(rpm);
}

void loop() {
  stepper1.step(stepsPerRevolution);
  delay(1000);
}
