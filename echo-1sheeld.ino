#define CUSTOM_SETTINGS
#define INCLUDE_VOICE_RECOGNIZER_SHIELD
#define INCLUDE_TERMINAL_SHIELD
#define INCLUDE_TEXT_TO_SPEECH_SHIELD
#define INCLUDE_INTERNET_SHIELD

#include <OneSheeld.h>

String nickname = "";
int score = 0;
String scoreString = (String) score;
static String jsonString1 = "{\"nickname\": ";
static String jsonString2 = ", \"score\": ";
static String jsonString3 = "}";

HttpRequest scoreRequest("http://kwillis.eu/hiscores/");

void setup()
{
  OneSheeld.begin();
  scoreRequest.addHeader("Content-Type", "application/x-www-form-urlencoded");
  scoreRequest.setOnSuccess(&onSuccess);
}

void loop ()
{  
  Terminal.println(&(jsonString1+nickname+jsonString2+scoreString+jsonString3)[0]);
  scoreRequest.addRawData(&(jsonString1+nickname+jsonString2+scoreString+jsonString3)[0]);
  Internet.performPost(scoreRequest);
}

void onSuccess (HttpResponse &response)
{
 Terminal.println("Request succeeded"); 
}
