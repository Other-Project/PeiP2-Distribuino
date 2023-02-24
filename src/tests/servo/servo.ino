#include <Servo.h>

Servo myservo;

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9
  myservo.write(140);
}

void loop() {
  myservo.write(140);
  delay(5000);    
  myservo.write(0);
  delay(1000); 
  myservo.write(0);
  delay(1000);
}
