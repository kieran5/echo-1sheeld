#define CUSTOM_SETTINGS
#define INCLUDE_VOICE_RECOGNIZER_SHIELD
#define INCLUDE_TERMINAL_SHIELD
#define INCLUDE_TEXT_TO_SPEECH_SHIELD

/* Include 1Sheeld library. */
#include <OneSheeld.h>

// Initialise previous milliseconds for timer used in loop function
unsigned long prevMs = 0;

// Flag to check if user has said the initial command to activate the shield
bool isActivated = false;

/* Voice commands initialisation */
const char initialCommand[] = "john";

const char robotGoForwardCommand[] = "robot go forward";
const char robotGoBackwardsCommand[] = "robot go backwards";
const char robotGoLeftCommand[] = "robot go left";
const char robotGoRightCommand[] = "robot go right";
char* s;


void setup()
{
  /* Start Communication. */
  OneSheeld.begin();
  /* Error Commands handiling. */
  VoiceRecognition.setOnError(error);

  // This will trigger the new command function each time a new voice command is recognised
  VoiceRecognition.setOnNewCommand(newCommand);

}

void loop ()
{

  // Get current milliseconds for creating the timer
  unsigned long currentMs = millis();

  // Voice recognition listens out every 3 seconds for a new command
  // This setup allows the voice recognition to be constantly active and listening out
  // Won't have to interact with the smartphone at all
  if (currentMs - prevMs >= 2500)
  {
    prevMs = currentMs;

    VoiceRecognition.start();
  }


}

void newCommand(String command)
{
  // The initial command needs to be heard by the shield for it to be activated
  // and other functions become available for use
  if (!strcmp(initialCommand, VoiceRecognition.getLastCommand()))
  {
    Serial.println("John called, sheild activated.");
    TextToSpeech.say("Yes darling?");
    isActivated = true;
  }

  if (strstr(VoiceRecognition.getLastCommand(), robotGoForwardCommand) && isActivated)
  {
    Serial.println("Robot go forward command received.");
    TextToSpeech.say("Go Forward");
    isActivated = false;

  }
  else if (strstr(VoiceRecognition.getLastCommand(), robotGoBackwardsCommand) && isActivated)
  {
    Serial.println("Robot go backwards command received.");
    TextToSpeech.say("Go Backwards");
    isActivated = false;
  }

  else if (strstr(VoiceRecognition.getLastCommand(), robotGoLeftCommand) && isActivated)
  {
    Serial.println("Robot go left command received.");
    TextToSpeech.say("Go Left");
    isActivated = false;

  }
  else if (strstr(VoiceRecognition.getLastCommand(), robotGoRightCommand) && isActivated)
  {
    Serial.println("Robot go right command received.");
    TextToSpeech.say("Go Right");
    isActivated = false;

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
