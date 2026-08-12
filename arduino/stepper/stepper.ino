#include <Stepper.h>

const int stepsPerRevolution = 2048;
const int rolePerMinute = 10;

Stepper myStepper(stepsPerRevolution, 21, 18, 19, 5);//ln1-ln3-ln2-ln4

void setup() {
  myStepper.setSpeed(rolePerMinute);
  Serial.begin(9600);
}

void loop() {  
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  delay(500);

  // Serial.println("counterclockwise");
  // myStepper.step(-stepsPerRevolution);
  // delay(500);
}
