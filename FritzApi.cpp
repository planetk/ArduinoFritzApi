#include <MD5Builder.h>

#include "FritzApi.h"

// TODO: Add Compiler Directive for logging stuff ... #define FRITZ_API_DEBUG

FritzApi::FritzApi(char* user, char* password, char* ip) {
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
   logPrintln("Initialising FritzApi for " + String(_ip));
   String response = getChallengeResponse();
   _sid = getSID(response);
   // TODO: Check "" and "0000000000000000"
   logPrintln("Got Session: " + _sid);
}

String FritzApi::getChallengeResponse() {
   // Get Challenge
   http.begin("http://" + String(_ip) + "/login_sid.lua");
   int retCode = http.GET();
   if (retCode != 200) {
     logPrintln("Error retrieving challenge " + retCode);
     return "";
   }
   String result = http.getString();
   // TODO: check indexOf > 0
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
  if (retCode != 200) {
    logPrintln("Error retrieving sessionId " + retCode);
  }
  String result = http.getString();
  // TODO: check indexOf > 0
  String sid = result.substring(result.indexOf("<SID>") + 5,  result.indexOf("</SID>"));
  return sid;
}

//setswitchon Schaltet Steckdose ein
FritzResult FritzApi::setSwitchOn(String ain) {
  logPrintln("Turning switch on: " + ain);
  FritzResult result = executeRequest("setswitchon&sid=" + _sid + "&ain=" + String(ain));
  mapResultToIntStatus(&result);
  return result;
}

//setswitchoff Schaltet Steckdose aus
FritzResult FritzApi::setSwitchOff(String ain) {
  logPrintln("Turning switch off: " + ain);
  FritzResult result = executeRequest ("setswitchoff&sid=" + _sid + "&ain=" + String(ain));
  mapResultToIntStatus(&result);
  return result;
}

//setswitchtoggle Toggeln der Steckdose ein/aus
FritzResult FritzApi::setSwitchToggle(String ain) {
  logPrintln("Toggling switch: " + ain);
  FritzResult result = executeRequest ("setswitchtoggle&sid=" + _sid + "&ain=" + String(ain));
  mapResultToIntStatus(&result);
  return result;
}

//getswitchstate Ermittelt Schaltzustand der Steckdose "0" oder "1" (Steckdose aus oder an), "inval" 
FritzResult FritzApi::getSwitchState(String ain) {
  logPrintln("Getting switch state: " + ain);
  FritzResult result = executeRequest ("getswitchstate&sid=" + _sid + "&ain=" + String(ain));
  mapResultToIntStatus(&result);
  return result;
}

//getswitchpresent Ermittelt Verbindungsstatus des Aktors
FritzResult FritzApi::getSwitchPresent(String ain) {
  logPrintln("Checking switch presence: " + ain);
  FritzResult result = executeRequest ("getswitchpresent&sid=" + _sid + "&ain=" + String(ain));
  mapResultToIntStatus(&result);
  return result;
}

//getswitchpower Ermittelt aktuell über die Steckdose entnommene Leistung (Leistung in mW, "inval" wenn unbekannt)
FritzResult FritzApi::getSwitchPower(String ain) {
  logPrintln("Getting switch power: " + ain);
  FritzResult result = executeRequest ("getswitchpower&sid=" + _sid + "&ain=" + String(ain));
  mapResultToIntStatus(&result);
  return result;
}

//getswitchenergy Liefert die über die Steckdose entnommene Ernergiemenge seit Erstinbetriebnahme oder Zurücksetzen der Energiestatistik (Energie in Wh, "inval" wenn unbekannt)
FritzResult FritzApi::getSwitchEnergy(String ain) {
  logPrintln("Getting switch energy: " + ain);
  FritzResult result = executeRequest ("getswitchenergy&sid=" + _sid + "&ain=" + String(ain));
  mapResultToIntStatus(&result);
  return result;
}



//getswitchlist Liefert die kommaseparierte AIN/MAC Liste aller bekannten Steckdosen
//getswitchpresent Ermittelt Verbindungsstatus des Aktors
//getswitchpower Ermittelt aktuell über die Steckdose entnommene Leistung (Leistung in mW, "inval" wenn unbekannt)
//getswitchenergy Liefert die über die Steckdose entnommene Ernergiemenge seit Erstinbetriebnahme oder Zurücksetzen der Energiestatistik (Energie in Wh, "inval" wenn unbekannt)
//getswitchname Liefert Bezeichner des Aktors Name
//getdevicelistinfos Liefert die grundlegenden Informationen aller SmartHome-Geräte (xml)

//gettemperature Letzte Temperaturinformation des Aktors (Temperatur-Wert in 0,1 °C, negative und positive Werte)
//gethkrtsoll Für HKR aktuell eingestellte Solltemperatur Temperatur-Wert in 0,5 °C,
//  Wertebereich: 16 – 56, 8 bis 28°C, 16 <= 8°C, 17 = 8,5°C...... 56 >= 28°C, 254 = ON , 253 = OFF
//gethkrkomfort Für HKR-Zeitschaltung eingestellte Komforttemperatur Temperatur-Wert in 0,5 °C,
//  Wertebereich: 16 – 56, 8 bis 28°C, 16 <= 8°C, 17 = 8,5°C...... 56 >= 28°C, 254 = ON , 253 = OFF
//gethkrabsenk Für HKR-Zeitschaltung eingestellte Spartemperatur Temperatur-Wert in 0,5 °C,
//  Wertebereich: 16 – 56, 8 bis 28°C, 16 <= 8°C, 17 = 8,5°C...... 56 >= 28°C, 254 = ON , 253 = OFF
//sethkrtsoll HKR Solltemperatur einstellen. Temperatur-Wert in 0,5 °C,
// Wertebereich: 16 – 56
//  Wertebereich: 16 – 56, 8 bis 28°C, 16 <= 8°C, 17 = 8,5°C...... 56 >= 28°C, 254 = ON , 253 = OFF

/*
 * Get list getThermostatList - polyfill
Set target temperature setTempTarget, supports 'ON'/'OFF' to enable/disable thermostat
Get target temperature getTempTarget
Get comfort temperature getTempComfort
Get night temperature getTempNight
Get battery charge getBatteryCharge (uses UI scraping, may be unstable)
Get window open getWindowOpen (uses UI scraping, may be unstable)
WLAN functions
Get the guest wlan settings getGuestWlan
Set the guest wlan setGuestWlan

Get the Fritz!OS version getOSVserion
Get the session ID getSessionID
Get device list as XML getDeviceListInfos >Fritz!OS 6.10
Get device list getDeviceList >Fritz!OS 6.10
Get device list with filter criteria applied getDeviceListFiltered >Fritz!OS 6.10
Get device getDevice >Fritz!OS 6.10
Get temperature getTemperature - polyfill
Get presence getPresence - polyfill
*/


FritzResult FritzApi::executeRequest(String request) {
  int lastError = 0;
  FritzResult result;
  for (int retries = 0; retries < 3; retries++) {
    http.begin("http://" + String(_ip) + "/webservices/homeautoswitch.lua?switchcmd=" + request);
    int retCode = http.GET();
    if (retCode == 200) {
      result.response = http.getString();
      result.httpStatus = 200;
      return result;
    } else {
      logPrintln("Error executing request: " + retCode);
      init();
      lastError=retCode;
    }
  }
  result.response = http.getString();
  result.httpStatus = lastError;
  logPrintln("Request " + request + " failed 3x. Last Error: " + lastError);
  return result;
}

void FritzApi::setDebugLog( Print &print ) { 
     logger = &print; //operate on the adress of print
}

void FritzApi::logPrintln( String entry ) { 
  if (logger) {
    logger->println(entry);
  }
}

void FritzApi::mapResultToIntStatus(FritzResult* result) {
  if (result->httpStatus != 200) {
    result->statusCode = -1;
  } else if (result->response == "inval") {
    result->statusCode = -2;
  } else {
    result->statusCode = atoi(result->response.c_str());
  }
}

