//includes

//for zumo
#include <ZumoMotors.h>
#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>


//for reflectance sensors
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>

//for gyro
#include <L3G.h>
#include <TurnSensor.h>
#include <Utilities.h>

#define QTR_THRESHOLD 1000; //desired color threshold for QTR sensors. 
#define NUM_SENSORS 6 //no. of sensors that the ZumoReflectanceSensorArray has
#define LED 13 //for the Zumo LED

ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);

unsigned int sensor_values[NUM_SENSORS]; //create an array which holds an int value for each one of our sensors to return a value to.

L3G gyro;

int myId = -1;
int reqId = -2;

String req = "";
String motor;

char value;
char reqCommand;

void setup()
{
  //turn LED on and wait for button push before playing tune and handshake with processing
  digitalWrite(LED, HIGH);

  Serial.begin(9600);
  turnSensorSetup();



  button.waitForButton();

  digitalWrite(LED, LOW);
  buzzer.play(">g32>>c32");


  Serial.println('I');

  delay(200);

  byte b1 = Serial.read();
  byte b2 = Serial.read();

  myId = b1 * 256 + b2;

  delay(100);

  Serial.println(myId);
}

void loop()
{
  sensors.read(sensor_values);

  if (Serial.available() > 0) {
    value = (char) Serial.read();

    switch (value)
    {
      case 'M':
        String idString = Serial.readStringUntil(':');
        reqId = idString.toInt();
        req = Serial.readString();
        reqCommand = req.charAt(0);

    }

    if (reqId == myId) {
      //request was for us...
      if (reqCommand == 'w') {
        motors.setSpeeds(100, 100);
        delay(2500);
        motors.setSpeeds(0, 0);
      }
      else if (reqCommand == 'a') {
        turn(90);
        motors.setSpeeds(100, 100);
        delay(2500);
        motors.setSpeeds(0, 0);

      }
      else if (reqCommand == 's') {
        motors.setSpeeds(-100, -100);
        delay(2500);
        motors.setSpeeds(0, 0);
      }
      else if (reqCommand == 'd') {
        turn(-90);
        motors.setSpeeds(100, 100);
        delay(2500);
        motors.setSpeeds(0, 0);
      }

      // Reset request variables ready for next request
      req = "";
      reqId = -2;
    }
  }
}

void turn(int target) {

  if (target > 0)
    Serial.println("turning anti-clockwise");
  else
    Serial.println("turning clockwise");

  turnSensorReset();
  int32_t currHeading = Utilities::wrapAngle(turnSensorUpdate());
  int32_t targetAngle = Utilities::wrapAngle(currHeading + target);
  float error, speed;
  bool done = false;
  while (!done) {

    error = Utilities::wrapAngle(turnSensorUpdate() - targetAngle);
    done = Utilities::inRange(fabs(error), (float) - 1, (float) 1);

    speed = 150;

    if (error > 0) {
      motors.setSpeeds(speed, -speed);
    }
    else {
      motors.setSpeeds(-speed, speed);
    }
  }
  motors.setSpeeds(0, 0);
}
