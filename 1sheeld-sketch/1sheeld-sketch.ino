#define CUSTOM_SETTINGS
#define INCLUDE_TERMINAL_SHIELD
#define INCLUDE_INTERNET_SHIELD
#define INCLUDE_PUSH_BUTTON_SHIELD
#define INCLUDE_VOICE_RECOGNIZER_SHIELD
#define INCLUDE_TEXT_TO_SPEECH_SHIELD

#include <AltSoftSerial.h>
#include <ArduinoSTL.h>
#include <OneSheeld.h>
#include <Player.h>

using namespace std;

//Set up SoftwareSerial for our xBee comms (AltSoftSerial using Rx Tx pins 8 and 9 on Arduino Uno board)
AltSoftSerial xBee;

vector<Player*> players;

char value;

int connectionCount = 0;

//Our player's name and their score
String nickname = "";
int score = 0;

//A string to construct a JSON object containing users nickname and score, so we can make API request with data
String jsonObject = "{\"nickname\": \"" + nickname + "\", \"score\": " + (String)score + "}";

//Our API route for handling score data
HttpRequest scoreRequest("https://kwillis.eu/hiscores");

// Voice recognition
bool voiceCommandActive = false;
// Commands to be recognised by Zumos
const char startGameCommand[] = "start game";
const char forwardCommand[] = "forward";
const char backwardCommand[] = "backward";
const char leftCommand[] = "left";
const char rightCommand[] = "right";

// Variable so the 1Sheeld knows who's turn it is next
int playerToMoveNext = 1;

unsigned long lastConnectedTime = millis();

void setup()
{
    //Set up HTTP request for RESTful API calls
    scoreRequest.setOnSuccess(&onSuccess);
    scoreRequest.setOnFailure(&onFailure);
    scoreRequest.addHeader("Content-Type", "application/json");

    //Set up serial on 115200 baud rate for our 1Sheeld+
    OneSheeld.begin();

    //Set up our xBee on 9600 baud rate (needs to be at a lower baud rate than our UART serial, as our UART is handling lock interrupts faster
    xBee.begin(9600);
    xBee.println("Sheeld First Broadcast");

    while (millis() - lastConnectedTime < 10000) {
        if (xBee.available() > 0) {

            value = (char)xBee.read();

            switch (value) {
            case 'I':
                ++connectionCount;
                xBee.println(connectionCount);

                bool nickAssigned = false;

                TextToSpeech.say("Say you name now.");
                delay(2000); //so john can talk without affecting voice recognition
                VoiceRecognition.start();
                delay(5000); //so player has time to say there name after start()

                while (!nickAssigned) {
                    
                    if (VoiceRecognition.isNewCommandReceived()) {
                        String nickname = VoiceRecognition.getLastCommand();
                        Player* player = new Player(connectionCount, nickname);
                        players.push_back(player);
                        nickAssigned = true;
                        TextToSpeech.say("Successfully registered, thanks.");
                        Terminal.println("true");
                    } else {
                        Terminal.println("false");
                    }
                }
                lastConnectedTime = millis();
                break;
            }
        }
    }
    TextToSpeech.say(String(connectionCount) + " players connected, the game will start now.");
    delay(4000);
}

void loop()
{
    VoiceRecognition.start();
    
    if(VoiceRecognition.isNewCommandReceived()) {
      if(strstr(VoiceRecognition.getLastCommand(), forwardCommand)) {
        moveZumo(playerToMoveNext, 'w');
        delay(3000);
        nextPlayersTurn();
        
      }
      else if(strstr(VoiceRecognition.getLastCommand(), leftCommand)) {
        moveZumo(playerToMoveNext, 'a');
        delay(3000);
        nextPlayersTurn();
                        
      }
      else if(strstr(VoiceRecognition.getLastCommand(), backwardCommand)) {
        moveZumo(playerToMoveNext, 's');
        delay(3000);
        nextPlayersTurn();
                
      }
      else if(strstr(VoiceRecognition.getLastCommand(), rightCommand)) {
        moveZumo(playerToMoveNext, 'd');
        delay(3000);
        nextPlayersTurn();
            
      }
  
      // Update Zumo's current location (cell number)
  
      // Check if new cell contains a bomb
            

      return;
    
    }  
}

void nextPlayersTurn() {
  if(connectionCount == playerToMoveNext) {
    TextToSpeech.say("Player 1 Turn");
    delay(1000);
    playerToMoveNext = 1;
  } else {
    TextToSpeech.say("Next Players Turn");
    delay(1000);
    playerToMoveNext++;
  }  
}

void moveZumo(int connectionID, char dir)
{

    String toSend = String(connectionID) + ":" + dir;
    xBee.println('M');
    xBee.println(toSend);
}

void submitScore()
{
    //Print JSON object to terminal for debugging
    Terminal.println("Attempting to post:");
    Terminal.println(&(jsonObject)[0]);

    //Add data to our HTTP request and post to our API
    scoreRequest.addRawData(&(jsonObject)[0]);
    Internet.performPost(scoreRequest);
}

void onFailure(HttpResponse& res)
{
    //Do below on an unsuccessful HTTP request
    Terminal.println("Request failed");
    Terminal.println(res.getStatusCode());
    Terminal.println(res.getTotalBytesCount());
}

void onSuccess(HttpResponse& res)
{
    //Do below on a successful HTTP request
    Terminal.println("Request succeeded");
    Terminal.println(res.getStatusCode());
    Terminal.println(res.getTotalBytesCount());
}
