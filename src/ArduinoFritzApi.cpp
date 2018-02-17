#include <MD5Builder.h>

#include "ArduinoFritzApi.h"

// TODO: Add Compiler Directive for logging stuff ... #define FRITZ_API_DEBUG

FritzApi::FritzApi(const char* user, const char* password, const char* ip) {
  _user = user;
  _pwd = password;
  _ip = ip;
}

FritzApi::~FritzApi(){
  _user="";
  _pwd="";
  _ip="";
}

void FritzApi::init() {
   String response = getChallengeResponse();
   _sid = getSID(response);
   Serial.println("SID: " + _sid);
   if (_sid == "") {
	 throw FRITZ_ERR_EMPTY_SID;
   } else if (_sid == "0000000000000000") {
  	 throw FRITZ_ERR_INVALID_SID;   	
   }
}

String FritzApi::getChallengeResponse() {
   // Get Challenge
   http.begin("http://" + String(_ip) + "/login_sid.lua");
   int retCode = http.GET();
   if (retCode < 0) {
	 throw FRITZ_ERR_HTTP_COMMUNICATION;
   } else if (retCode != 200) {
	 throw FRITZ_ERR_NO_CHALLENGE;
   }
   String result = http.getString();
   String challenge = result.substring(result.indexOf("<Challenge>") + 11, result.indexOf("</Challenge>"));
   String challengeReponse = challenge + "-" + String(_pwd);
   String responseHash = "";
   for (unsigned int i = 0; i  < challengeReponse.length(); i++) {
     // TODO: Sorry, no real UTF-16LE for now... just add 00
     responseHash = responseHash + String(challengeReponse.charAt(i), HEX) + "00";
   }

   MD5Builder md5;
   md5.begin();
   md5.addHexString(responseHash);
   md5.calculate();
   
   return challenge + "-" + md5.toString();
}

String FritzApi::getSID(String response) {
  http.begin("http://" + String(_ip) + "/login_sid.lua?user=" + String(_user) + "&response=" + response);
  int retCode = http.GET();
  if (retCode < 0) {
    throw FRITZ_ERR_HTTP_COMMUNICATION;
  } else if (retCode != 200) {
    throw FRITZ_ERR_NO_SID;
  }
  String result = http.getString();
  // TODO: check indexOf > 0
  String sid = result.substring(result.indexOf("<SID>") + 5,  result.indexOf("</SID>"));
  return sid;
}

boolean FritzApi::setSwitchOn(String ain) {
  String result = executeRequest("setswitchon&sid=" + _sid + "&ain=" + String(ain));
  return (atoi(result.c_str()) == 1);
}

boolean FritzApi::setSwitchOff(String ain) {
  String result = executeRequest("setswitchoff&sid=" + _sid + "&ain=" + String(ain));
  return (atoi(result.c_str()) == 1);
}

boolean FritzApi::setSwitchToggle(String ain) {
  String result = executeRequest("setswitchtoggle&sid=" + _sid + "&ain=" + String(ain));
  return (atoi(result.c_str()) == 1);
}

boolean FritzApi::getSwitchState(String ain) {
  String result = executeRequest("getswitchstate&sid=" + _sid + "&ain=" + String(ain));
  return (atoi(result.c_str()) == 1);
}

boolean FritzApi::getSwitchPresent(String ain) {
  String result = executeRequest("getswitchpresent&sid=" + _sid + "&ain=" + String(ain));
  return (atoi(result.c_str()) == 1);
}

double FritzApi::getSwitchPower(String ain) {
  String result = executeRequest("getswitchpower&sid=" + _sid + "&ain=" + String(ain));
  if (result == "inval") {
  	throw FRITZ_ERR_VALUE_NOT_AVAILABLE;
  }
  return atof(result.c_str()) / 1000;
}

int FritzApi::getSwitchEnergy(String ain) {
  String result = executeRequest("getswitchenergy&sid=" + _sid + "&ain=" + String(ain));
  if (result == "inval") {
  	throw FRITZ_ERR_VALUE_NOT_AVAILABLE;
  }
  return atoi(result.c_str());
}

double FritzApi::getTemperature(String ain) {
  String result = executeRequest("gettemperature&sid=" + _sid + "&ain=" + String(ain));
  if (result == "inval") {
  	throw FRITZ_ERR_VALUE_NOT_AVAILABLE;
  }
  return atof(result.c_str()) / 10;
}

String FritzApi::getSwitchName(String ain) {
  return executeRequest("getswitchname&sid=" + _sid + "&ain=" + String(ain));
}

double FritzApi::getThermostatNominalTemperature(String ain) {
  String result = executeRequest("gethkrtsoll&sid=" + _sid + "&ain=" + String(ain));
  if (result == "inval") {
  	throw FRITZ_ERR_VALUE_NOT_AVAILABLE;
  }
  return convertTemperature(result);
}

double FritzApi::getThermostatComfortTemperature(String ain) {
  String result = executeRequest("gethkrkomfort&sid=" + _sid + "&ain=" + String(ain));
  if (result == "inval") {
  	throw FRITZ_ERR_VALUE_NOT_AVAILABLE;
  }
  return convertTemperature(result);
}

double FritzApi::getThermostatReducedTemperature(String ain) {
  String result = executeRequest("gethkrabsenk&sid=" + _sid + "&ain=" + String(ain));
  if (result == "inval") {
  	throw FRITZ_ERR_VALUE_NOT_AVAILABLE;
  }
  return convertTemperature(result);
}

double FritzApi::setThermostatNominalTemperature(String ain, double temp) {
  int param;
  if (temp == FRITZ_OFF_TEMPERATURE) {
    param=253;
  } else if (temp == FRITZ_ON_TEMPERATURE) {
    param=254;
  } else if (temp < 8.0f) {
    param=16;
  } else if (temp > 28.0f) {
    param=56;
  } else {
    param = 2*temp;
  }
  String result = executeRequest("sethkrtsoll&sid=" + _sid + "&ain=" + String(ain) + "&param=" + param);
  return temp;
}

String FritzApi::executeRequest(String request) {
  String result;
  int httpStatus;
  for (int retries = 0; retries < 3; retries++) {
    http.begin("http://" + String(_ip) + "/webservices/homeautoswitch.lua?switchcmd=" + request);
    httpStatus = http.GET();
    if (httpStatus == 200) {
      return http.getString();
    } else {
      // Try to get new Session
      init();
    }
  }
  if (httpStatus < 0) {
    throw FRITZ_ERR_HTTP_COMMUNICATION;
  } else {
    throw httpStatus;
  }
}

double FritzApi::convertTemperature(String result) {
  double temp = atof(result.c_str());
  if (temp == 254) {
    return FRITZ_ON_TEMPERATURE;
  } else if (temp == 253 ){
    return FRITZ_OFF_TEMPERATURE;
  }
  return temp/2;
}