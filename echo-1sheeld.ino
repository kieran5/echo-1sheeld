/*

  /

*/

#define CUSTOM_SETTINGS
#define INCLUDE_VOICE_RECOGNIZER_SHIELD
#define INCLUDE_TERMINAL_SHIELD
#define INCLUDE_TEXT_TO_SPEECH_SHIELD

/* Include 1Sheeld library. */
#include <OneSheeld.h>

/* Voice commands initialisation */
//const char testCommand[] = "robot";

const char robotGoForwardCommand[] = "robot go forward";
const char robotGoBackwardsCommand[] = "robot go backwards";
const char robotGoLeftCommand[] = "robot go left";
const char robotGoRightCommand[] = "robot go right";
const char robotStopCommand[] = "robot stop";
char* s;


void setup()
{
  /* Start Communication. */
  Serial.begin(9600);
  OneSheeld.begin();
  /* Error Commands handiling. */
  VoiceRecognition.setOnError(error);

  VoiceRecognition.start();

}

void loop ()
{
  /* Check if new command received. */
  if (VoiceRecognition.isNewCommandReceived())
  {
    //Terminal.println(VoiceRecognition.getLastCommand());
    //Terminal.println(robotGoForwardCommand);

    if (strstr(VoiceRecognition.getLastCommand(), robotGoForwardCommand))
    {
      Serial.println("w");
      //Terminal.println("Robot go forward command received.");
      //TextToSpeech.say("Go Forward");
      
    }
    else if (strstr(VoiceRecognition.getLastCommand(), robotGoBackwardsCommand))
    {
      Serial.println("s");
      //Terminal.println("Robot go backwards command received.");
      //TextToSpeech.say("Go Backwards");

    }
    else if (strstr(VoiceRecognition.getLastCommand(), robotGoLeftCommand))
    {
      Serial.println("a");
      //Terminal.println("Robot go left command received.");
      //TextToSpeech.say("Go Left");
    }
    else if (strstr(VoiceRecognition.getLastCommand(), robotGoRightCommand))
    {
      Serial.println("d");
      //Terminal.println("Robot go right command received.");
      //TextToSpeech.say("Go Right");
    }
    else if (strstr(VoiceRecognition.getLastCommand(), robotStopCommand))
    {
      Serial.println("q");
      //Terminal.println("Robot stop command received.");
      //TextToSpeech.say("Stop");
    }



  }
}



/* Error checking function. */
void error(byte errorData)
{
  /* Switch on error and print it on the terminal. */
  switch (errorData)
  {
    case NETWORK_TIMEOUT_ERROR: Terminal.println("Network timeout"); break;
    case NETWORK_ERROR: Terminal.println("Network Error"); break;
    case AUDIO_ERROR: Terminal.println("Audio error"); break;
    case SERVER_ERROR: Terminal.println("No Server"); break;
    case SPEECH_TIMEOUT_ERROR: Terminal.println("Speech timeout"); break;
    case NO_MATCH_ERROR: Terminal.println("No match"); break;
    case RECOGNIZER_BUSY_ERROR: Terminal.println("Busy"); break;
  }
}

