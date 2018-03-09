#define CUSTOM_SETTINGS
#define INCLUDE_TERMINAL_SHIELD
#define INCLUDE_INTERNET_SHIELD

#include <OneSheeld.h>
#include <AltSoftSerial.h>

AltSoftSerial xbee;



String nickname = "Tyrone";
int score = 420;

String scoreString = (String) score;
static String jsonString1 = "{\"nickname\": \"";
static String jsonString2 = "\", \"score\": ";
static String jsonString3 = "}";
String jsonObject = jsonString1 + nickname + jsonString2 + scoreString + jsonString3;

HttpRequest scoreRequest("https://kwillis.eu/hiscores");

void setup()
{
  OneSheeld.begin();
  xbee.begin(9600);
  
  
  scoreRequest.setOnSuccess(&onSuccess);
  scoreRequest.setOnFailure(&onFailure);
  scoreRequest.addHeader("Content-Type", "application/json");

  Terminal.println(&(jsonObject)[0]);
  scoreRequest.addRawData(&(jsonObject)[0]);
  
  Internet.performPost(scoreRequest);
}

void loop()
{
  xbee.println("Hello John");
}

void onSuccess (HttpResponse &res)
{
  Terminal.println("Request succeeded");
  Terminal.println(res.getStatusCode());
  Terminal.println(res.getTotalBytesCount());
}
 
void onFailure (HttpResponse &res)
{
  Terminal.println("Request failed");
  Terminal.println(res.getStatusCode());
  Terminal.println(res.getTotalBytesCount());
}

