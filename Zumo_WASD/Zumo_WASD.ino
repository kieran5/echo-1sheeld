//includes
#include <ZumoMotors.h>


//global variables
//left and right motor speed, adjust here for known motor imbalance
int baseLMS = 100; int baseRMS = 100; 
ZumoMotors motors;

void setup()
{
  Serial.begin(9600);
  
}

void loop()
{
  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);


  char motor = (char) Serial.read();
  switch (motor)
  {
    case 'w':
      advance(baseLMS, baseRMS); break;
    case 'a':
      advance((-baseLMS), (baseRMS));
      break;
    case 's':
      advance(-baseLMS, -baseRMS); break;
    case 'd':
      advance((baseLMS), (-baseRMS));
      break;

  }
}

void advance(int lms, int rms)
{
  while (!(Serial.read() == 'q'))
  {

    {
      motors.setLeftSpeed(lms);
      motors.setRightSpeed(rms);
    }
  }
}


