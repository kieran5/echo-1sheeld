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

int c = -1;
char motor;

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

  while(c == -1)
  {
    c = Serial.read();
  }

  c = c - 48;
}

void loop()
{
  motor = (char) Serial.read();
  switch (motor)
  {
    case 'w':
      motors.setLeftSpeed(lms);
      motors.setRightSpeed(rms);
      break;
    case 'a':
      motors.setLeftSpeed(-lms);
      motors.setRightSpeed(rms);
      break;
    case 's':
      motors.setLeftSpeed(-lms);
      motors.setRightSpeed(-rms);
      break;
    case 'd':
      motors.setLeftSpeed(lms);
      motors.setRightSpeed(-rms);
      break;
    case 'q':
      motors.setLeftSpeed(0);
      motors.setRightSpeed(0);
      break;
  }
}




