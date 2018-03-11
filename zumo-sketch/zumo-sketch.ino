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
int reqId = -2;
String req = "";
String motor;
char value;
char reqCommand;

void setup()
{
  //turn LED on and wait for button push before playing tune and handshake with processing
  digitalWrite(LED, HIGH);
  button.waitForButton();
  digitalWrite(LED, LOW);
  buzzer.play(">g32>>c32");
  delay(1000);

  Serial.begin(9600);

  delay(200);

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
      if(reqCommand == 'w') {
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
