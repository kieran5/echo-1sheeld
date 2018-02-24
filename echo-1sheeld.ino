/*

/

*/

#define CUSTOM_SETTINGS
#define INCLUDE_VOICE_RECOGNIZER_SHIELD
#define INCLUDE_TERMINAL_SHIELD

/* Include 1Sheeld library. */
#include <OneSheeld.h>

/* Voice commands initialisation */
const char zumoTestCommand[] = "zumo";

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
    if(!strcmp(zumoTestCommand,VoiceRecognition.getLastCommand()))
    {
      /* Play the track. */
      Serial.println("Zumo command received.");
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
