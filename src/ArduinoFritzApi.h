#ifndef FRITZ_API_H
#define FRITZ_API_H

#define FRITZ_API_DEBUGGING

#include <HTTPClient.h>

struct FritzResult { int httpStatus; int statusCode; String response; }; 

class FritzApi {
  public:
    // Constructor: FB user, FB password, FB address (ip or 'fritz.box')
    FritzApi(char* user, char* password, char* ip); 
    ~FritzApi();

    void init();
#ifdef FRITZ_API_DEBUGGING
    void setDebugLog( Print &print );
#endif

    FritzResult setSwitchOn(String ain);
    FritzResult setSwitchOff(String ain);
    FritzResult setSwitchToggle(String ain);
    FritzResult getSwitchState(String ain);
    FritzResult getSwitchPresent(String ain);
    FritzResult getSwitchPower(String ain);
    FritzResult getSwitchEnergy(String ain);

  private:
    char* _user;
    char* _pwd;
    char* _ip;
    String _sid;
    HTTPClient http;

#ifdef FRITZ_API_DEBUGGING
    Print* logger;
    void logPrintln(String entry);
#endif

    String getChallengeResponse();
    String getSID(String response);
    FritzResult executeRequest(String request);
    void mapResultToIntStatus(FritzResult* result);
    
};
  
#endif // FRITZ_API_H
