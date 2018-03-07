#define CUSTOM_SETTINGS
#define INCLUDE_VOICE_RECOGNIZER_SHIELD
#define INCLUDE_TERMINAL_SHIELD
#define INCLUDE_TEXT_TO_SPEECH_SHIELD
#define INCLUDE_INTERNET_SHIELD

#include <OneSheeld.h>

HttpRequest scoreRequest("http://kwillis.eu/hiscores/");

void setup()
{
  OneSheeld.begin();
  scoreRequest.addHeader("Content-Type", "application/x-www-form-urlencoded");
  scoreRequest.setOnSuccess(&onSuccess);
}

void loop ()
{  
  Internet.performPost(scoreRequest);
}

void onSuccess (HttpResponse &response)
{
 Terminal.println("Request succeeded"); 
}
