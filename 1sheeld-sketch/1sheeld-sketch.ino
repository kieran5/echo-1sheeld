#define CUSTOM_SETTINGS
#define INCLUDE_TERMINAL_SHIELD
#define INCLUDE_INTERNET_SHIELD
#define INCLUDE_PUSH_BUTTON_SHIELD

#include <OneSheeld.h>
#include <AltSoftSerial.h>
//#include <ArduinoSTL.h>

//Set up SoftwareSerial for our xBee comms (AltSoftSerial using Rx Tx pins 8 and 9 on Arduino Uno board)
AltSoftSerial xBee;

//using std::vector;

bool hasRun = false;

//vector<int> zumos;
char value;

int connectionCount = 0;

//Our player's name and their score
String nickname = "";
int score = 0;

//A string to construct a JSON object containing users nickname and score, so we can make API request with data
String jsonObject = "{\"nickname\": \"" + nickname + "\", \"score\": " + (String) score + "}";

//Our API route for handling score data
HttpRequest scoreRequest("https://kwillis.eu/hiscores");

void setup()
{
  //Set up serial on 115200 baud rate for our 1Sheeld+
  OneSheeld.begin();

  //Set up our xBee on 9600 baud rate (needs to be at a lower baud rate than our UART serial, as our UART is handling lock interrupts faster
  xBee.begin(9600);

  //Set up HTTP request for RESTful API calls
  scoreRequest.setOnSuccess(&onSuccess);
  scoreRequest.setOnFailure(&onFailure);
  scoreRequest.addHeader("Content-Type", "application/json");

  //  //Test UART and SoftwareSerial are working
  //  delay(2000);
  //  Serial.println("----- UART Serial -----");
    xBee.println("Sheeld First Broadcast");

  
}

void loop()
{
  if (xBee.available() > 0) {

    value = (char) xBee.read();

    switch (value)
    {
      case 'I':
        ++connectionCount;
        delay(100);
        xBee.write(connectionCount / 256);
        xBee.write(connectionCount % 256);
        xBee.write(0x0A);
        break;
    }
  }

  if(PushButton.isPressed()) {
    moveZumo(1, 'w');
    delay(2000);
  }

  //moveZumo(1, 'w');
  //delay(1000);

}

void moveZumo(int connectionID, char dir) {

  String toSend = String(connectionID) + ":" + dir;
  
  xBee.println('M');
  xBee.println(toSend);

}

void submitScore() {
  //Print JSON object to terminal for debugging
  Terminal.println("Attempting to post:");
  Terminal.println(&(jsonObject)[0]);

  //Add data to our HTTP request and post to our API
  scoreRequest.addRawData(&(jsonObject)[0]);
  Internet.performPost(scoreRequest);
}

void onFailure (HttpResponse &res)
{
  //Do below on an unsuccessful HTTP request
  Terminal.println("Request failed");
  Terminal.println(res.getStatusCode());
  Terminal.println(res.getTotalBytesCount());
}

void onSuccess (HttpResponse &res)
{
  //Do below on a successful HTTP request
  Terminal.println("Request succeeded");
  Terminal.println(res.getStatusCode());
  Terminal.println(res.getTotalBytesCount());
}
