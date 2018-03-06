//includes
#include <ZumoMotors.h>


//global variables
//left and right motor speed, adjust here for known motor imbalance
int lms = 100; int rms = 100;
ZumoMotors motors;

void setup()
{
  Serial.begin(9600);

}

void loop()
{

  char motor = (char) Serial.read();
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




