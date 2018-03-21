//includes

//for zumo
#include <Pushbutton.h>
#include <ZumoBuzzer.h>
#include <ZumoMotors.h>

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
#define SPEED 75

ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);

unsigned int sensor_values[NUM_SENSORS]; //create an array which holds an int value for each one of our sensors to return a value to.

#include <avr/pgmspace.h>


const char victoryTune[] PROGMEM = "! O5 L16 agafaea dac+adaea fa<aa<bac#a dac#adaea f"
                             "O6 dcd<b-d<ad<g d<f+d<gd<ad<b- d<dd<ed<f+d<g d<f+d<gd<ad"
                             "L8 MS <b-d<b-d MLe-<ge-<g MSc<ac<a ML d<fd<f O5 MS b-gb-g"
                             "ML >c#e>c#e MS afaf ML gc#gc# MS fdfd ML e<b-e<b-"
                             "O6 L16ragafaea dac#adaea fa<aa<bac#a dac#adaea faeadaca"
                             "<b-acadg<b-g egdgcg<b-g <ag<b-gcf<af dfcf<b-f<af"
                             "<gf<af<b-e<ge c#e<b-e<ae<ge <fe<ge<ad<fd"
                             "O5 e>ee>ef>df>d b->c#b->c#a>df>d e>ee>ef>df>d"
                             "e>d>c#>db>d>c#b >c#agaegfe f O6 dc#dfdc#<b c#4";

const char deathTune[] PROGMEM = "dd-c";

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

    myId = Serial.parseInt();

    Serial.println(myId);
}

void loop()
{
    sensors.read(sensor_values);

    if (Serial.available() > 0) {
        value = (char)Serial.read();

        switch (value) {
        case 'M':
            String idString = Serial.readStringUntil(':');
            reqId = idString.toInt();
            req = Serial.readString();
            reqCommand = req.charAt(0);
        }

        if (reqId == myId) {
            //Serial.println("HIT");
            //request was for us...
            if (reqCommand == 'w') {
                advance();
            } else if (reqCommand == 'a') {
                turn(90);
                advance();
            } else if (reqCommand == 's') {
                turn(180);
                advance();
            } else if (reqCommand == 'd') {
                turn(-90);
                advance();
            } else if (reqCommand == 'b') {
                doDeathSpin();
            } else if (reqCommand == 'v') {
                buzzer.playFromProgramSpace(victoryTune);
            }

                // Reset request variables ready for next request
                req = "";
            reqId = -2;
        }
    }
}

/**
* @author      Jake Slade
* @description This method is used to accurately turn the Zumo using it's gyroscope
* @params      The angle that you'd like to turn, -90 would turn 90 degrees clockwise, 90 would turn 90 degrees anti-clockwise.
* @returns     void
*/

void turn(int target)
{
    turnSensorReset();
    int32_t currHeading = Utilities::wrapAngle(turnSensorUpdate());
    int32_t targetAngle = Utilities::wrapAngle(currHeading + target);
    float error, speed;
    bool done = false;
    while (!done) {

        error = Utilities::wrapAngle(turnSensorUpdate() - targetAngle);
        done = Utilities::inRange(fabs(error), (float)-1, (float)1);

        speed = 150;

        if (error > 0) {
            motors.setSpeeds(speed, -speed);
        } else {
            motors.setSpeeds(-speed, speed);
        }
    }
    motors.setSpeeds(0, 0);
}

/**
* @author      Jake Slade
* @description This method is called to send the Zumo in to a new grid cell. It will cross one black line, 
               and stop at the next that it hits, and then automatically readjust itself so that is is flush wiuth the line. 
* @params      A reference of the HTTP protocol response
* @returns     void
*/
void advance()
{
    bool hasCrossedLine = false;
    bool isAtEnd = false;

    while (!isAtEnd) {

        motors.setSpeeds(SPEED, SPEED);

        while (!hasCrossedLine) {

            sensors.read(sensor_values);

            if (isOverLine(sensor_values[2]) && isOverLine(sensor_values[3])) {
                delay(1000);
                hasCrossedLine = true;
            }
        }

        sensors.read(sensor_values);

        if (isOverLine(sensor_values[2]) && isOverLine(sensor_values[3])) {
            isAtEnd = true;
        }
        moveForwardWithinBoundaries();
    }
    motors.setSpeeds(0, 0);
    delay(250);
    motors.setSpeeds(-75, -75);
    delay(250);
    motors.setSpeeds(0, 0);
}


/**
* @author      Jake Slade
* @description A bool check to see if our QTR sensors are above the desired threshhold (in our case, over a line).
* @params      The pins that you want to check, in our case this would be an element of sensor_values, eg. sensor_values[5]
* @returns     True if the sensor is above the threshold, otherwise false
*/
bool isOverLine(int sensorPin)
{
    return sensorPin > QTR_THRESHOLD;
}

/**
* @author      Jake Slade
* @description This method is for auto re-alignment of the Zumo at the end of each turn.
* @params      void
* @returns     void
*/
void moveForwardWithinBoundaries()
{
    if (isOverLine(sensor_values[0])) //if leftmost sensor detects the border
    {
        while (isOverLine(sensor_values[0]) && !isOverLine(sensor_values[5])) //now if the leftmost sensor detects the border and the rightmost sensor does not, safe to assume it is not a dead end
        {
            sensors.read(sensor_values);
            motors.setSpeeds(-150, 150);
        }
    } else if (isOverLine(sensor_values[5])) //same again for the other (rightmost) side of the Zumo...
    {
        while (isOverLine(sensor_values[5]) && !isOverLine(sensor_values[0])) {
            sensors.read(sensor_values);
            motors.setSpeeds(150, -150);
        }
    }
}

/**
* @author      Jake Slade
* @description Makes the Zumo perform a 'death animation' and play a sad tune.
* @params      void
* @returns     void
*/
void doDeathSpin()
{
    buzzer.playFromProgramSpace(deathTune);
    for (int i = 300; i > 0; i--) {
        motors.setSpeeds(i, -i);
        delay(10);
    }
}
