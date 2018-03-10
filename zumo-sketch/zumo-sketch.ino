//includes
#include <ZumoMotors.h>
#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>

#define LED 13 //for the Zumo LED

ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12


//global variables

//left and right motor speed, adjust here for known motor imbalance
int lms = 100; int rms = 100;

int myId = -1;
String motor;
char command;

String myAddress = "01";

void setup()
{
  //turn LED on and wait for button push before playing tune and handshake with processing
  digitalWrite(LED, HIGH);
  button.waitForButton();
  digitalWrite(LED, LOW);
  buzzer.play(">g32>>c32");
  delay(1000);

  Serial.begin(9600);

  Serial.println("I");
  
  delay(100);

  myId = (int) Serial.read();
  
}

void loop()
{
  if (myId != -1) {
      digitalWrite(LED, HIGH);
  }
  else {
      digitalWrite(LED, LOW);
  }




//  //first element is the zumo identifier, second is WASD command
//  char myString[2];
//  motor.toCharArray(myString, 2);
//
//  char idSignal = myString[0];
//  char wasdSignal = myString[1];
//
//  Serial.println("First element: " + myString[0]);
//  Serial.println("Second element: " + myString[1]);
//
//  if (zumoId == idSignal) {
//    switch (wasdSignal)
//    {
//      case 'w':
//        motors.setLeftSpeed(lms);
//        motors.setRightSpeed(rms);
//        break;
//      case 'a':
//        motors.setLeftSpeed(-lms);
//        motors.setRightSpeed(rms);
//        break;
//      case 's':
//        motors.setLeftSpeed(-lms);
//        motors.setRightSpeed(-rms);
//        break;
//      case 'd':
//        motors.setLeftSpeed(lms);
//        motors.setRightSpeed(-rms);
//        break;
//      case 'q':
//        motors.setLeftSpeed(0);
//        motors.setRightSpeed(0);
//        break;
//    }
//
//    delay(5000);
//  }
}




