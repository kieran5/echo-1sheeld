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


void setup()
{
  /* Start Communication. */
  OneSheeld.begin();
  /* Error Commands handiling. */
  VoiceRecognition.setOnError(error);
  
  VoiceRecognition.start();

}

void loop () 
{
  /* Check if new command received. */
  if(VoiceRecognition.isNewCommandReceived())
  {    
    /* Compare the command. */
    if(!strcmp(robotGoForwardCommand,VoiceRecognition.getLastCommand()))
    {
      /* Play the track. */
      Serial.println("Robot go forward command received.");
      TextToSpeech.say("Go Forward");
    }
    else if(!strcmp(robotGoBackwardsCommand,VoiceRecognition.getLastCommand()))
    {
      Serial.println("Robot go backwards command received.");
      TextToSpeech.say("Go Backwards");            
    }
    else if(!strcmp(robotGoLeftCommand,VoiceRecognition.getLastCommand()))
    {
      Serial.println("Robot go left command received.");
      TextToSpeech.say("Go Left");

    }
    else if(!strcmp(robotGoRightCommand,VoiceRecognition.getLastCommand()))
    {
      Serial.println("Robot go right command received.");
      TextToSpeech.say("Go Right");
            
    }


    
  }
}



/* Error checking function. */
void error(byte errorData)
{
  /* Switch on error and print it on the terminal. */
  switch(errorData)
  {
    case NETWORK_TIMEOUT_ERROR: Terminal.println("Network timeout");break;
    case NETWORK_ERROR: Terminal.println("Network Error");break;
    case AUDIO_ERROR: Terminal.println("Audio error");break;
    case SERVER_ERROR: Terminal.println("No Server");break;
    case SPEECH_TIMEOUT_ERROR: Terminal.println("Speech timeout");break;
    case NO_MATCH_ERROR: Terminal.println("No match");break;
    case RECOGNIZER_BUSY_ERROR: Terminal.println("Busy");break;
  }
}
