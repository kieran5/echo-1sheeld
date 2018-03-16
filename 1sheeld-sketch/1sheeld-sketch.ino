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


int gameMatrix[4][4][2]; //the 3dimensional array that holds the game state
//[0] 0 for empty or a zumo's ID
//[1] 0 or empty or a bomb's ID

int zumoDetails[4][3]; //a 2 dimensional array to hold zumo details, limited to 2 to keep memory usage minimal
//[0] zumo ID
//[1] Zumo orientation NESW as 1234
//[2] Does it have a bomb, 0 for no, 1 for yes
//[3] Score




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

//should be called after the zumo has physically moved. returns true if the move is safe, returns false if the zumo is dead.
bool zumoHasMoved(int playerID) {
  int newX;
  int newY;

  //search the array for the correct zumo, by looping through the array a row at a time and checking if each cell contains a zumo with the correct user id
  for (int x = 0; x < 4; x++)
  {
    for (int y = 0; y < 4; y++)
    {
      if (gameMatrix[x][y][0] == playerID)
      {

        //set the co-ordinates that the zumo has moved to
        switch (zumoDetails[playerID - 1][0])
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
            newX = x - 1;
            newY = y;
            break;
        }


        if (containsBomb(newX, newY))
        {
          //take the id of the player who left the bomb
          int id = gameMatrix[newX][newY][1];

          //remove the bomb
          gameMatrix[newX][newY][1] = 0;

          //Give that zumo a new bomb
          zumoDetails[id - 1][1] = 1;
          // increase its score by 5 points
          zumoDetails[id - 1][2] = zumoDetails[id - 1][2] + 5;

          //if they have deployed their bomb, find it and remove it
          if (zumoDetails[playerID - 1][1] == 0)
          {
            for (int x = 0; x < 4; x++)
            {
              for (int y = 0; y < 4; y++)
              {
                if (gameMatrix[x][y][1] == playerID)
                {
                  gameMatrix[x][y][1] = 0;
                }
              }
            }
          }

          //remove their zumo from the grid
          gameMatrix[x][y][0] = 0;

          //the current player's zumo is dead. Post their score.
          postScore(playerID, zumoDetails[playerID - 1][2]);

          return false;
        }
        //if you haven't hit a bomb
        //move the representation of the zumo to the new square
        gameMatrix[newX][newY][0] = gameMatrix[x][y][0];
        //clear the previous square
        gameMatrix[x][y][0] = 0;

        //give a point for surviving a turn
        zumoDetails[playerID - 1][2] = zumoDetails[playerID - 1][2] + 1;

        return true;
      }
    }
  }
}




//returns true if the co-ordinates contain a bomb, and false if they do not
bool containsBomb(int x, int y) {
  if (gameMatrix[x][y][1] != 0)
  {
    return true;
  }
  return false;
}

void setZumo(int playerID) {
  gameMatrix[playerID - 1][0][0] = playerID;
  zumoDetails[playerID - 1][0] = 1;
  zumoDetails[playerID - 1][1] = 1;
  zumoDetails[playerID - 1][2] = 0;
}

//call this when a player signals they want to drop their bomb. Players should drop their bomb before they move
void setBomb(int playerID) {
  //if the player has a bomb
  if (zumoDetails[playerID - 1][1] == 1) {
    //search for the players zumo and drop a bomb on the same tile
    for (int x = 0; x < 4; x++)
    {
      for (int y = 0; y < 4; y++)
      {
        if (gameMatrix[x][y][0] == playerID)
        {
          gameMatrix[x][y][1] = playerID;
        }
      }
    }
  }
}

//posts a score. is called when a zumo dies, needs to be called for the last man standing, we need to figure out how to tell when the last man is standing.
void postScore(int playerID, int playerScore)
{

  //for testing only, replace this with a function which gets the name associated to the zumo id
  //and posts this name alongside the score
  Serial.print("player: ");
  Serial.println(playerID);
  Serial.print("Score: ");
  Serial.println(playerScore);

  //check if there's a last man standing
  int count = 0;
  int lastZumo = 0;
  int lastMansX = 0;
  int lastMansY = 0;
  //look for zumo's
  for (int x = 0; x < 4; x++)
  {
    for (int y = 0; y < 4; y++)
    {
      if (gameMatrix[x][y][0] != 0)
      {
        //count the ones you find and remember the last on you've found
        //and where it is
        count++;
        lastZumo = gameMatrix[x][y][0];
        lastMansX = x;
        lastMansY = y;
      }
    }
  }
  //if one and only one zumo was found
  if (count == 1)
  {
    //give it ten points for being a top lad
    zumoDetails[lastZumo - 1][2] = zumoDetails[lastZumo - 1][2] + 10;
    //if it had deployed a bomb, remove it from the table
    if (zumoDetails[lastZumo - 1][1] == 0)
    {
      for (int x = 0; x < 4; x++)
      {
        for (int y = 0; y < 4; y++)
        {
          if (gameMatrix[x][y][1] == lastZumo)
          {
            gameMatrix[x][y][1] = 0;
          }
        }
      }
    }
    //remove it from the game matrix
    gameMatrix[lastMansX][lastMansY][0] = 0;
    //post its score
    postScore(lastZumo, zumoDetails[lastZumo - 1][2]);
    //do a victory dance or something?
    //victory(playerID);
  }
}

//tells the zumo if you have turned, call this each time a 90 degree turn is made
void setOrientation(int playerID, char turnVal) {

  //for turning right
  // advance round the compass clockwise 1 position
  if (turnVal == 'r') {
    if (zumoDetails[playerID - 1][0] == 4)
    {
      zumoDetails[playerID - 1][0] = 1;
    } else {
      zumoDetails[playerID - 1][0] = zumoDetails[playerID - 1][0] + 1;
    }
  }
  //for turning left
  // advance round the compass anticlockwise 1 position
  if (turnVal == 'l') {
    if (zumoDetails[playerID - 1][0] == 1)
    {
      zumoDetails[playerID - 1][0] = 4;
    } else {
      zumoDetails[playerID - 1][0] = zumoDetails[playerID - 1][0] - 1;
    }
  }
}

bool canMove(int playerID) {
  for (int x = 0; x < 4; x++)
  {
    for (int y = 0; y < 4; y++)
    {
      if (gameMatrix[x][y][1] == playerID) {
        if((y==3&&zumoDetails[playerID-1][0]==1)||
        (x==3&&zumoDetails[playerID-1][0]==2)||
        (y==0&&zumoDetails[playerID-1][0]==3)||
        (x==0&&zumoDetails[playerID-1][0]==4)){
          return false;
        }
      }
    }
  }
  return true;
}
