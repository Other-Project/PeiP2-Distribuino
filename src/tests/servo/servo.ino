#include <Servo.h>

Servo myservo;
bool test = true;

void setup() {
  myservo.attach(13);  // attaches the servo on pin 9
  myservo.write(140);
}

void loop() {
  if(test){
    myservo.write(0);
    delay(1000);
    myservo.write(140);
    delay(5000);
  }
}
