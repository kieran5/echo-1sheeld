#define CUSTOM_SETTINGS
#define INCLUDE_TERMINAL_SHIELD
#define INCLUDE_INTERNET_SHIELD
#define INCLUDE_PUSH_BUTTON_SHIELD
#define INCLUDE_VOICE_RECOGNIZER_SHIELD
#define INCLUDE_TEXT_TO_SPEECH_SHIELD

#include <OneSheeld.h>
#include <AltSoftSerial.h>
#include <Player.h>
#include <ArduinoSTL.h>
#include <vector>

using namespace std;

//Set up SoftwareSerial for our xBee comms (AltSoftSerial using Rx Tx pins 8 and 9 on Arduino Uno board)
AltSoftSerial xBee;

char value;

int connectionCount = 0;

vector<Player*> players;

//Our player's name and their score
String nickname = "";
int score = 0;

//A string to construct a JSON object containing users nickname and score, so we can make API request with data
String jsonObject = "{\"nickname\": \"" + nickname + "\", \"score\": " + (String) score + "}";

//Our API route for handling score data
HttpRequest scoreRequest("https://kwillis.eu/hiscores");

// Voice recognition
bool gameStarted = false;
// Commands to be recognised by Zumos
const char startGameCommand[] = "start game";
const char forwardCommand[] = "forward";
const char backwardCommand[] = "backward";
const char leftCommand[] = "left";
const char rightCommand[] = "right";

// Variable so the 1Sheeld knows who's turn it is next
int playerToMoveNext = 1;

// This stuff explains what is in the [z]axis of the matrix array. It is not needed by the program, but is laid out here for understanding
// 0 for no, 1 for zumo,
//int zumo?; //z[0]

//0 for no, 1 for bomb
//int bomb?: //z[1]

//the playerID of the zumo or bomb
//int playerID; //z[2]

//the facing of a zumo, 1 is north/up, 2 is east/right, 3 is south/down, 4 is west/left
//int orientation; //z[3]

//does this zumo still have a bomb it can drop, 1 for yes, 0 for no.
//int hasBomb; //z[4]

//The current score of the zumo
//int Score; //z[5]



//This is actual program stuff

//the 3dimensional array that holds the game state
int matrix[4][4][6];

int newX;
int newY;

//should be called after the zumo has physically moved. returns true if the move is safe, returns false if the zumo is dead.
bool moveZumo(int playerID) {

  //search the array for the correct zumo, by looping through the array a row at a time and checking if each cell contains a zumo with the correct user id
  for (int x = 0; x < 4; x++)
  {
    for (int y = 0; y < 4; y++)
    {
      if (matrix[x][y][0] == 1 && matrix[x][y][2] == playerID)
      {

        //set the co-ordinates that the zuumo has moved to
        switch (matrix[x][y][3])
        {
          case 1:
            newY = y + 1;
            newX = x;
            break;
          case 2:
            newX = x + 1;
            newY = y;
            break;
          case 3:
            newY = y - 1;
            newX = x;
            break;
          case 4:
            newX = x + 1;
            newY = y;
            break;
        }


        if (ContainsBomb(newX, newY))
        {
          //take the id of the player who left the bomb
          int id = matrix[newX][newY][2];

          //remove the bomb
          matrix[newX][newY][1] = 0;
          matrix[newX][newY][2] = 0;

          //search for their zumo in the array
          for (int x = 0; x < 4; x++)
          {
            for (int y = 0; y < 4; y++)
            {
              if (matrix[x][y][0] == 1 && matrix[x][y][2] == id)

                //Give that zumo a new bomb
                matrix[x][y][4] = 1;
              // increase its score
              matrix[x][y][5] = matrix[x][y][5] + 5;

              //the current player's zumo is dead. Post their score.
              postScore(matrix[x][y][2], matrix[x][y][5]);

              //if they have deployed their bomb, find it and remove it
              if (matrix[x][y][4] == 0)
              {
                for (int x = 0; x < 4; x++)
                {
                  for (int y = 0; y < 4; y++)
                  {
                    if (matrix[x][y][1] == 1 && matrix[x][y][2] == playerID)
                    {
                      matrix[x][y][1] = 0;
                      matrix[x][y][2] = 0;
                    }
                  }
                }
              }
            }
          }

          //remove their zumo from the grid
          matrix[x][y][0] = 0;
          matrix[x][y][3] = 0;
          matrix[x][y][4] = 0;
          matrix[x][y][5] = 0;

          return false;
        }
        //if you haven't hit a bomb
        //move the representation of the zumo to the new square
        //don't move the bomb!
        matrix[newX][newY][0] = matrix[x][y][0];
        matrix[newX][newY][2] = matrix[x][y][2];
        matrix[newX][newY][3] = matrix[x][y][3];
        matrix[newX][newY][4] = matrix[x][y][4];
        matrix[newX][newY][5] = matrix[x][y][5] + 1;
        //clear the previous square
        //don't clear the bomb!
        matrix[x][y][0] = 0;
        matrix[x][y][3] = 0;
        matrix[x][y][4] = 0;
        matrix[x][y][5] = 0;
        //don't clear the user ID if a bomb has been dropped
        if (matrix[x][y][1] == 0)
        {
          matrix[x][y][2] = 0;
        }

        return true;
      }
    }
  }
}



//returns playerID if the co-ordinates contain a bomb, and 0 if they do not
bool ContainsBomb(int x, int y) {
  if (matrix[x][y][1] = 1)
  {
    return true;
  }
  return false;
}

void setZumo(int playerID) {
  matrix[1][playerID][0] = 1;
  matrix[1][playerID][1] = 0;
  matrix[1][playerID][2] = playerID - 1;
  matrix[1][playerID][3] = 1;
  matrix[1][playerID][4] = 1;
  matrix[1][playerID][5] = 0;
}

//call this when a player signals they want to drop their bomb. Players should drop their bomb before they move
void setBomb(int playerID, char dir) {
  //search for the players zumo and drop a bomb on the same tile
  for (int x = 0; x < 4; x++)
  {
    for (int y = 0; y < 4; y++)
    {
      if (matrix[x][y][0] == 1 && matrix[x][y][2] == playerID)
      {

      }
    }
  }
}

//posts a score. is called when a zumo dies, needs to be called for the last man standing, we need to figure out how to tell when the last man is standing.
void postScore(int playerID, int playerScore)
{
  //needs sorting
  //there needs to be an array that holds playerID's and nicknames which we can compare here
  //then post player nickname and score to the website
}

//tells the zumo if you have turned, call this each time a 90 degree turn is made
void setOrientation(int playerID, char turnVal) {
  //Search for the correct zumo
  for (int x = 0; x < 4; x++)
  {
    for (int y = 0; y < 4; y++)
    {
      if (matrix[x][y][0] == 1 && matrix[x][y][2] == playerID)
      {

        //for turning right
        // advance round the compass clockwise 1 position
        if (turnVal == 'r') {
          if (matrix[x][y][3] == 4)
          {
            matrix[x][y][3] = 1;
          } else {
            matrix[x][y][3] = matrix[x][y][3] + 1;
          }
        }
        //for turning left
        // advance round the compass anticlockwise 1 position
        if (turnVal == 'l') {
          if (matrix[x][y][3] == 1)
          {
            matrix[x][y][3] = 4;
          } else {
            matrix[x][y][3] = matrix[x][y][3] - 1;
          }
        }

      }
    }
  }
}

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
  
  
  while(!gameStarted) {
      VoiceRecognition.start();
      value = (char) xBee.read();

      if (VoiceRecognition.isNewCommandReceived()) {
        if(strstr(VoiceRecognition.getLastCommand(), startGameCommand)) {
          gameStarted = true;
          break;           
        }
      }
      else if(value == 'I') {
        ++connectionCount;
        delay(100);
        xBee.write(connectionCount / 256);
        xBee.write(connectionCount % 256);
        xBee.write(0x0A);

        delay(1000);

        TextToSpeech.say("Player " + String(connectionCount) + " say your name.");
        
        delay(2000);

        bool nicknameAssigned = false;
        while(!nicknameAssigned) {
          VoiceRecognition.start();
          if (VoiceRecognition.isNewCommandReceived()) {
            
            Terminal.println("New command received");
            
            String nickname = VoiceRecognition.getLastCommand();
  
            Terminal.println("yaname - " + nickname);
  
            Player* player = new Player(connectionCount, nickname);
            players.push_back(player);
  
            int playerID = players[0]->getPlayerID();
            String playerNickname = players[0]->getNickname();
            TextToSpeech.say("Player " + String(playerID) + " is called " + playerNickname);
            Terminal.println("Player " + String(playerID) + " is called " + playerNickname);

            nicknameAssigned = true;
            break;
          }          
        }
        
      }    
  }

  TextToSpeech.say("Game is now starting.");
  
}

void loop()
{
  // At least 2 players are required to play the game
  //if(connectionCount > 1) {
    
    VoiceRecognition.start();
    
    if(VoiceRecognition.isNewCommandReceived()) {
      if(strstr(VoiceRecognition.getLastCommand(), forwardCommand) && gameStarted) {
        moveZumo(playerToMoveNext, 'w');
        nextPlayersTurn();
        
      }
      else if(strstr(VoiceRecognition.getLastCommand(), leftCommand) && gameStarted) {
        moveZumo(playerToMoveNext, 'a');
        nextPlayersTurn();
                        
      }
      else if(strstr(VoiceRecognition.getLastCommand(), backwardCommand) && gameStarted) {
        moveZumo(playerToMoveNext, 's');
        nextPlayersTurn();
                
      }
      else if(strstr(VoiceRecognition.getLastCommand(), rightCommand) && gameStarted) {
        moveZumo(playerToMoveNext, 'd');
        nextPlayersTurn();
            
      }
  
  
      // Update Zumo's current location (cell number)
  
      // Check if new cell contains a bomb
            
    
    }  
  //}
}

void nextPlayersTurn() {
  if(connectionCount == playerToMoveNext) {
    playerToMoveNext = 1;
  } else {
    playerToMoveNext++;
  }

  TextToSpeech.say("Next Players Turn");
  
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
