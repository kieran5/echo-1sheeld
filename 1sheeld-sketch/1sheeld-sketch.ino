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


//Set up SoftwareSerial for our xBee comms (AltSoftSerial using Rx Tx pins 5 and 13 on Arduino Leonardo board)
AltSoftSerial xBee;

std::vector<Player*> players;

char value;

int connectionCount = 0;

//Our API route for handling score data
HttpRequest scoreRequest("https://kwillis.eu/hiscores");

// Voice recognition
// Commands to be recognised by Zumos
const char forwardCommand[] = "forward";
const char backwardCommand[] = "backward";
const char leftCommand[] = "left";
const char rightCommand[] = "right";

// Variable so the 1Sheeld knows who's turn it is next
int currentPlayer = 1;

int gameMatrix[4][4][2]; //the 3dimensional array that holds the game state
//[0] 0 for empty or a zumo's ID
//[1] 0 for empty or a bomb's ID

int zumoDetails[4][3]; //a 2 dimensional array to hold zumo details
//[0] Zumo orientation NESW as 1234
//[1] Does it have a bomb, 0 for no, 1 for yes
//[2] Score

// Last connected time variable to be used in the timer for connecting new players
unsigned long lastConnectedTime = millis();


void setup()
{
  //Set up serial on 115200 baud rate for our 1Sheeld+
  OneSheeld.begin();

  //Set up our xBee on 9600 baud rate (needs to be at a lower baud rate than our UART serial, as our UART is handling lock interrupts faster
  xBee.begin(9600);
  xBee.println("Sheeld First Broadcast");

  // Ten second timer to connect a new player
  // After no players are connected for 10 seconds, the game will automatically begin
  while (millis() - lastConnectedTime < 10000) {
    if (xBee.available() > 0) {

      value = (char)xBee.read();

      // When the Zumo is turned on & its button is pressed
      // It sends an 'I' to the 1Sheeld
      switch (value) {
        case 'I':
          // We increment the connection count variable
          // Send this number across to the Zumo to assign as it's unique ID
          ++connectionCount;
          xBee.println(connectionCount);

          assignNicknameToPlayer();

          break;
      }
    }
  }
  // Confirm to players how many of them have connected
  String numPlayers = String(connectionCount);
  TextToSpeech.say(numPlayers + "players connected. Player 1, it's your turn.");
  delay(4000);

  // Activate voice recognition ready for first players command
  VoiceRecognition.start();
}

void loop()
{
  // Reads in voice command from user and sends
  // communication to Zumo to tell it which way to move
  if (VoiceRecognition.isNewCommandReceived()) {
    if (strstr(VoiceRecognition.getLastCommand(), forwardCommand)) {
      if (canMove(currentPlayer)) {
        moveZumo(currentPlayer, 'w');
        delay(3000);
        nextPlayersTurn();
      }
    }
    else if (strstr(VoiceRecognition.getLastCommand(), leftCommand)) {
      // Zumo always knows what direction it is facing
      // We have to change orientation to check if the Zumo can move that way
      // If so, we tell the Zumo to move. If not, we change the orientation back to its original state
      // Then ask player to give a valid command
      setOrientation(currentPlayer, 'l');

      if (canMove(currentPlayer)) {
        moveZumo(currentPlayer, 'a');
        delay(3000);
        nextPlayersTurn();
      }
      else {
        setOrientation(currentPlayer, 'r');
      }
    }
    else if (strstr(VoiceRecognition.getLastCommand(), backwardCommand)) {
      setOrientation(currentPlayer, 'l');
      setOrientation(currentPlayer, 'l');

      if (canMove(currentPlayer)) {
        moveZumo(currentPlayer, 's');
        delay(3000);
        nextPlayersTurn();
      }
      else {
        setOrientation(currentPlayer, 'r');
        setOrientation(currentPlayer, 'r');
      }
    }
    else if (strstr(VoiceRecognition.getLastCommand(), rightCommand)) {
      setOrientation(currentPlayer, 'r');

      if (canMove(currentPlayer)) {
        moveZumo(currentPlayer, 'd');
        delay(3000);
        nextPlayersTurn();
      }
      else {
        setOrientation(currentPlayer, 'l');
      }
    }
    return;
  }

  // Awaiting for user to press the push button on 1Sheeld app to drop
  // a bomb on the square they are currently on
  if (PushButton.isPressed()) {
    setBomb(currentPlayer);
  }
}

/**
  @ author Kieran
  @ description Every time a Zumo is connected, this method will be called to prompt the user to
  say their name to be assigned to their corrosponding Zumo
*/
void assignNicknameToPlayer() {
  bool nickAssigned = false;

  TextToSpeech.say("Say your name now.");
  delay(2000); //so text to speech can talk without affecting voice recognition
  VoiceRecognition.start();
  delay(5000); //so player has time to say their name after start()

  while (!nickAssigned) {

    if (VoiceRecognition.isNewCommandReceived()) {
      String nickname = VoiceRecognition.getLastCommand();

      // Once a nickname has been recognised and assigned
      // We can create a new Player object and initialise with the players ID & nickname
      Player* player = new Player(connectionCount, nickname);

      // Push new player object to our vector so we can access throughout
      players.push_back(player);

      // Change bool to true to break out of while loop
      nickAssigned = true;

      // Set Zumo on our grid and notify user that they have successfully registered
      setZumo(connectionCount);
      TextToSpeech.say("Player " + String(connectionCount) + " successfully registered.");

    } else {
      Terminal.println("false");
    }
  }
  // Reset 10 second timer
  lastConnectedTime = millis();
}

/**
  @ author Kieran
  @ description This method will be called every time a valid move is made by a player.
  This method will ensure the next player is chronologically selected and is still alive before breaking out & letting the game continue
*/
void nextPlayersTurn() {
  // If last players turn, then needs to go back to players 1's turn
  if (connectionCount == currentPlayer) {
    currentPlayer = 1;
  } else {
    currentPlayer++;
  }

  // Check current player is alive before saying it is their turn
  // If not we use recursion & call this method again until we find
  // a player that is alive to break out of the method
  if (players[currentPlayer - 1]->isAlive()) {
    TextToSpeech.say("Player " + String(currentPlayer) + "'s turn");
    delay(1000);
    VoiceRecognition.start();
  }
  else {
    nextPlayersTurn();
  }
}

/**
   @ author  Frank
   @ description  Update the position of the zumo in the matrix and check if it survives

   @param playerID  Integer representing the zumo to be handled
   @param dir  Char representing the direction of the zumo
*/
void moveZumo(int playerID, char dir)
{
  String toSend = String(playerID) + ":" + dir;
  xBee.println('M');
  xBee.println(toSend);

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
          //Give that plkayer's zumo a new bomb
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
          delay(2000);
          xBee.println('M');
          toSend = String(playerID) + ":b";
          xBee.println(toSend);
          players[playerID - 1]->die();
          //the current player's zumo is dead. Post their score.
          postScore(playerID, zumoDetails[playerID - 1][2]);
          return;
        }
        //if you haven't hit a bomb
        //move the representation of the zumo to the new square
        gameMatrix[newX][newY][0] = gameMatrix[x][y][0];
        //clear the previous square
        gameMatrix[x][y][0] = 0;
        //give a point for surviving a turn
        zumoDetails[playerID - 1][2] = zumoDetails[playerID - 1][2] + 1;
        return;
      }
    }
  }
}


/**
  @author      Jake Slade
  @description This method retrieves a nickname and score for the associated player ID from an instantiation of Player class,
               adds it to an HTTP request and submits it to our REST api and adds it to the game highscores
  @params      The player ID of the users details you would like to submit to the hiscores
  @returns     void
*/
void submitScore(int playerID)
{
  scoreRequest.setOnSuccess(&onSuccess);
  scoreRequest.setOnFailure(&onFailure);
  scoreRequest.addHeader("Content-Type", "application/json");
  scoreRequest.setContentType("application/json");
  //Our player's name and their score
  String nickname = players[playerID - 1]->getNickname();
  int score = players[playerID - 1]->getScore();

  //A string to construct a JSON object containing users nickname and score, so we can make API request with data
  String jsonObject = "{\"nickname\": \"" + nickname + "\", \"score\": " + String(score) + "}";

  Terminal.println(&(jsonObject)[0]);

  //Add data to our HTTP request and post to our API
  scoreRequest.addRawData(&(jsonObject)[0]);

  Internet.performPost(scoreRequest);
}

/**
  @author      Jake Slade
  @description This method is invoked when our HTTP request fails. It shows us the status code of the failure and the amount of data.
  @params      A reference of the HTTP protocol response
  @returns     void
*/
void onFailure(HttpResponse& res)
{
  //Do below on an unsuccessful HTTP request
  Terminal.println("Request failed");
  Terminal.println(res.getStatusCode());
  Terminal.println(res.getTotalBytesCount());
}

/**
  @author      Jake Slade
  @description This method is invoked when our HTTP request succeeds. It shows us the status code of the failure and the amount of data.
  @params      A reference of the HTTP protocol response
  @returns     void
*/
void onSuccess(HttpResponse& res)
{
  //Do below on a successful HTTP request
  Terminal.println("Request succeeded");
  Terminal.println(res.getStatusCode());
  Terminal.println(res.getTotalBytesCount());
}

/**
   @ author  Frank
   @ description  check a tile to see if a bomb is present

   @param  x  Integer representing the x co-ordinate
   @param  y  Integer representing the y co-ordinate
   @return boolean specifying whether a bomb was found
*/
bool containsBomb(int x, int y) {
  if (gameMatrix[x][y][1] != 0)
  {
    return true;
  }
  return false;
}

/**
   @ author  Frank
   @ description  add a new player's zumo to the game

   @param playerID  Integer representing the zumo to be handled
*/
void setZumo(int playerID) {
  //set the zumo up in the right column
  gameMatrix[playerID - 1][0][0] = playerID;
  //set the orientation
  zumoDetails[playerID - 1][0] = 1;
  //give it a bomb
  zumoDetails[playerID - 1][1] = 1;
  //reset the score
  zumoDetails[playerID - 1][2] = 0;
}

/**
   @ author  Frank
   @ description  place a bomb in the same game tile as the current player's zumo

   @param playerID  Integer representing the zumo to be handled
*/
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

/**
   @ author  Frank
   @ description  if only one player remains, command them to play a victory tune and pass their details to the submitScore function

   @param playerID  Integer representing the zumo to be handled
   @param playerScore  Integer representing the player's score
*/
void postScore(int playerID, int playerScore)
{


  //check if there's a last man standing
  int count = 0;
  int lastZumo = 0;
  int lastMansX = 0;
  int lastMansY = 0;
  //look for zumos
  for (int x = 0; x < 4; x++)
  {
    for (int y = 0; y < 4; y++)
    {
      if (gameMatrix[x][y][0] != 0)
      {
        //count the ones you find and remember the last one you've found
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
    delay(5000);
    // Game over
    TextToSpeech.say("Game over. Player" + String(lastZumo) + " wins!");
    delay(3000);
    xBee.println('M');
    String sendVictory = String(lastZumo) + ":v";
    xBee.println(sendVictory);
    delay(2000);
    //remove it from the game matrix
    gameMatrix[lastMansX][lastMansY][0] = 0;
    //post its score
    players[lastZumo - 1]->setScore(zumoDetails[lastZumo - 1][2]);
    submitScore(lastZumo);
    delay(2000);
    // Enter endless while loop to terminate program
    while (true) {}
  }
}

/**
   @ author  Frank
   @ description  keep track of the player's zumo making 90 degree turns

   @param playerID  Integer representing the zumo to be handled
   @param turnVal  Char representing the turn direction
*/
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

/**
   @ author  Frank
   @ description  check if a proposed move is possible without violating game rules

   @param playerID  Integer representing the zumo to be handled
   @return Boolean representing whether a proposed move is possible
*/
bool canMove(int playerID) {
  for (int x = 0; x < 4; x++)
  {
    for (int y = 0; y < 4; y++)
    {
      if (gameMatrix[x][y][0] == playerID) {
        if ((y == 3 && zumoDetails[playerID - 1][0] == 1) ||
            (x == 3 && zumoDetails[playerID - 1][0] == 2) ||
            (y == 0 && zumoDetails[playerID - 1][0] == 3) ||
            (x == 0 && zumoDetails[playerID - 1][0] == 4))
        {
          TextToSpeech.say("Invalid move. You can't leave the grid.");
          delay(1000);
          VoiceRecognition.start();
          return false;
        }
        if ((zumoDetails[playerID - 1][0] == 1 && gameMatrix[x][y + 1][0] != 0) ||
            (zumoDetails[playerID - 1][0] == 2 && gameMatrix[x + 1][y][0] != 0) ||
            (zumoDetails[playerID - 1][0] == 3 && gameMatrix[x][y - 1][0] != 0) ||
            (zumoDetails[playerID - 1][0] == 4 && gameMatrix[x - 1][y][0] != 0))
        {
          TextToSpeech.say("Invalid move. There is a player in the way.");
          delay(1000);
          VoiceRecognition.start();
          return false;
        }
      }
    }
  }
  return true;
}
