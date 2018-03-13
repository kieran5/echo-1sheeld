#define CUSTOM_SETTINGS
#define INCLUDE_TERMINAL_SHIELD
#define INCLUDE_INTERNET_SHIELD
#define INCLUDE_PUSH_BUTTON_SHIELD
#define INCLUDE_VOICE_RECOGNIZER_SHIELD
#define INCLUDE_TEXT_TO_SPEECH_SHIELD

#include <OneSheeld.h>
#include <AltSoftSerial.h>

//Set up SoftwareSerial for our xBee comms (AltSoftSerial using Rx Tx pins 8 and 9 on Arduino Uno board)
AltSoftSerial xBee;

char value;

int connectionCount = 0;

//Our player's name and their score
String nickname = "";
int score = 0;

//A string to construct a JSON object containing users nickname and score, so we can make API request with data
String jsonObject = "{\"nickname\": \"" + nickname + "\", \"score\": " + (String) score + "}";

//Our API route for handling score data
HttpRequest scoreRequest("https://kwillis.eu/hiscores");

// Voice recognition
bool voiceCommandActive = false;
// Commands to be recognised by Zumos
const char forwardCommand[] = "forward";
const char backwardCommand[] = "backward";
const char leftCommand[] = "left";
const char rightCommand[] = "right";

// Variable so the 1Sheeld knows who's turn it is next
int playerToMoveNext = 1;

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

  xBee.println("Sheeld First Broadcast");

  //VoiceRecognition.start();
  
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

  // At least 2 players are required to play the game
  if(connectionCount > 1) {
    
    voiceCommandActive = true;
    VoiceRecognition.start();
    
    if(VoiceRecognition.isNewCommandReceived()) {
      if(strstr(VoiceRecognition.getLastCommand(), forwardCommand) && voiceCommandActive) {
        moveZumo(playerToMoveNext, 'w');
        
      }
      else if(strstr(VoiceRecognition.getLastCommand(), leftCommand) && voiceCommandActive) {
        moveZumo(playerToMoveNext, 'a');
        moveZumo(playerToMoveNext, 'w');
                        
      }
      else if(strstr(VoiceRecognition.getLastCommand(), backwardCommand) && voiceCommandActive) {
        moveZumo(playerToMoveNext, 's');
                
      }
      else if(strstr(VoiceRecognition.getLastCommand(), rightCommand) && voiceCommandActive) {
        moveZumo(playerToMoveNext, 'd');
        moveZumo(playerToMoveNext, 'w');
            
      }
  
      voiceCommandActive = false;
  
  
      // Update Zumo's current location (cell number)
  
      // Check if new cell contains a bomb
      
  
      if(connectionCount == playerToMoveNext) {
        playerToMoveNext = 1;
      } else {
        playerToMoveNext++;
      }      
    
    }  
  }
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
